#include "plan_parser.h"
#include <util/utilities.h>
#include <util/data_utilities.h>

#include <boost/property_tree/json_parser.hpp>
#include <boost/tokenizer.hpp>
#include <boost/foreach.hpp>
#include <iostream>
#include <sstream>
#include <operators/support/aggregate_id.h>

#include <operators/sql_input.h>
#include <operators/secure_sql_input.h>
#include <operators/zk_sql_input.h>
#include <operators/sort.h>
#include <operators/nested_loop_aggregate.h>
#include <operators/group_by_aggregate.h>
#include <operators/scalar_aggregate.h>
#include <operators/keyed_join.h>
#include <operators/sort_merge_join.h>
#include <operators/basic_join.h>
#include <operators/filter.h>
#include <operators/project.h>
#include <parser/expression_parser.h>

using namespace vaultdb;
using boost::property_tree::ptree;


template<typename B>
PlanParser<B>::PlanParser(const string &db_name, const string & sql_file, const string & json_file,
                          const int &limit) : db_name_(db_name), input_limit_(limit) {
    parseSqlInputs(sql_file);
    parseSecurePlan(json_file);
}

// for ZK plans
template<typename B>
PlanParser<B>::PlanParser(const string &db_name, const string & json_file, const int &limit) : db_name_(db_name), input_limit_(limit), zk_plan_(true) {
    parseSecurePlan(json_file);

}

template<typename B>
Operator<B> *PlanParser<B>::parse(const string &db_name, const string &json_file, const int &limit) {
    PlanParser p(db_name, json_file, limit);
    return p.getRoot();
}

template<typename B>
Operator<B> *PlanParser<B>::parse(const std::string & db_name, const string & sql_file, const string & json_file, const int & limit) {
    PlanParser p(db_name, sql_file, json_file, limit);
    return p.root_;
}



template<typename B>
void PlanParser<B>::parseSqlInputs(const std::string & sql_file) {

    vector<std::string> lines = DataUtilities::readTextFile(sql_file);

    std::string query;
    int query_id = 0;
    bool init = false;
    B has_dummy = false;
    pair<int, SortDefinition> input_parameters; // operator_id, sorting info (if applicable)

    for(vector<string>::iterator pos = lines.begin(); pos != lines.end(); ++pos) {

        if((*pos).substr(0, 2) == "--") { // starting a new query
            if(init) { // skip the first one
                has_dummy = (query.find("dummy_tag") != std::string::npos);
                bool tmp =  (query.find("dummy_tag") != std::string::npos);
                query_id = input_parameters.first;

                operators_[query_id] = createInputOperator(query, input_parameters.second, has_dummy, tmp);
                operators_.at(query_id)->setOperatorId(query_id);

            }
            // set up the next header
            input_parameters = parseSqlHeader(*pos);
            query = "";
            init = true;
        }
        else {
            query += *pos + " ";
        }
    }

    // output the last one
    has_dummy = (query.find("dummy") != std::string::npos);
    bool tmp =  (query.find("dummy_tag") != std::string::npos);

    query_id = input_parameters.first;

    operators_[query_id] = createInputOperator(query, input_parameters.second,  has_dummy, tmp);
    operators_.at(query_id)->setOperatorId(query_id);

}

template<typename B>
void PlanParser<B>::parseSecurePlan(const string & plan_file) {
    stringstream ss;
    std::vector<std::string> json_lines = DataUtilities::readTextFile(plan_file);
    for(vector<string>::iterator pos = json_lines.begin(); pos != json_lines.end(); ++pos)
        ss << *pos << endl;

    boost::property_tree::ptree pt;
    boost::property_tree::read_json(ss, pt);


    BOOST_FOREACH(boost::property_tree::ptree::value_type &v, pt.get_child("rels."))
                {
                    assert(v.first.empty()); // array elements have no names

                    boost::property_tree::ptree inputs = v.second.get_child("id");
                    int operator_id = v.second.get_child("id").template get_value<int>();
                    string op_name =  (std::string) v.second.get_child("relOp").data();
                    parseOperator(operator_id, op_name, v.second);
                }
}



