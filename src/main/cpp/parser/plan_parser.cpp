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
#include <operators/group_by_aggregate.h>
#include <operators/scalar_aggregate.h>
#include <operators/keyed_join.h>
#include <operators/basic_join.h>
#include <operators/filter.h>
#include <operators/project.h>
#include <operators/join.h>
#include <parser/expression_parser.h>

using namespace vaultdb;
using boost::property_tree::ptree;


template<typename B>
PlanParser<B>::PlanParser(const std::string &db_name, std::string plan_name, const int & limit) : db_name_(db_name), input_limit_(limit) {
    std::string sql_file = Utilities::getCurrentWorkingDirectory() + "/conf/plans/queries-" + plan_name + ".sql";
    std::string plan_file = Utilities::getCurrentWorkingDirectory() + "/conf/plans/mpc-" + plan_name + ".json";

    parseSqlInputs(sql_file);
    parseSecurePlan(plan_file);



}

template<typename B>
PlanParser<B>::PlanParser(const string &db_name, std::string plan_name, NetIO * netio, const int & party, const int & limit):  db_name_(db_name), netio_(netio), party_(party), input_limit_(limit)
{
    std::string sql_file = Utilities::getCurrentWorkingDirectory() + "/conf/plans/queries-" + plan_name + ".sql";
    std::string plan_file = Utilities::getCurrentWorkingDirectory() + "/conf/plans/mpc-" + plan_name + ".json";

    parseSqlInputs(sql_file);
    parseSecurePlan(plan_file);

}

// ZK constructor
template<typename B>
PlanParser<B>::PlanParser(const std::string & db_name, const std::string plan_name, BoolIO<NetIO> *ios[], const size_t & zk_threads, const int & party, const int & limit) : db_name_(db_name), zk_threads_(zk_threads), ios_(ios), party_(party), input_limit_(limit), zk_plan_(true) {
        std::string plan_file = Utilities::getCurrentWorkingDirectory() + "/conf/plans/zk-" + plan_name + ".json";
        parseSecurePlan(plan_file);

}


template<typename B>
shared_ptr<Operator<B>> PlanParser<B>::parse(const string &db_name, const string &plan_name, const int & limit) {
    PlanParser p(db_name, plan_name, limit);
    return p.getRoot();
}

template<typename B>
shared_ptr<Operator<B>> PlanParser<B>::parse(const string &db_name, const string &plan_name, NetIO * netio, const int & party, const int & limit ) {
    PlanParser p(db_name, plan_name, netio, party, limit);
    return p.root_;
}

