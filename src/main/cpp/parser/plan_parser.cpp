#include "plan_parser.h"
#include <util/utilities.h>
#include <util/data_utilities.h>

#include <boost/property_tree/json_parser.hpp>
#include <boost/tokenizer.hpp>
#include <boost/foreach.hpp>
#include <iostream>
#include <sstream>
#include <operators/support/aggregate_id.h>
#include "opt/operator_cost_model.h"
#include <operators/sql_input.h>
#include <operators/secure_sql_input.h>
#include <operators/zk_sql_input.h>
#include <operators/scalar_aggregate.h>
#include <operators/keyed_join.h>
#include <operators/sort_merge_join.h>
#include <operators/merge_join.h>
#include <operators/basic_join.h>
#include <operators/filter.h>
#include <operators/project.h>
#include <parser/expression_parser.h>
#include <operators/shrinkwrap.h>

using namespace vaultdb;
using boost::property_tree::ptree;


template<typename B>
PlanParser<B>::PlanParser(const string &db_name, const string & sql_file, const string & json_file,
                          const int &limit) : db_name_(db_name), input_limit_(limit) {
    parseSqlInputs(sql_file);
    parseSecurePlan(json_file);

   if(getAutoFlag())
        calculateAutoAggregate();
}

// for ZK plans
template<typename B>
PlanParser<B>::PlanParser(const string &db_name, const string & json_file, const int &limit) : db_name_(db_name), input_limit_(limit), zk_plan_(false) {
    json_only_ = true;

    parseSecurePlan(json_file);

    if(getAutoFlag())
        calculateAutoAggregate();

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
    tuple<int, SortDefinition, int> input_parameters; // operator_id, sorting info (if applicable)

    for(vector<string>::iterator pos = lines.begin(); pos != lines.end(); ++pos) {

        if((*pos).substr(0, 2) == "--") { // starting a new query
            if(init) { // skip the first one
                has_dummy = (query.find("dummy_tag") != std::string::npos);
                bool tmp =  (query.find("dummy_tag") != std::string::npos);
                query_id = get<0>(input_parameters);

                // Define Party if exists
                if(get<2>(input_parameters)>0)
                    operators_[query_id] = createInputOperator(query, get<1>(input_parameters), get<2>(input_parameters), has_dummy, tmp);
                else
                    operators_[query_id] = createInputOperator(query, get<1>(input_parameters), has_dummy, tmp);

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

    query_id = get<0>(input_parameters);

    // Define Party if exists
    if(get<2>(input_parameters)>0)
        operators_[query_id] = createInputOperator(query, get<1>(input_parameters), get<2>(input_parameters), has_dummy, tmp);
    else
        operators_[query_id] = createInputOperator(query, get<1>(input_parameters), has_dummy, tmp);

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

    if(op_name == "LogicalSort")   op = parseSort(operator_id, tree);
    if(op_name == "LogicalAggregate") { op = parseAggregate(operator_id, tree); }
    if(op_name == "LogicalJoin")  op = parseJoin(operator_id, tree);
    if(op_name == "LogicalProject")  op = parseProjection(operator_id, tree);
    if(op_name == "LogicalFilter")  op = parseFilter(operator_id, tree);
    if(op_name == "JdbcTableScan")  op = parseSeqScan(operator_id, tree);
    if(op_name == "ShrinkWrap")  op = parseShrinkwrap(operator_id, tree);

    if(op_name == "LogicalValues"){
        if(json_only_)
            parseLocalScan(operator_id, tree);
        else
            return;
    }
    else if(op != nullptr) {

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
Operator<bool> *PlanParser<B>::createInputOperator(const string &sql, const SortDefinition &collation, const int &input_party, const bool &has_dummy_tag, const bool & plain_has_dummy_tag) {
    size_t limit = (input_limit_ < 0) ? 0 : input_limit_;

    return new SqlInput(db_name_, sql, plain_has_dummy_tag, collation, input_party, limit);
}

template<typename B>
Operator<emp::Bit> *PlanParser<B>::createInputOperator(const string &sql, const SortDefinition &collation, const int &input_party, const emp::Bit &has_dummy_tag, const bool & plain_has_dummy_tag) {
    size_t limit = (input_limit_ < 0) ? 0 : input_limit_;

    if(zk_plan_) {
        return new ZkSqlInput(db_name_, sql, plain_has_dummy_tag, collation,  limit);
    }

    return new SecureSqlInput(db_name_, sql, plain_has_dummy_tag, input_party, limit, collation);

}

template<typename B>
void PlanParser<B>::calculateAutoAggregate() {

    // Define the aggregate types
    enum AggregateType {
        SMA,
        NLA,
    };

    // Initialize variables to store the minimum cost and corresponding combination
    size_t min_cost = ULONG_MAX;
    int min_combination = -1;

    // Get the number of aggregate operators
    int num_aggregates = sma_vector.size();

    // Iterate over all possible combinations
    for (int combination = 0; combination < (1 << num_aggregates); combination++) {
        // Initialize the cost for this combination
        size_t cost = 0;

        for (int i = 0; i < num_aggregates; i++) {
            // Determine the type of the i-th aggregate operator
            AggregateType type = (combination & (1 << i)) ? NLA : SMA;

            size_t cur_output_cardinality_;
            GroupByAggregate<B> *sma;
            // Fetch the correct operator and add its cost
            if (type == SMA) {
                // Check if SMA has sort
                if(sort_vector[i] != nullptr)
                    cost += OperatorCostModel::operatorCost((SecureOperator *) sort_vector[i]);

                sma = sma_vector[i];
                cost += OperatorCostModel::operatorCost((SecureOperator *) sma);

                // Propagate output cardinality
                cur_output_cardinality_ = sma->getOutputCardinality();

            } else {  // type == NestedLoopAggregate
                NestedLoopAggregate<B> *nla = nla_vector[i];
                cost += OperatorCostModel::operatorCost((SecureOperator *) nla);

                // Propagate output cardinality
                cur_output_cardinality_ = nla->getOutputCardinality();
            }

            // Get the operator id of the next aggregate operator (or the end if this is the last one)
            int next_op_id = (i + 1 < num_aggregates) ? agg_id[i + 1] : operators_.size();

            // Propagate the output cardinality to the operators between this aggregate and the next one
            for (int op_id = agg_id[i] + 1; op_id < next_op_id; op_id++) {
                Operator<B> *cur_op = operators_[op_id];
                cur_op->setOutputCardinality(cur_output_cardinality_);

                // Check if Sort after Aggregate, and does not need because it is already sorted by SMA
                if (cur_op->getTypeString() == "Sort" && type == SMA) {
                    SortDefinition cur_sort_order = cur_op->getSortOrder();
                    if (GroupByAggregate<B>::sortCompatible(cur_sort_order, sma->group_by_)) {
                        cur_output_cardinality_ = cur_op->getOutputCardinality();
                        continue;
                    }
                }
                cost += OperatorCostModel::operatorCost((SecureOperator *) cur_op);
                cur_output_cardinality_ = cur_op->getOutputCardinality();
            }
        }

        std::cout << "Cost : " << cost << ", combination : " << combination << "\n";

        // If this combination is cheaper than the current best, update the minimum cost and combination
        if (cost < min_cost) {
            min_cost = cost;
            min_combination = combination;
        }
    }

    // Create an instance of each operator for the minimum combination
    for (int i = 0; i < num_aggregates; i++) {
        // Determine the type of the i-th aggregate operator
        AggregateType type = (min_combination & (1 << i)) ? NLA : SMA;

        // Define the group by ordinals and aggregators variables
        std::vector<int32_t> group_by_ordinals;
        vector<ScalarAggregateDefinition> aggregators;
        Operator<B>* child = operators_[agg_id[i] - 1];

        // Fetch the correct operator and create a new instance
        if (type == SMA) {
            // if sort flag is set, create and link a new sort operator
            if(sort_vector[i] != nullptr){
                Sort<B>* sort_before_sma = sort_vector[i];
                SortDefinition sort_order = sort_before_sma->getSortOrder();
                Operator<B>* real_sort_before_sma = new Sort<B>(child, sort_order);
                support_ops_.emplace_back(real_sort_before_sma);
                child->setParent(real_sort_before_sma);
                real_sort_before_sma->setChild(child);
                child = real_sort_before_sma;
            }

            group_by_ordinals = sma_vector[i]->group_by_;
            aggregators = sma_vector[i]->aggregate_definitions_;
            bool check_sort = sma_vector[i]->check_sort_;

            GroupByAggregate<B> *real_sma = new GroupByAggregate<B>(child, group_by_ordinals, aggregators, check_sort);
            real_sma->setOutputCardinality( sma_vector[i]->getJsonOutputCardinality());
            child->setParent(real_sma);
            real_sma->setChild(child);
            operators_[agg_id[i]] = real_sma;

            if(agg_id[i] + 1 == operators_.size())
                root_ = real_sma;

        } else {  // type == NestedLoopAggregate
            group_by_ordinals = nla_vector[i]->group_by_;
            aggregators = nla_vector[i]->aggregate_definitions_;
            int cardBound = nla_vector[i]->getOutputCardinality();

            NestedLoopAggregate<B> *real_nla = new NestedLoopAggregate<B>(child, group_by_ordinals, aggregators, cardBound);
            child->setParent(real_nla);
            real_nla->setChild(child);
            operators_[agg_id[i]] = real_nla;

            if(agg_id[i] + 1 == operators_.size())
                root_ = real_nla;
        }

        operators_.at(agg_id[i])->setOperatorId(agg_id[i]);

        // set the parent of the next operator to this one
        if (agg_id[i] + 1 < operators_.size()){
            operators_[agg_id[i] + 1]->setChild(operators_[agg_id[i]]);
            operators_[agg_id[i]]->setParent(operators_[agg_id[i] + 1]);
        }

        // Get the operator id of the next aggregate operator (or the end if this is the last one)
        int next_op_id = (i + 1 < num_aggregates) ? agg_id[i + 1] : operators_.size();

        // Propagate the output cardinality to the operators between this aggregate and the next one
        for (int op_id = agg_id[i] + 1; op_id < next_op_id; op_id++) {
            Operator<B> *cur_op = operators_[op_id];
            cur_op->setOutputCardinality(operators_[agg_id[i]]->getOutputCardinality());
        }
    }

    // Delete SMA vector
    for (auto &sma : sma_vector) {
        if (sma != nullptr)
            delete sma;
    }

    // Delete NLA vector
    for (auto &nla : nla_vector) {
        if (nla != nullptr)
            delete nla;
    }

    // Delete Sort vector
    for (auto &sort : sort_vector) {
        if (sort != nullptr)
            delete sort;
    }
    // Clear and shrink SMA vector
    sma_vector.clear();
    sma_vector.shrink_to_fit();

    // Clear and shrink NLA vector
    nla_vector.clear();
    nla_vector.shrink_to_fit();

    // Clear and shrink Sort vector
    sort_vector.clear();
    sort_vector.shrink_to_fit();
}


template<typename B>
Operator<B> *PlanParser<B>::parseAggregate(const int &operator_id, const boost::property_tree::ptree &aggregate_json) {

    // parse the aggregators
    std::vector<int32_t> group_by_ordinals;
    vector<ScalarAggregateDefinition> aggregators;
    int cardinality_bound = -1;

    if(aggregate_json.count("group") > 0) {
        ptree group_by = aggregate_json.get_child("group.");

        for (ptree::const_iterator it = group_by.begin(); it != group_by.end(); ++it) {

            int ordinal = it->second.get_value<int>();
            group_by_ordinals.push_back(ordinal);
        }
    }

    // Parse Cardinality Bound info from JSON
    if(aggregate_json.count("cardBound") > 0)
        cardinality_bound = aggregate_json.get_child("cardBound").template get_value<int>();

    bool check_sort = true;
    if(aggregate_json.count("checkSort") > 0) {
        check_sort = aggregate_json.get_child("checkSort").template get_value<bool>();
    }

    string agg_algo;
    if(aggregate_json.count("operator-algorithm") > 0)
        agg_algo = aggregate_json.get_child("operator-algorithm").template get_value<string>();
    assert(agg_algo == "nested-loop-aggregate" || agg_algo == "sort-merge-aggregate" || agg_algo == "auto" || agg_algo == "");


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

        assert(!s.is_distinct); // distinct not yet implemented

        aggregators.push_back(s);
    }

    Operator<B> *child = getChildOperator(operator_id, aggregate_json);

    if(!group_by_ordinals.empty()) {

        // Use Cost Model to calculate NLA and SMA, pick more better one.
        if(agg_algo == "auto") {
            GroupByAggregate<B>* sma;
            NestedLoopAggregate<B>* nla;
            if(check_sort){
                // if sort not aligned, insert a sort op
                SortDefinition child_sort = child->getSortOrder();
                if (!GroupByAggregate<B>::sortCompatible(child_sort, group_by_ordinals)) {
                    // insert sort
                    SortDefinition child_sort;
                    for (uint32_t idx: group_by_ordinals) {
                        child_sort.template emplace_back(ColumnSort(idx, SortDirection::ASCENDING));
                    }
                    Sort<B> *sort_before_sma = new Sort<B>(child->clone(), child_sort);
                    sma = new GroupByAggregate<B>(sort_before_sma->clone(), group_by_ordinals, aggregators, check_sort);
                    sort_vector.push_back(sort_before_sma);
                }
                else{
                    sma = new GroupByAggregate<B>(child->clone(), group_by_ordinals, aggregators, check_sort);
                    sort_vector.push_back(nullptr);
                }
                nla = new NestedLoopAggregate<B>(child->clone(), group_by_ordinals, aggregators, cardinality_bound);
            }
            else {
                sma = new GroupByAggregate<B>(child->clone(), group_by_ordinals, aggregators, check_sort);
                sort_vector.push_back(nullptr);
                nla = new NestedLoopAggregate<B>(child->clone(), group_by_ordinals, aggregators, cardinality_bound);
            }

            sma->setJsonOutputCardinality(cardinality_bound);
            sma_vector.push_back(sma);
            nla_vector.push_back(nla);
            setAutoFlag(true);
            agg_id.push_back(operator_id);

            return sma;
        }

        if(cardinality_bound > 0 && (agg_algo == "nested-loop-aggregate" || agg_algo == ""))
            return new NestedLoopAggregate<B>(child, group_by_ordinals, aggregators, cardinality_bound);
        else if(!check_sort && (agg_algo == "sort-merge-aggregate" || agg_algo == ""))
            return new GroupByAggregate<B>(child, group_by_ordinals, aggregators, check_sort, cardinality_bound);
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
            return new GroupByAggregate<B>(child, group_by_ordinals, aggregators, check_sort, cardinality_bound);
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

    string join_type;
    if(join_tree.count("operator-algorithm") > 0)
        join_type = join_tree.get_child("operator-algorithm").template get_value<string>();

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

            // Use Cost Model to calculate NLJ and SMJ, pick more better one.
            if(join_type == "auto") {

                Operator<B>* smj = new SortMergeJoin<B>(lhs->clone(), rhs->clone(), foreign_key, join_condition->clone());
                Operator<B>* nlj = new KeyedJoin<B>(lhs->clone(), rhs->clone(), foreign_key, join_condition->clone(), sort_def);

                size_t SMJ_cost = OperatorCostModel::operatorCost((SecureOperator *) smj);
                size_t NLJ_cost = OperatorCostModel::operatorCost((SecureOperator *) nlj);
                join_type = (SMJ_cost < NLJ_cost) ? "sort-merge-join" : "nested-loop-join";

                cout << "smj cost : " << SMJ_cost << ", nlj cost : " << NLJ_cost << ", join type : " << join_type << endl;
                delete smj;
                delete nlj;

                if (join_type == "sort-merge-join") {
                    return new SortMergeJoin<B>(lhs, rhs, foreign_key, join_condition);
                }
                else {
                    return new KeyedJoin<B>(lhs, rhs, foreign_key, join_condition, sort_def);
                }

            }
            else if (join_type == "sort-merge-join")
                return new SortMergeJoin<B>(lhs, rhs, foreign_key, join_condition);

            return new KeyedJoin<B>(lhs, rhs, foreign_key, join_condition, sort_def);

        }
        else { // if unspecified but FK, use KeyedJoin
            return new KeyedJoin<B>(lhs, rhs, foreign_key, join_condition, sort_def);
        }
    }

    if (join_type == "merge-join") {
        if(join_tree.count("dummy-handling") > 0 && join_tree.get_child("dummy-handling").template get_value<string>() == "OR")
            return new MergeJoin<B>(lhs, rhs, join_condition, SortDefinition(), true);
        else
            return new MergeJoin<B>(lhs, rhs, join_condition, SortDefinition(), false);
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

template<typename B>
void PlanParser<B>::parseLocalScan(const int & operator_id, const boost::property_tree::ptree &local_scan_tree) {
    string sql = "";
    int input_party = 0;
    bool multiple_sort_ = false;
    B plain_has_dummy_tag = false;

    if(local_scan_tree.count("sql") > 0)
        sql = local_scan_tree.get_child("sql").template get_value<string>();

    if(local_scan_tree.count("party") > 0)
        input_party = local_scan_tree.get_child("party").template get_value<int>();

    plain_has_dummy_tag =  (sql.find("dummy_tag") != std::string::npos);
    bool tmp = (sql.find("dummy_tag") != std::string::npos);

    int collationIndex = 1; // Start index for multiple collations
    bool foundMultipleSorts = false;
    while (true) {
        std::string collationKey = "collation-" + std::to_string(collationIndex);
        boost::optional<const boost::property_tree::ptree&> collationNode = local_scan_tree.get_child_optional(collationKey);

        if(collationIndex > 1)// At least two collation entries were found
            foundMultipleSorts = true;

        if (!collationNode) {
            break; // No more collations found
        }

        SortDefinition sort_definition; // Define a sort definition for this collation

        for (const auto& collationEntry : *collationNode) {
            ColumnSort cs;
            cs.first = collationEntry.second.get_child("field").get_value<int>();
            std::string direction_str = collationEntry.second.get_child("direction").get_value<std::string>();
            cs.second = (direction_str == "ASCENDING") ? SortDirection::ASCENDING : SortDirection::DESCENDING;
            sort_definition.push_back(cs); // Push the ColumnSort to the current SortDefinition
        }

        interesting_sort_orders_[operator_id].push_back(sort_definition);

        collationIndex++; // Move to the next collation index
    }
    // If there is only one sort, parse it
    if(!foundMultipleSorts){
        boost::property_tree::ptree sort_payload = local_scan_tree.get_child("collation");

        SortDefinition sort_definition; // Define a single sort definition

        for (ptree::const_iterator it = sort_payload.begin(); it != sort_payload.end(); ++it) {
            ColumnSort cs;
            cs.first = it->second.get_child("field").get_value<int>(); // field_idx
            std::string direction_str = it->second.get_child("direction").get_value<std::string>();
            cs.second = (direction_str == "ASCENDING") ? SortDirection::ASCENDING : SortDirection::DESCENDING;
            sort_definition.push_back(cs); // Push the ColumnSort to the single SortDefinition
        }
        interesting_sort_orders_[operator_id].push_back(sort_definition);
    }
    if(input_party > 0)
        operators_[operator_id] = createInputOperator(sql, interesting_sort_orders_[operator_id].front(), input_party, plain_has_dummy_tag, tmp);
    else
        operators_[operator_id] = createInputOperator(sql, interesting_sort_orders_[operator_id].front(), plain_has_dummy_tag, tmp);
    operators_.at(operator_id)->setOperatorId(operator_id);
    root_ = operators_[operator_id];
}


template<typename B>
Operator<B> *PlanParser<B>::parseShrinkwrap(const int & operator_id, const boost::property_tree::ptree &pt) {

    ptree input_list = pt.get_child("inputs.");
    ptree::const_iterator it = input_list.begin();
    int op_id = it->second.get_value<int>();
    Operator<B> *op = operators_.at(op_id);

    size_t output_cardinality = pt.get_child("output_cardinality").template get_value<int>();

    return new Shrinkwrap<B>(op->getOutput(), output_cardinality);
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
tuple<int, SortDefinition, int> PlanParser<B>::parseSqlHeader(const string &header) {
    int comma_idx = header.find( ',');
    int operator_id = std::atoi(header.substr(3, comma_idx-3).c_str()); // chop off "-- "

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

    //result.second = output_collation;
    int party_number = 0;

    if (header.find("party") != std::string::npos) {
        int party_start = header.find("party:");
        int party_end = header.find('\n', party_start);
        std::string party_str = header.substr(party_start + 6, party_end - party_start - 6);
        party_number = std::atoi(party_str.c_str());
    }

    tuple<int, SortDefinition, int> result = make_tuple(operator_id, output_collation, party_number);
    return result;
}

template<typename B>
Operator<B> *PlanParser<B>::getOperator(const int &op_id) {
    return operators_.find(op_id)->second;
}


// optimize a left-deep tree with a fixed join order
// optimize operator implementations and sort placement

// Plan enumerator to iteratively try all potential impls for each operator declared in query plan.
//  * For leafs/LogicalValues cycle through all interesting sort orders then recurse to next tree layer
//  * For joins cycle through NLJ vs SMJ (assume no partial sort opt for now)
//  * For aggs try NLA and SMA (if a card bound exists)
//  * For SMA check if we need sort before op.
//  * After SMA assess if we still need any sort that's a parent to it.
//  * Important : Since we get leaf from operators_.begin, json file needs to be in the increasing order
//  * For example, if OP#0, OP#1 are logical values, OP#2 join should have lhs as 0, and rhs as 1, not vice versa.

template<typename B>
Operator<B> *PlanParser<B>::optimizeTree() {
    Operator<B> *leaf = operators_.begin()->second;
    optimizeTreeHelper(leaf);
    return nullptr;

}

// helper function for recursing up operator tree
template<typename B>
void PlanParser<B>::optimizeTreeHelper(Operator<B> *op) {

    Operator<B> *node = op->clone();

    switch(op->getType()) {
        case OperatorType::SQL_INPUT:
        case OperatorType::SECURE_SQL_INPUT:
        case OperatorType::ZK_SQL_INPUT:
        case OperatorType::TABLE_INPUT: {
            // first try with given sort order (if any) and empty set unconditionally
            auto sorts = getCollations(node);
            for (auto &sort: sorts) {
                node->setSortOrder(sort);
                optimizeTree_operators_[node->getOperatorId()] = node;

                std::cout << node->toString() << endl;
                recurseNode(node);
            }
            break;
        }
        case OperatorType::KEYED_NESTED_LOOP_JOIN:
        case OperatorType::SORT_MERGE_JOIN: {

            // for each rhs leaf collation cycle through its collations and then try both SMJ and NLJ with each collation
            recurseJoin(node);
            break;
        }
        case OperatorType::NESTED_LOOP_AGGREGATE:
        case OperatorType::SORT_MERGE_AGGREGATE: {
            // for each agg try NLA and SMA
            // for SMA check if we need sort before op, also check if we have any extra sorts that are unneeded?
            node->setSortOrder(node->getChild()->getSortOrder());

            std::cout << "Agg :  Order by: " << DataUtilities::printSortDefinition(node->getSortOrder())  << ", ";
            recurseAgg(node);
            break;
        }
        case OperatorType::SORT:
            // Sort is always same with sort order
            node->setSortOrder(op->getSortOrder());
            recurseNode(node);
            break;
        default:
            // Same with child's sort order
            node->setSortOrder(node->getChild()->getSortOrder());
            optimizeTree_operators_[node->getOperatorId()] = node;

            std::cout << node->toString() << endl;
            recurseNode(node);
            break;
    }

    delete node;
}


template<typename B>
void PlanParser<B>::recurseNode(Operator<B> *op) {

    auto original = operators_.at(op->getOperatorId());
    auto parent = original->getParent();
    // at the root node
    if(parent == nullptr) {
        size_t plan_cost = op->planCost();
        std::cout << op->printTree() << endl;
        std::cout << "Cost : " << std::to_string(plan_cost) << "\n" << "----------------------------" << std::endl;

        if(plan_cost < min_plan_cost_) {
            min_plan_cost_ = plan_cost;
            if(min_cost_plan_ != nullptr) {
                delete min_cost_plan_;
            }
            min_cost_plan_ = op->clone();
        }
        return;
    }

    // else recurse up the tree
    optimizeTreeHelper(parent);

}


template<typename B>
Operator<B> *PlanParser<B>::fetchLeaf(Operator<B> *op) {
    while(!op->isLeaf()) {
        op = op->getChild();
    }
    return op;
}

template<typename B>
void PlanParser<B>::recurseJoin(Operator<B> *join) {
    auto rhs_leaf = fetchLeaf(join->getChild(1));
    auto rhs_sorts = getCollations(rhs_leaf);
    for(auto &sort: rhs_sorts) {
        rhs_leaf->setSortOrder(sort);
        optimizeTree_operators_[rhs_leaf->getOperatorId()] = rhs_leaf;

        std::cout << rhs_leaf->toString() << endl;
        // TODO : If there is more than one op in rhs, we need to set sort order for all of them.

        // TODO: add method to propagate sort to parent nodes
        // recommend doing this in Operator<B> - like adding an updateSortOrder() method that uses the same logic in operator constructor
        //recurseNode(join);

        if(join->getType() == OperatorType::KEYED_NESTED_LOOP_JOIN) {
            // For KeyedJoin, sort order is same with foreign key's sort order
            KeyedJoin<B> *kj = (KeyedJoin<B> *)join;

            // Childs are pointing original parent, Need to fix those to point new parent.
            kj->setChild(optimizeTree_operators_.at(kj->getChild(0)->getOperatorId()));
            kj->setChild(optimizeTree_operators_.at(kj->getChild(1)->getOperatorId()), 1);
            optimizeTree_operators_.at(kj->getChild(0)->getOperatorId())->setParent(kj);
            optimizeTree_operators_.at(kj->getChild(1)->getOperatorId())->setParent(kj);

            kj->setSortOrder(join->getChild(kj->foreignKeyChild())->getSortOrder());

            std::cout << kj->toString() << endl;

            recurseNode(kj);

            // For SMJ, sort order is same with sort key's order
            SortMergeJoin j(join->getChild(0)->clone(), join->getChild(1)->clone(), kj->foreignKeyChild(), kj->getPredicate());

            std::cout << j.toString() << endl;

            recurseNode(&j);
        }
        else {
            SortMergeJoin<B> *smj = (SortMergeJoin<B> *) join;

            // Childs are pointing original parent, Need to fix those to point new parent.
            smj->setChild(optimizeTree_operators_.at(smj->getChild(0)->getOperatorId()));
            smj->setChild(optimizeTree_operators_.at(smj->getChild(1)->getOperatorId()), 1);
            optimizeTree_operators_.at(smj->getChild(0)->getOperatorId())->setParent(smj);
            optimizeTree_operators_.at(smj->getChild(1)->getOperatorId())->setParent(smj);

//            smj->setSortOrder(join->getChild(smj->foreignKeyChild())->getSortOrder());
            std::cout << smj->toString() << endl;
            recurseNode(smj);


            KeyedJoin j(join->getChild(0)->clone(), join->getChild(1)->clone(), smj->foreignKeyChild(), smj->getPredicate());
            recurseNode(&j);
        }
    }
}

template<typename B>
void PlanParser<B>::recurseAgg(Operator<B> *agg) {

    Operator<B> *child = agg->getChild();
    if (agg->getType() == OperatorType::NESTED_LOOP_AGGREGATE) {
        NestedLoopAggregate<B> *nla = (NestedLoopAggregate<B> *) agg;
        std::vector<int32_t> group_by_ordinals = nla->group_by_;
        // if sort not aligned, insert a sort op
        SortDefinition child_sort = child->getSortOrder();
        if (!GroupByAggregate<B>::sortCompatible(child_sort, group_by_ordinals)) {
            // insert sort
            SortDefinition child_sort;
            for (uint32_t idx: group_by_ordinals) {
                child_sort.template emplace_back(ColumnSort(idx, SortDirection::ASCENDING));
            }
            Sort<B> *sort_before_sma = new Sort<B>(child->clone(), child_sort);
            GroupByAggregate a(sort_before_sma->clone(), group_by_ordinals, nla->aggregate_definitions_,
                               true);
            child->setParent(sort_before_sma);
            sort_before_sma->setChild(child);
            sort_before_sma->setParent(&a);
            a.setChild(sort_before_sma);
            recurseNode(&a);
        } else {
            GroupByAggregate a(child->clone(), group_by_ordinals, nla->aggregate_definitions_, true);
            recurseNode(&a);
        }
        recurseNode(nla);
    } else {
        GroupByAggregate<B> *sma = (GroupByAggregate<B> *) agg;

        SortDefinition cur_sort = sma->getSortOrder();
        std::vector<int32_t> group_by_ordinals = sma->group_by_;
        if (!GroupByAggregate<B>::sortCompatible(cur_sort, group_by_ordinals)) {
            // insert sort
            SortDefinition sort_order;
            for (uint32_t idx: group_by_ordinals) {
                sort_order.template emplace_back(ColumnSort(idx, SortDirection::ASCENDING));
            }
            Sort<B> *sort_before_sma = new Sort<B>(child->clone(), sort_order);
            child->setParent(sort_before_sma);
            sort_before_sma->setChild(child);
            sort_before_sma->setParent(sma);
            sma->setChild(sort_before_sma);
        }


        recurseNode(sma);
        NestedLoopAggregate a(child->clone(), sma->group_by_, sma->aggregate_definitions_,
                              sma->getJsonOutputCardinality());

        // In NestedLoop, child's sort is not remained,
        // TODO : need to change this part of setting sort order.
        a.setSortOrder(child->getSortOrder());
        a.setOperatorId(sma->getOperatorId());

        //TODO : check if we need to add sort order to parent. Need Expected Sort

        recurseNode(&a);
    }
}


template class vaultdb::PlanParser<bool>;
template class vaultdb::PlanParser<emp::Bit>;