template<typename B>
void PlanParser<B>::parseOperator(const int &operator_id, const string &op_name, const ptree & tree) {

    Operator<B> *op = nullptr;

    if(operator_id > 1)
    {
        Operator<B> *child = getChildOperator(operator_id, tree);
        QuerySchema child_schema = child->getOutputSchema();
    }

    if(op_name == "LogicalValues") return; // handled in createInput
    if(op_name == "LogicalSort")   op = parseSort(operator_id, tree);
    if(op_name == "LogicalAggregate")  op = parseAggregate(operator_id, tree);
    if(op_name == "LogicalJoin")  op = parseJoin(operator_id, tree);
    if(op_name == "LogicalProject")  op = parseProjection(operator_id, tree);
    if(op_name == "LogicalFilter")  op = parseFilter(operator_id, tree);
    if(op_name == "JdbcTableScan")  op = parseSeqScan(operator_id, tree);

    if(op != nullptr) {
        operators_[operator_id] = op;
        operators_.at(operator_id)->setOperatorId(operator_id);

        root_ = op;
    }
    else
        throw std::invalid_argument("Unknown operator type: " + op_name);



}


template<typename B>
Operator<B> *PlanParser<B>::parseSort(const int &operator_id, const boost::property_tree::ptree &sort_tree) {

    boost::property_tree::ptree sort_payload = sort_tree.get_child("collation");
    SortDefinition sort_definition;
    int limit = -1;

    for (ptree::const_iterator it = sort_payload.begin(); it != sort_payload.end(); ++it) {
        ColumnSort cs;
        cs.first = it->second.get_child("field").get_value<int>(); // field_idx
        std::string direction_str =    it->second.get_child("direction").get_value<std::string>();
        cs.second = (direction_str == "ASCENDING") ? SortDirection::ASCENDING : SortDirection::DESCENDING;
        sort_definition.push_back(cs);
    }



    if(sort_tree.count("fetch") > 0) {
         limit = sort_tree.get_child("fetch.literal").template get_value<int>();
        // if we have a LIMIT clause, we need to sort on dummy tag first so that we output only real values
        if(sort_definition[0].first != -1) {
            sort_definition.insert(sort_definition.begin(), ColumnSort(-1, SortDirection::ASCENDING));
        }

    }

    Operator<B> *child = getChildOperator(operator_id, sort_tree);

    return new Sort<B>(child, sort_definition, limit);


}

template<typename B>
Operator<bool> *PlanParser<B>::createInputOperator(const string &sql, const SortDefinition &collation, const bool &has_dummy_tag, const bool & plain_has_dummy_tag) {
    size_t limit = (input_limit_ < 0) ? 0 : input_limit_;

    return new SqlInput(db_name_, sql, plain_has_dummy_tag, collation, limit);
}

template<typename B>
Operator<emp::Bit> *PlanParser<B>::createInputOperator(const string &sql, const SortDefinition &collation, const emp::Bit &has_dummy_tag, const bool & plain_has_dummy_tag) {
    size_t limit = (input_limit_ < 0) ? 0 : input_limit_;

    if(zk_plan_) {
        return new ZkSqlInput(db_name_, sql, plain_has_dummy_tag, collation,  limit);
    }

    return new SecureSqlInput(db_name_, sql, plain_has_dummy_tag, collation,  limit);

}



template<typename B>
Operator<B> *PlanParser<B>::parseAggregate(const int &operator_id, const boost::property_tree::ptree &aggregate_json) {

    // parse the aggregators
    std::vector<int32_t> group_by_ordinals;
    vector<ScalarAggregateDefinition> aggregators;
    int cardBound = -1;

    if(aggregate_json.count("group") > 0) {
        ptree group_by = aggregate_json.get_child("group.");

        for (ptree::const_iterator it = group_by.begin(); it != group_by.end(); ++it) {

            int ordinal = it->second.get_value<int>();
            group_by_ordinals.push_back(ordinal);
        }
    }

    // Parse Cardinality Bound info from JSON
    if(aggregate_json.count("cardBound") > 0)
        cardBound = aggregate_json.get_child("cardBound").template get_value<int>();

    boost::property_tree::ptree agg_payload = aggregate_json.get_child("aggs");



    for (ptree::const_iterator it = agg_payload.begin(); it != agg_payload.end(); ++it) {

        ScalarAggregateDefinition s;
        std::string agg_type_str = it->second.get_child("agg.kind").get_value<std::string>();
        s.type = Utilities::getAggregateId(agg_type_str);

        // operands
        ptree::const_iterator operand_pos = it->second.get_child("operands.").begin();
        ptree::const_iterator operand_end = it->second.get_child("operands.").end();

        s.ordinal = (operand_pos != operand_end) ? operand_pos->second.get_value<int>() : -1; // -1 for *, e.g. COUNT(*)
        s.alias = it->second.get_child("name").template get_value<std::string>();
        s.is_distinct = (it->second.get_child("distinct").template get_value<std::string>() == "false") ? false : true;

        assert(s.is_distinct == false); // distinct not yet implemented

        aggregators.push_back(s);
    }

    Operator<B> *child = getChildOperator(operator_id, aggregate_json);


    if(!group_by_ordinals.empty()) {
        if(cardBound > 0)
            return new NestedLoopAggregate<B>(child, group_by_ordinals, aggregators, cardBound);
        else {
            // if sort not aligned, insert a sort op
            SortDefinition child_sort = child->getSortOrder();
            if (!GroupByAggregate<B>::sortCompatible(child_sort, group_by_ordinals)) {
                // insert sort
                SortDefinition child_sort;
                for (uint32_t idx: group_by_ordinals) {
                    child_sort.template emplace_back(ColumnSort(idx, SortDirection::ASCENDING));
                }
                child = new Sort<B>(child, child_sort);
                support_ops_.template emplace_back(child);
            }
            return new GroupByAggregate<B>(child, group_by_ordinals, aggregators);
        }
    }
    else {

        return new ScalarAggregate<B>(child, aggregators);
    }


}