template<typename B>
shared_ptr<Operator<B>>
PlanParser<B>::parse(const string &db_name, const string &plan_name, BoolIO<NetIO> **ios, const size_t &zk_threads,
                     const int &party, const int &limit) {

    PlanParser p(db_name, plan_name, ios, zk_threads, party, limit);
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

    for(vector<std::string>::iterator pos = lines.begin(); pos != lines.end(); ++pos) {

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
    for(std::vector<std::string>::iterator pos = json_lines.begin(); pos != json_lines.end(); ++pos)
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
    shared_ptr<Operator<B> > op;
    // TODO: parse schema for add'l validation
    if(op_name == "LogicalValues") return; // handled in createInput
    if(op_name == "LogicalSort")   op = parseSort(operator_id, tree);
    if(op_name == "LogicalAggregate")  op = parseAggregate(operator_id, tree);
    if(op_name == "LogicalJoin")  op = parseJoin(operator_id, tree);
    if(op_name == "LogicalProject")  op = parseProjection(operator_id, tree);
    if(op_name == "LogicalFilter")  op = parseFilter(operator_id, tree);
    if(op_name == "JdbcTableScan")  op = parseSeqScan(operator_id, tree);

    if(op.get() != nullptr) {
        operators_[operator_id] = op;
        operators_.at(operator_id)->setOperatorId(operator_id);

        root_ = op;
    }
    else
        throw std::invalid_argument("Unknown operator type: " + op_name);



}


template<typename B>
std::shared_ptr<Operator<B>> PlanParser<B>::parseSort(const int &operator_id, const boost::property_tree::ptree &sort_tree) {

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

    const shared_ptr<Operator<B> > child = getChildOperator(operator_id, sort_tree);

    return shared_ptr<Operator<B> > (new Sort<B>(child.get(), sort_definition, limit));


}

template<typename B>
shared_ptr<Operator<bool>>
PlanParser<B>::createInputOperator(const string &sql, const SortDefinition &collation, const bool &has_dummy_tag, const bool & plain_has_dummy_tag) {
    size_t limit = (input_limit_ < 0) ? 0 : input_limit_;

    shared_ptr<SqlInput> sql_input(new SqlInput(db_name_, sql, plain_has_dummy_tag, collation, limit));
    return sql_input;
}

template<typename B>
shared_ptr<Operator<emp::Bit>>
PlanParser<B>::createInputOperator(const string &sql, const SortDefinition &collation, const emp::Bit &has_dummy_tag, const bool & plain_has_dummy_tag) {
    size_t limit = (input_limit_ < 0) ? 0 : input_limit_;

    if(zk_plan_) {
        shared_ptr<ZkSqlInput> input(new ZkSqlInput(db_name_, sql, plain_has_dummy_tag, collation, ios_, zk_threads_, party_, limit));
        return input;
    }

    shared_ptr<SecureSqlInput> input(new SecureSqlInput(db_name_, sql, plain_has_dummy_tag, collation, netio_, party_, limit));
    return input;

}



template<typename B>
std::shared_ptr<Operator<B>> PlanParser<B>::parseAggregate(const int &operator_id, const boost::property_tree::ptree &aggregate_json) {

    // parse the aggregators
    std::vector<int32_t> group_by_ordinals;
    vector<ScalarAggregateDefinition> aggregators;

    if(aggregate_json.count("group") > 0) {
        ptree group_by = aggregate_json.get_child("group.");

        for (ptree::const_iterator it = group_by.begin(); it != group_by.end(); ++it) {

            int ordinal = it->second.get_value<int>();
            group_by_ordinals.push_back(ordinal);
        }
    }
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

    shared_ptr<Operator<B> > child = getChildOperator(operator_id, aggregate_json);


    if(!group_by_ordinals.empty()) {
        // if sort not aligned, insert a sort op
        SortDefinition child_sort = child->getSortOrder();
        if(!GroupByAggregate<B>::sortCompatible(child_sort, group_by_ordinals)) {
            // insert sort
            SortDefinition child_sort;
            for(uint32_t idx : group_by_ordinals) {
                child_sort.template emplace_back(ColumnSort(idx, SortDirection::ASCENDING));
            }
            child = std::shared_ptr<Operator<B>> (new Sort<B>(child.get(), child_sort));
            support_ops_.template emplace_back(child);
        }
        return shared_ptr<Operator<B> >(new GroupByAggregate<B>(child.get(), group_by_ordinals, aggregators));
    }
    else {

        return shared_ptr<Operator<B> >(new ScalarAggregate<B>(child.get(), aggregators));
    }


}

template<typename B>
std::shared_ptr<Operator<B>> PlanParser<B>::parseJoin(const int &operator_id, const ptree &join_tree) {
    boost::property_tree::ptree join_condition_tree = join_tree.get_child("condition");



    ptree input_list = join_tree.get_child("inputs.");
    ptree::const_iterator it = input_list.begin();
    int lhs_id = it->second.get_value<int>();
    shared_ptr<Operator<B> > lhs  = operators_.at(lhs_id);
    ++it;
    int rhs_id = it->second.get_value<int>();
    shared_ptr<Operator<B> > rhs  = operators_.at(rhs_id);

    QuerySchema schema = Join<B>::concatenateSchemas(lhs->getOutputSchema(), rhs->getOutputSchema());
    BoolExpression<B> join_condition = ExpressionParser<B>::parseBoolExpression(join_condition_tree, schema);


    // if fkey designation exists, use this to create keyed join
    // key: foreignKey
    if(join_tree.count("foreignKey") > 0) {
        int foreign_key = join_tree.get_child("foreignKey").template get_value<int>();
        shared_ptr<Operator<B> > op(new KeyedJoin<B>(lhs.get(), rhs.get(), foreign_key, join_condition));
        return op;

    }


    shared_ptr<Operator<B> > op(new BasicJoin<B>(lhs.get(), rhs.get(), join_condition));
    return op;


}

template<typename B>
std::shared_ptr<Operator<B>> PlanParser<B>::parseFilter(const int &operator_id, const ptree &pt) {

    boost::property_tree::ptree filter_condition_tree = pt.get_child("condition");
    std::shared_ptr<Operator<B> > child = getChildOperator(operator_id, pt);
    BoolExpression<B> filter_condition = ExpressionParser<B>::parseBoolExpression(filter_condition_tree,
                                                                                  child->getOutputSchema());
     shared_ptr<Operator<B> > op(new Filter<B>(child.get(), filter_condition));
     return op;
}

template<typename B>
std::shared_ptr<Operator<B>> PlanParser<B>::parseProjection(const int &operator_id, const ptree &project_tree) {

    shared_ptr<Operator<B> > child_operator = getChildOperator(operator_id, project_tree);
    QuerySchema child_schema = child_operator->getOutputSchema();

    ExpressionMapBuilder<B>  builder(child_schema);
    ptree expressions = project_tree.get_child("exprs");
    uint32_t src_ordinal, dst_ordinal = 0;

    for (ptree::const_iterator it = expressions.begin(); it != expressions.end(); ++it) {
        Expression<B> *expr = ExpressionParser<B>::parseExpression(it->second, child_schema);
//        std::cout << "Parsed expr: " << expr->toString() << std::endl;
//        std::cout << "Root addr: " << (size_t) ((GenericExpression<B> *) expr)->root_ << std::endl;
//
//        if(expr->kind()  == ExpressionKind::INPUT_REF) {
//            GenericExpression<B> expression_impl = *((GenericExpression<B> *) expr);
//            InputReferenceNode<B> input_ref  = *((InputReferenceNode<B> *) expression_impl.root_);
//            src_ordinal = input_ref.read_idx_;
//            builder.addMapping(src_ordinal, dst_ordinal);
//        }
//
//        std::cout << "Root addr: " << (size_t) ((GenericExpression<B> *) expr)->root_ << std::endl;

        builder.addExpression(expr, dst_ordinal);

      ++dst_ordinal;
    }

    shared_ptr<Project<B> > project(new Project<B>(child_operator.get(), builder.getExprs()));

    return project;
}

template<typename B>
std::shared_ptr<Operator<B>> PlanParser<B>::parseSeqScan(const int & operator_id, const boost::property_tree::ptree &seq_scan_tree) {

    ptree::const_iterator table_name_start = seq_scan_tree.get_child("table.").begin();
    string table_name = table_name_start->second.get_value<std::string>();
    // order by to make truncated sets reproducible
    string sql = "SELECT * FROM " + table_name + " ORDER BY (1), (2), (3) ";
    return createInputOperator(sql, SortDefinition(), B(false), false);
}



// *** Utilities ***

// child is always the "N-1" operator if unspecified, i.e., if my_op_id is 5, then it is 4.
template<typename B>
const std::shared_ptr<Operator<B> >
PlanParser<B>::getChildOperator(const int &my_operator_id, const boost::property_tree::ptree &pt) const {

    if(pt.count("inputs") > 0) {
        ptree input_list = pt.get_child("inputs");
        ptree::const_iterator it = input_list.begin();
        int parent_id = it->second.get_value<int>();
        shared_ptr<Operator<B>> parent_operator  = operators_.at(parent_id);
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
shared_ptr<Operator<B>> PlanParser<B>::getOperator(const int &op_id) {
    return operators_.find(op_id)->second;
}



template class vaultdb::PlanParser<bool>;
template class vaultdb::PlanParser<emp::Bit>;