template<typename B>
Operator<B> *PlanParser<B>::parseJoin(const int &operator_id, const ptree &join_tree) {
    boost::property_tree::ptree join_condition_tree = join_tree.get_child("condition");

    ptree input_list = join_tree.get_child("inputs.");
    ptree::const_iterator it = input_list.begin();
    int lhs_id = it->second.get_value<int>();
    Operator<B> *lhs  = operators_.at(lhs_id);
    ++it;
    int rhs_id = it->second.get_value<int>();
    Operator<B> *rhs  = operators_.at(rhs_id);

    Expression<B> *join_condition = ExpressionParser<B>::parseExpression(join_condition_tree, lhs->getOutputSchema(), rhs->getOutputSchema());

    // for basic join and keyed join with fkey on lhs:
    SortDefinition sort_def = lhs->getSortOrder();

    // add offset for rhs
    for(auto pos : rhs->getSortOrder()) {
        sort_def.emplace_back(ColumnSort(pos.first + lhs->getOutputSchema().getFieldCount(), pos.second));
    }

    // if fkey designation exists, use this to create keyed join
    // key: foreignKey
    if(join_tree.count("foreignKey") > 0) {
        int foreign_key = join_tree.get_child("foreignKey").template get_value<int>();

        if(foreign_key == 1) {
            // switch sort orders
            sort_def.clear();
            // rhs --> lhs
            for(auto pos : rhs->getSortOrder()) {
                sort_def.emplace_back(ColumnSort(pos.first + lhs->getOutputSchema().getFieldCount(), pos.second));
            }

            for(auto pos : lhs->getSortOrder()) {
                sort_def.emplace_back(pos);
            }

        }

        if(join_tree.count("operator-algorithm") > 0) {
            string joinType = join_tree.get_child("operator-algorithm").template get_value<string>();
            if (joinType == "sort-merge-join")
                return new SortMergeJoin<B>(lhs, rhs, foreign_key, join_condition);
            return new KeyedJoin<B>(lhs, rhs, foreign_key, join_condition, sort_def);


        }
        else { // if unspecified but FK, use KeyedJoin
            return new KeyedJoin<B>(lhs, rhs, foreign_key, join_condition, sort_def);
        }
    }


    return new BasicJoin<B>(lhs, rhs, join_condition, sort_def);

}

template<typename B>
Operator<B> *PlanParser<B>::parseFilter(const int &operator_id, const ptree &pt) {

    boost::property_tree::ptree filter_condition_tree = pt.get_child("condition");
    Operator<B> *child = getChildOperator(operator_id, pt);
    Expression<B> *filter_condition = ExpressionParser<B>::parseExpression(filter_condition_tree,
                                                                                  child->getOutputSchema());
     return new Filter<B>(child, filter_condition);
}

template<typename B>
Operator<B> *PlanParser<B>::parseProjection(const int &operator_id, const ptree &project_tree) {

    Operator<B> *child = getChildOperator(operator_id, project_tree);
    QuerySchema child_schema = child->getOutputSchema();

    ptree output_fields = project_tree.get_child("fields");
    vector<string> output_names;

    for (ptree::const_iterator it = output_fields.begin(); it != output_fields.end(); ++it)  {
        output_names.emplace_back(it->second.data());
    }


    ExpressionMapBuilder<B>  builder(child_schema);
    ptree expressions = project_tree.get_child("exprs");
    uint32_t  dst_ordinal = 0;

    for (ptree::const_iterator it = expressions.begin(); it != expressions.end(); ++it) {
        Expression<B> *expr = ExpressionParser<B>::parseExpression(it->second, child_schema);
        builder.addExpression(expr, dst_ordinal);

      ++dst_ordinal;
    }
    Project<B> *p =  new Project<B>(child, builder.getExprs());
    QuerySchema schema = p->getOutputSchema();
    // add field names
    if(output_names.size() == schema.getFieldCount()) {
        for(int i = 0; i < schema.getFieldCount(); ++i) {
            QueryFieldDesc f = schema.getField(i);
            f.setName(f.getTableName(), output_names[i]);
            schema.putField(f);
        }
        p->setSchema(schema);
    }

    return p;
}

template<typename B>
Operator<B> *PlanParser<B>::parseSeqScan(const int & operator_id, const boost::property_tree::ptree &seq_scan_tree) {

    ptree::const_iterator table_name_start = seq_scan_tree.get_child("table.").begin();
    string table_name = table_name_start->second.get_value<std::string>();
    // order by to make truncated sets reproducible
    string sql = "SELECT * FROM " + table_name + " ORDER BY (1), (2), (3) ";
    return createInputOperator(sql, SortDefinition(), B(false), false);
}



// *** Utilities ***

// child is always the "N-1" operator if unspecified, i.e., if my_op_id is 5, then it is 4.
template<typename B>
Operator<B> *PlanParser<B>::getChildOperator(const int &my_operator_id, const boost::property_tree::ptree &pt) const {

    if(pt.count("inputs") > 0) {
        ptree input_list = pt.get_child("inputs");
        ptree::const_iterator it = input_list.begin();
        int parent_id = it->second.get_value<int>();
        Operator<B> *parent_operator  = operators_.at(parent_id);
        return parent_operator;
    }

    int child_id = my_operator_id - 1;
    if(operators_.find(child_id) != operators_.end())
        return operators_.find(child_id)->second;

    throw new std::invalid_argument("Missing operator id " + std::to_string(child_id) + Utilities::getStackTrace());
}







template<typename B>
const std::string PlanParser<B>::truncateInput(const std::string sql) const {
    std::string query = sql;
    if(input_limit_ > 0) {
        query = "SELECT * FROM (" + sql + ") input LIMIT " + std::to_string(input_limit_);
    }
    return query;
}

// examples (from TPC-H Q1, Q3):
// 0, collation: (0 ASC, 1 ASC)
// 1, collation: (0 ASC, 2 DESC, 3 ASC)
//   (above actually all ASC in tpc-h, DESC for testing)

template<typename B>
pair<int, SortDefinition> PlanParser<B>::parseSqlHeader(const string &header) {
    int comma_idx = header.find( ',');
    int operator_id = std::atoi(header.substr(3, comma_idx-3).c_str()); // chop off "-- "

    pair<int, SortDefinition> result;
    result.first = operator_id;
    SortDefinition output_collation;


    if(header.find("collation") != string::npos) {
        int sort_start = header.find('(');
        int sort_end = header.find(')');
        string collation = header.substr(sort_start + 1, sort_end - sort_start - 1);

        boost::tokenizer<boost::escaped_list_separator<char> > tokenizer(collation);
        for(boost::tokenizer<boost::escaped_list_separator<char> >::iterator beg=tokenizer.begin(); beg!=tokenizer.end();++beg) {
            boost::tokenizer<> sp(*beg); // space delimited
            boost::tokenizer<>::iterator  entries = sp.begin();

            int ordinal = std::atoi(entries->c_str());
            std::string direction = *(++entries);
            assert(direction == "ASC" || direction == "DESC");
            ColumnSort sort(ordinal, (direction == "ASC") ? SortDirection::ASCENDING : SortDirection::DESCENDING);
            output_collation.emplace_back(sort);
        }
    }

    result.second = output_collation;

    return result;
}

template<typename B>
Operator<B> *PlanParser<B>::getOperator(const int &op_id) {
    return operators_.find(op_id)->second;
}



template class vaultdb::PlanParser<bool>;
template class vaultdb::PlanParser<emp::Bit>;
