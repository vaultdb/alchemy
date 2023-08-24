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
#include <util/logger.h>
#include <regex>

using namespace vaultdb;
using boost::property_tree::ptree;
using namespace Logging;

template<typename B>
PlanParser<B>::PlanParser(const string &db_name, const string & sql_file, const string & json_file,
                          const int &limit) : db_name_(db_name), input_limit_(limit) {
    parseSqlInputs(sql_file);
    parseSecurePlan(json_file);

   if(getAutoFlag())
        calculateAutoAggregate();
}

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
    if(op_name == "LogicalValues") if(json_only_) {
            op = parseLocalScan(operator_id, tree);
    } else {
        return;  // parsed elsewhere
    }

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
    int num_aggregates = sma_vector_.size();

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
                if(sort_vector_[i] != nullptr)
                    cost += OperatorCostModel::operatorCost((SecureOperator *) sort_vector_[i]);

                sma = sma_vector_[i];
                cost += OperatorCostModel::operatorCost((SecureOperator *) sma);

                // Propagate output cardinality
                cur_output_cardinality_ = sma->getOutputCardinality();

            } else {  // type == NestedLoopAggregate
                NestedLoopAggregate<B> *nla = nla_vector_[i];
                cost += OperatorCostModel::operatorCost((SecureOperator *) nla);

                // Propagate output cardinality
                cur_output_cardinality_ = nla->getOutputCardinality();
            }

            // Get the operator id of the next aggregate operator (or the end if this is the last one)
            int next_op_id = (i + 1 < num_aggregates) ? agg_id_[i + 1] : operators_.size();

            // Propagate the output cardinality to the operators between this aggregate and the next one
            for (int op_id = agg_id_[i] + 1; op_id < next_op_id; op_id++) {
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
		Logger* log = get_log();
        log->write("Cost : " + std::to_string(cost) + ", combination : " + std::to_string(combination), Level::INFO);

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
        Operator<B>* child = operators_[agg_id_[i] - 1];

        // Fetch the correct operator and create a new instance
        if (type == SMA) {
            // if sort flag is set, create and link a new sort operator
            if(sort_vector_[i] != nullptr){
                Sort<B>* sort_before_sma = sort_vector_[i];
                SortDefinition sort_order = sort_before_sma->getSortOrder();
                Operator<B>* real_sort_before_sma = new Sort<B>(child, sort_order);
                support_ops_.emplace_back(real_sort_before_sma);
                child->setParent(real_sort_before_sma);
                real_sort_before_sma->setChild(child);
                child = real_sort_before_sma;
            }

            group_by_ordinals = sma_vector_[i]->group_by_;
            aggregators = sma_vector_[i]->aggregate_definitions_;
            bool check_sort = sma_vector_[i]->check_sort_;
            SortDefinition effective_sort = sma_vector_[i]->effective_sort_;

            GroupByAggregate<B> *real_sma = new GroupByAggregate<B>(child, group_by_ordinals, aggregators, check_sort, effective_sort);
            real_sma->setOutputCardinality(sma_vector_[i]->getJsonOutputCardinality());
            child->setParent(real_sma);
            real_sma->setChild(child);
            operators_[agg_id_[i]] = real_sma;

            if(agg_id_[i] + 1 == operators_.size())
                root_ = real_sma;

        } else {  // type == NestedLoopAggregate
            group_by_ordinals = nla_vector_[i]->group_by_;
            aggregators = nla_vector_[i]->aggregate_definitions_;
            int cardBound = nla_vector_[i]->getOutputCardinality();
            SortDefinition effective_sort = nla_vector_[i]->effective_sort_;

            NestedLoopAggregate<B> *real_nla = new NestedLoopAggregate<B>(child, group_by_ordinals, aggregators, cardBound, effective_sort);
            child->setParent(real_nla);
            real_nla->setChild(child);
            operators_[agg_id_[i]] = real_nla;

            if(agg_id_[i] + 1 == operators_.size())
                root_ = real_nla;
        }

        operators_.at(agg_id_[i])->setOperatorId(agg_id_[i]);

        // set the parent of the next operator to this one
        if (agg_id_[i] + 1 < operators_.size()){
            operators_[agg_id_[i] + 1]->setChild(operators_[agg_id_[i]]);
            operators_[agg_id_[i]]->setParent(operators_[agg_id_[i] + 1]);
        }

        // Get the operator id of the next aggregate operator (or the end if this is the last one)
        int next_op_id = (i + 1 < num_aggregates) ? agg_id_[i + 1] : operators_.size();

        // Propagate the output cardinality to the operators between this aggregate and the next one
        for (int op_id = agg_id_[i] + 1; op_id < next_op_id; op_id++) {
            Operator<B> *cur_op = operators_[op_id];
            cur_op->setOutputCardinality(operators_[agg_id_[i]]->getOutputCardinality());
        }
    }

    // Delete SMA vector
    for (auto &sma : sma_vector_) {
        if (sma != nullptr)
            delete sma;
    }

    // Delete NLA vector
    for (auto &nla : nla_vector_) {
        if (nla != nullptr)
            delete nla;
    }

    // Delete Sort vector
    for (auto &sort : sort_vector_) {
        if (sort != nullptr)
            delete sort;
    }
    // Clear and shrink SMA vector
    sma_vector_.clear();
    sma_vector_.shrink_to_fit();

    // Clear and shrink NLA vector
    nla_vector_.clear();
    nla_vector_.shrink_to_fit();

    // Clear and shrink Sort vector
    sort_vector_.clear();
    sort_vector_.shrink_to_fit();
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
    if(aggregate_json.count("cardinality-bound") > 0)
        cardinality_bound = aggregate_json.get_child("cardinality-bound").template get_value<int>();
    else if(aggregate_json.count("cardBound") > 0)
        cardinality_bound = aggregate_json.get_child("cardBound").template get_value<int>();

    bool check_sort = true;
    if(aggregate_json.count("checkSort") > 0) {
        check_sort = aggregate_json.get_child("checkSort").template get_value<bool>();
    }

    SortDefinition effective_sort;
    if(aggregate_json.count("effective-collation") > 0) {
        boost::property_tree::ptree sort_payload = aggregate_json.get_child("effective-collation");
        int limit = -1;

        for (ptree::const_iterator it = sort_payload.begin(); it != sort_payload.end(); ++it) {
            ColumnSort cs;
            cs.first = it->second.get_child("field").get_value<int>(); // field_idx
            std::string direction_str = it->second.get_child("direction").get_value<std::string>();
            cs.second = (direction_str == "ASCENDING") ? SortDirection::ASCENDING : SortDirection::DESCENDING;
            effective_sort.push_back(cs);
        }
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
                    sort_vector_.push_back(sort_before_sma);
                    sma->effective_sort_ = effective_sort;
                }
                else{
                    sma = new GroupByAggregate<B>(child->clone(), group_by_ordinals, aggregators, check_sort);
                    sort_vector_.push_back(nullptr);
                    sma->effective_sort_ = effective_sort;
                }
                nla = new NestedLoopAggregate<B>(child->clone(), group_by_ordinals, aggregators, cardinality_bound);
            }
            else {
                sma = new GroupByAggregate<B>(child->clone(), group_by_ordinals, aggregators, check_sort);
                sort_vector_.push_back(nullptr);
                sma->effective_sort_ = effective_sort;
                nla = new NestedLoopAggregate<B>(child->clone(), group_by_ordinals, aggregators, cardinality_bound);
            }

            sma->setJsonOutputCardinality(cardinality_bound);
            sma->effective_sort_ = effective_sort;
            nla->effective_sort_ = effective_sort;
            sma_vector_.push_back(sma);
            nla_vector_.push_back(nla);
            setAutoFlag(true);
            agg_id_.push_back(operator_id);

            return sma;
        }

        if(cardinality_bound > 0 && (agg_algo == "nested-loop-aggregate" || agg_algo == ""))
            return new NestedLoopAggregate<B>(child, group_by_ordinals, aggregators, cardinality_bound, effective_sort);
        else if(!check_sort && (agg_algo == "sort-merge-aggregate" || agg_algo == ""))
            return new GroupByAggregate<B>(child, group_by_ordinals, aggregators, check_sort, cardinality_bound, effective_sort);
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
            return new GroupByAggregate<B>(child, group_by_ordinals, aggregators, check_sort, cardinality_bound, effective_sort);
        }
    }
    else {
        return new ScalarAggregate<B>(child, aggregators);
    }


}

template<typename B>
Operator<B> *PlanParser<B>::parseJoin(const int &operator_id, const ptree &join_tree) {
    Logger *log = get_log();
    string join_type;


    boost::property_tree::ptree join_condition_tree = join_tree.get_child("condition");

    ptree input_list = join_tree.get_child("inputs.");
    ptree::const_iterator it = input_list.begin();
    int lhs_id = it->second.get_value<int>();
    Operator<B> *lhs  = operators_.at(lhs_id);
    ++it;
    int rhs_id = it->second.get_value<int>();
    Operator<B> *rhs  = operators_.at(rhs_id);

    Expression<B> *join_condition = ExpressionParser<B>::parseExpression(join_condition_tree, lhs->getOutputSchema(), rhs->getOutputSchema());

    if(join_tree.count("operator-algorithm") > 0)
        join_type = join_tree.get_child("operator-algorithm").template get_value<string>();

    // check it is a valid join algo spec (if specified)
    assert(join_type == "sort-merge-join" || join_type == "nested-loop-join" || join_type == "merge-join" || join_type == "auto" || join_type.empty());

    // if fkey designation exists, use this to create keyed join
    // key: foreignKey
    if(join_tree.count("foreignKey") > 0) {
        int foreign_key = join_tree.get_child("foreignKey").template get_value<int>();

        if (join_type == "auto") {

            auto smj = new SortMergeJoin<B>(lhs->clone(), rhs->clone(), foreign_key, join_condition->clone());
            auto nlj = new KeyedJoin<B>(lhs->clone(), rhs->clone(), foreign_key, join_condition->clone());
            size_t smj_cost = OperatorCostModel::operatorCost((SecureOperator *) smj);
            size_t nlj_cost = OperatorCostModel::operatorCost((SecureOperator *) nlj);

            auto join_key_idxs = smj->joinKeyIdxs();
            bool lhs_sort_compatible = smj->sortCompatible(lhs);
            bool rhs_sort_compatible = smj->sortCompatible(rhs);

            delete nlj;
            delete smj;


            // check sort compatibility for SMJ
            // TODO: consider case where we insert sort in front of both lhs and rhs inputs
            if (lhs_sort_compatible && !rhs_sort_compatible) {

                // rhs will have second keys
                vector<ColumnSort> rhs_sort;
                int lhs_col_cnt = lhs->getOutputSchema().getFieldCount();
                SortDefinition lhs_sort = lhs->getSortOrder();

                for (int i = 0; i < join_key_idxs.size(); ++i) {
                    int idx = join_key_idxs[i].second;
                    rhs_sort.emplace_back(ColumnSort(idx - lhs_col_cnt, lhs_sort[i].second));
                }

                Operator<B> *rhs_sorter = new Sort<B>(rhs->clone(), rhs_sort);
                auto smj_presorted = new SortMergeJoin<B>(lhs->clone(), rhs_sorter, foreign_key, join_condition->clone());
                auto smj_opt_cost = OperatorCostModel::operatorCost((SecureOperator *) smj_presorted);
                delete smj_presorted;

                if (smj_opt_cost < smj_cost && smj_opt_cost < nlj_cost) {
                    log->write("Operator (" + std::to_string(operator_id) + "). " +
                        "smj cost : " + std::to_string(smj_cost) +
                        ", pre-sorted smj cost: " + std::to_string(smj_opt_cost) +
                        ", nlj cost : " + std::to_string(nlj_cost) +
                        ", join type: presorted smj", Level::INFO);

                    Sort<B> *sorter = new Sort<B>(rhs, rhs_sort);
                    return new SortMergeJoin<B>(lhs, sorter, foreign_key, join_condition);
                }
            } else if (rhs_sort_compatible && !lhs_sort_compatible) {
               vector<ColumnSort> lhs_sort;
               SortDefinition rhs_sort = rhs->getSortOrder();

               for (int i = 0; i < join_key_idxs.size(); ++i) {
                   int idx = join_key_idxs[i].first;
                   lhs_sort.emplace_back(ColumnSort(idx, rhs_sort[i].second));
               }

               Operator<B> *lhs_sorter = new Sort<B>(lhs->clone(), lhs_sort);
               auto smj_presorted = new SortMergeJoin<B>(lhs->clone(), lhs_sorter, foreign_key, join_condition->clone());
               auto smj_opt_cost = OperatorCostModel::operatorCost((SecureOperator *) smj_presorted);
               delete smj_presorted;

               if (smj_opt_cost < smj_cost && smj_opt_cost < nlj_cost) {
                   log->write("Operator (" + std::to_string(operator_id) + "). " +
                   "smj cost : " + std::to_string(smj_cost) +
                   ", pre-sorted smj cost: " + std::to_string(smj_opt_cost) +
                   ", nlj cost : " + std::to_string(nlj_cost) +
                   ", join type: presorted smj", Level::INFO);

                   Sort<B> *sorter = new Sort<B>(lhs, lhs_sort);
                   return new SortMergeJoin<B>(sorter, rhs, foreign_key, join_condition);
               }

            }

            string selected_join = (smj_cost < nlj_cost) ? "sort-merge-join" : "nested-loop-join";

            log->write("Operator (" + std::to_string(operator_id) + "). " +
                "smj cost : " + std::to_string(smj_cost) +
                ", nlj cost : " + std::to_string(nlj_cost) +
                ", join type : " + selected_join, Level::INFO);

            if (selected_join == "sort-merge-join") {
                return new SortMergeJoin<B>(lhs, rhs, foreign_key, join_condition);
            }
            else {
                return new KeyedJoin<B>(lhs, rhs, foreign_key, join_condition);
            }

        } // end join-algorithm="auto"

         if (join_type == "sort-merge-join") {
             return new SortMergeJoin<B>(lhs, rhs, foreign_key, join_condition);
         }
        else { // if algorithm unspecified but FK, use KeyedJoin
             return new KeyedJoin<B>(lhs, rhs, foreign_key, join_condition);
         }

    } // end pk-fk join

    if (join_type == "merge-join") {
        if(join_tree.count("dummy-handling") > 0 && join_tree.get_child("dummy-handling").template get_value<string>() == "OR")
            return new MergeJoin<B>(lhs, rhs, join_condition, SortDefinition(), true);
        else
            return new MergeJoin<B>(lhs, rhs, join_condition, SortDefinition(), false);
    }


    return new BasicJoin<B>(lhs, rhs, join_condition);

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
Operator<B> *PlanParser<B>::parseLocalScan(const int & operator_id, const boost::property_tree::ptree &local_scan_tree) {
    string sql = "";
    int input_party = 0;
    bool multiple_sort_ = false;
    B plain_has_dummy_tag = false;
    string merge_sql = "", op_algo = "";
    int local_tuple_limit = input_limit_;

    if(local_scan_tree.count("sql") > 0)
        sql = local_scan_tree.get_child("sql").template get_value<string>();
    if(local_scan_tree.count("merge-sql") > 0)
        merge_sql = local_scan_tree.get_child("merge-sql").template get_value<string>();
    if(local_scan_tree.count("operator-algorithm") > 0)
        op_algo = local_scan_tree.get_child("operator-algorithm").template get_value<string>();

    if(local_scan_tree.count("party") > 0)
        input_party = local_scan_tree.get_child("party").template get_value<int>();

    if(local_scan_tree.count("input-limit") > 0)
        local_tuple_limit = local_scan_tree.get_child("input-limit").template get_value<int>();

    plain_has_dummy_tag =  (sql.find("dummy-tag") != std::string::npos) || (sql.find("dummy_tag") != std::string::npos);
    bool dummy_tag = (sql.find("dummy-tag") != std::string::npos) || (sql.find("dummy_tag") != std::string::npos);

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
    if(!foundMultipleSorts && local_scan_tree.count("collation") > 0){
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


    SortDefinition sort_def = (interesting_sort_orders_.find(operator_id) != interesting_sort_orders_.end()) ?  interesting_sort_orders_[operator_id].front() : SortDefinition();
    if(op_algo == "merge-input" && std::is_same_v<B, Bit> && local_tuple_limit > 0) {
        return createMergeInput(merge_sql, dummy_tag, local_tuple_limit, sort_def, plain_has_dummy_tag);
    }
    if(input_party > 0) return createInputOperator(sql, sort_def, input_party, plain_has_dummy_tag, dummy_tag);
    // else
    return createInputOperator(sql, sort_def, plain_has_dummy_tag, dummy_tag);
}


template<typename B>
Operator<B> *PlanParser<B>::parseShrinkwrap(const int & operator_id, const boost::property_tree::ptree &pt) {

    ptree input_list = pt.get_child("inputs.");
    ptree::const_iterator it = input_list.begin();
    int child_op_id = it->second.get_value<int>();
    Operator<B> *op = operators_.at(child_op_id);

    size_t output_cardinality;
    if(pt.count("output_cardinality") > 0)
           output_cardinality =  pt.get_child("output_cardinality").template get_value<int>();

    if(pt.count("output-cardinality") > 0)
        output_cardinality =  pt.get_child("output-cardinality").template get_value<int>();

    return new Shrinkwrap<B>(op, output_cardinality);
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
void PlanParser<B>::optimizeTree() {
    auto new_root = root_->clone();
    auto leaf = new_root;
    while(leaf->getChild() != nullptr) leaf = leaf->getChild();

    assert(leaf->getType() == OperatorType::SECURE_SQL_INPUT);

    //clone operators_ map into optimizeTree_operators_
    for (auto &entry : operators_) {
        optimizeTree_operators_[entry.first] = entry.second->clone();
    }

    optimizeTreeHelper(leaf);

    std::cout << "Total Plans : " << std::to_string(total_plan_cnt_) << endl;
    changeOperatorSortOrder();
    deleteOptmizeTreeOperators();
}

template<typename B>
void PlanParser<B>::deleteOptmizeTreeOperators() {
    for (auto &entry : optimizeTree_operators_) {
        Operator<B> *op = entry.second;

        // Set the children to nullptr
        op->setNullChild(nullptr, 0);
        op->setNullChild(nullptr, 1);
        op->setParent(nullptr);
        // Delete the operator
        delete op;
        entry.second = nullptr;  // Optional: set the map value to nullptr too
    }

    // Optionally, you can also clear the map after deleting the operators
    optimizeTree_operators_.clear();
}


template<typename B>
void PlanParser<B>::changeOperatorSortOrder() {
    // Extract the sort orders
    std::map<int, SortEntry> sort_orders = extractSortOrders(min_cost_plan_string_);

    SortEntry sort_before_sma;  // This will store the SortDefinition of the sort right before SMA

    // Modify the operators sort orders
    for (const auto &order : sort_orders) {
        int op_id = order.first;
        const SortDefinition &sort_def = std::get<0>(order.second);
        int next_op_id = std::get<1>(order.second);

        if (operators_.count(op_id) > 0) {

            Operator<B> *op = operators_[op_id];
            OperatorType op_type = op->getType();

            if (op_type == OperatorType::SQL_INPUT || op_type == OperatorType::SECURE_SQL_INPUT ||
                    op_type == OperatorType::ZK_SQL_INPUT || op_type == OperatorType::TABLE_INPUT) {
                switch(op_type) {
                    case OperatorType::SQL_INPUT:
                        break;
                    case OperatorType::SECURE_SQL_INPUT:
                        // Attempt to cast the pointer to SecureSqlInput type
                        SecureSqlInput* secureSqlInputOp = dynamic_cast<SecureSqlInput*>(op);

                        if (secureSqlInputOp) { // If the cast was successful
                            string sql_statement_ = secureSqlInputOp->getInputQuery();
                            sql_statement_ = modifySqlStatement(sql_statement_, sort_def);
                            secureSqlInputOp->setInputQuery(sql_statement_);
                        }
                        break;
                }
            } else if(op_type == OperatorType::SORT_MERGE_AGGREGATE || op_type == OperatorType::NESTED_LOOP_AGGREGATE) {
                //If optimized plan has different aggregate with original plan
                if(op->getTypeString() != std::get<2>(order.second)) {
                    Operator<B> *child = op->getChild();
                    Operator<B> *parent = op->getParent();

                    if(std::get<2>(order.second) == "GroupByAggregate"){
                        GroupByAggregate<B> *sma = (GroupByAggregate<B> *) optimizeTree_operators_[op_id];
                        sma->setChild(child);
                        child->setParent(sma);
                        sma->setParent(parent);
                        parent->setChild(sma);
                        operators_[op_id] = sma;
                    }
                    else{
                        NestedLoopAggregate<B> *nla = (NestedLoopAggregate<B> *) optimizeTree_operators_[op_id];
                        nla->setChild(child);
                        child->setParent(nla);
                        nla->setParent(parent);
                        parent->setChild(nla);
                        operators_[op_id] = nla;
                    }

                    // Delete the original operator
                    op->setNullChild(nullptr);
                    op->setParent(nullptr);
                    delete(op);
                }

                op = operators_[op_id];
                // If optimized plan has sort before sma.
                if(next_op_id == -2 && std::get<1>(sort_before_sma) == op->getOperatorId()){
                    // If the SMA operator has a child operator of type SORT
                    if(op->getChild()->getType() == OperatorType::SORT) {
                        op->getChild()->setSortOrder(std::get<0>(sort_before_sma));
                    }
                    // Need to define new sort operator
                    else{
                        Operator<B> *child = op->getChild();
                        Sort<B> *new_sort = new Sort<B>(child, std::get<0>(sort_before_sma));
                        new_sort->setOperatorId(-1);
                        child->setParent(new_sort);
                        new_sort->setChild(child);
                        new_sort->setParent(op);
                        op->setChild(new_sort);
                    }
                }
                //If optimized plan has no sort before sma
                else{
                    //If original plan has sort before sma
                    if(op->getChild()->getType() == OperatorType::SORT) {
                        Operator<B>* delete_sort = op->getChild();
                        Operator<B>* child = delete_sort->getChild();
                        delete_sort->setNullChild(nullptr);
                        delete_sort->setParent(nullptr);
                        delete delete_sort;
                        op->setChild(child);
                        child->setParent(op);
                    }
                }
            }  else if(op_type == OperatorType::KEYED_NESTED_LOOP_JOIN || op_type == OperatorType::SORT_MERGE_JOIN) {
                //If optimized plan has different aggregate with original plan
                if(op->getTypeString() != std::get<2>(order.second)) {
                    Operator<B> *lhs = op->getChild(0);
                    Operator<B> *rhs = op->getChild(1);
                    Operator<B> *parent = op->getParent();

                    if(std::get<2>(order.second) == "KeyedJoin"){
                        KeyedJoin<B> *kj = (KeyedJoin<B> *) optimizeTree_operators_[op_id];
                        kj->setParent(parent);
                        parent->setChild(kj);
                        kj->setChild(lhs,0);
                        kj->setChild(rhs,1);
                        lhs->setParent(kj);
                        rhs->setParent(kj);
                        operators_[op_id] = kj;
                    }
                    else{
                        SortMergeJoin<B> *smj = (SortMergeJoin<B> *) optimizeTree_operators_[op_id];
                        smj->setParent(parent);
                        parent->setChild(smj);
                        smj->setChild(lhs,0);
                        smj->setChild(rhs,1);
                        lhs->setParent(smj);
                        rhs->setParent(smj);
                        operators_[op_id] = smj;
                    }
                    // Delete the original operator
                    op->setNullChild(nullptr);
                    op->setNullChild(nullptr, 1);
                    op->setParent(nullptr);
                    delete(op);
                }
            }
            if(sort_def[0].first == -2)
                operators_[op_id]->setSortOrder(SortDefinition());
            else
                operators_[op_id]->setSortOrder(sort_def);
        } else if(op_id == -1) {
            sort_before_sma = order.second;
        }
    }
}

template<typename B>
std::vector<std::string> PlanParser<B>::extractColumnsFromSql(const std::string& sql) {
    std::regex columnRegex(R"(SELECT\s+(.*?)\s*FROM)");
    std::smatch matches;
    if (std::regex_search(sql, matches, columnRegex)) {
        std::string columnsStr = matches[1];
        std::stringstream ss(columnsStr);
        std::string segment;
        std::vector<std::string> columns;

        // Extracting columns and their aliases
        while (std::getline(ss, segment, ',')) {
            std::size_t aliasPos = segment.rfind("AS");
            if (aliasPos != std::string::npos) {
                // Take alias name if exists
                columns.push_back(segment.substr(aliasPos + 3));
            } else {
                // Otherwise, extract raw column name
                std::stringstream segmentStream(segment);
                std::string potentialColumn;
                segmentStream >> potentialColumn;
                columns.push_back(potentialColumn);
            }
        }
        return columns;
    }
    return {};
}

template<typename B>
std::string PlanParser<B>::modifySqlStatement(const std::string& originalSql, const SortDefinition& sort_defs) {
    auto columns = extractColumnsFromSql(originalSql);

    std::string orderByClause = "";
    for (const auto& def : sort_defs) {
        if (def.first < 0 || def.first >= columns.size()) continue; // skip invalid column index

        std::string direction = (def.second == SortDirection::ASCENDING) ? "ASC" : "DESC";
        if (!orderByClause.empty()) orderByClause += ", ";
        orderByClause += columns[def.first] + " " + direction;
    }

    std::regex orderByRegex("ORDER BY [^)]+$");
    if (orderByClause.empty()) {
        return std::regex_replace(originalSql, orderByRegex, "");
    } else if (std::regex_search(originalSql, orderByRegex)) {
        return std::regex_replace(originalSql, orderByRegex, "ORDER BY " + orderByClause);
    } else {
        return originalSql + " ORDER BY " + orderByClause;
    }
}


template<typename B>
std::map<int, typename PlanParser<B>::SortEntry> PlanParser<B>::extractSortOrders(const std::string &plan_string) {
    std::map<int, SortEntry> sort_orders;
    std::regex line_regex("\\n");
    std::sregex_token_iterator line_iterator(plan_string.begin(), plan_string.end(), line_regex, -1);
    std::sregex_token_iterator end;

    int prev_op_id = -1;
    std::string prev_op_type;
    std::string op_type;

    while (line_iterator != end) {
        std::string line = *line_iterator++;
        std::regex op_id_regex("#(-?\\d+):");
        std::smatch op_id_match;

        if (std::regex_search(line, op_id_match, op_id_regex)) {
            int op_id = std::stoi(op_id_match[1].str());

            std::regex op_type_regex(": (\\w+) order by:");
            std::smatch op_type_match;
            if (std::regex_search(line, op_type_match, op_type_regex)) {
                op_type = op_type_match[1].str();

                std::regex sort_regex("\\{([^}]+)\\}");
                std::smatch sort_match;
                if (std::regex_search(line, sort_match, sort_regex)) {
                    std::string sort_str = sort_match[1].str();

                    SortDefinition sort_order;
                    if (!sort_str.empty()) {
                        std::regex order_regex("<(\\-?\\d+),\\s(ASC|DESC|INVALID)>");
                        std::smatch order_match;
                        std::string::const_iterator order_start(sort_str.cbegin());

                        while (std::regex_search(order_start, sort_str.cend(), order_match, order_regex)) {
                            int32_t ordinal = std::stoi(order_match[1].str());
                            SortDirection dir = (order_match[2].str() == "ASC") ? SortDirection::ASCENDING : SortDirection::DESCENDING;
                            sort_order.push_back({ordinal, dir});
                            order_start = order_match.suffix().first;
                        }
                    } else {
                        // If order by set is empty, set default sort definition
                        sort_order.push_back({-2, SortDirection::ASCENDING});
                    }

                    int next_op_id = -1;
                    if (prev_op_type == "GroupByAggregate" && op_type == "Sort" && op_id == -1) {
                        next_op_id = prev_op_id;

                        // set sma's int value as -2, so that it can be flag for sort exsists before sma.
                        sort_orders[prev_op_id] = std::make_tuple(sort_order, -2, "GroupByAggregate");
                    }

                    sort_orders[op_id] = std::make_tuple(sort_order, next_op_id, op_type);
                } else if (line.find("order by: {}") != std::string::npos) { // Check if the line contains an empty order by
                    // If order by set is empty, set default sort definition
                    SortDefinition sort_order;
                    sort_order.push_back({-2, SortDirection::ASCENDING});
                    sort_orders[op_id] = std::make_tuple(sort_order, -1, op_type); // Using -1 as the default next_op_id for these cases
                }
            }

            prev_op_id = op_id;
            prev_op_type = op_type;
        }
    }

    return sort_orders;
}


// helper function for recursing up operator tree
template<typename B>
void PlanParser<B>::optimizeTreeHelper(Operator<B> *op) {

    Operator<B> *node = op->clone();

    switch(op->getType()) {
        case OperatorType::SQL_INPUT:
        case OperatorType::ZK_SQL_INPUT:
        case OperatorType::TABLE_INPUT: {
            // first try with given sort order (if any) and empty set unconditionally
            auto sorts = getCollations(op);
            for (auto &sort: sorts) {
                node->setSortOrder(sort);
                optimizeTree_operators_[node->getOperatorId()] = node;
                //std::cout << node->toString() << endl;
                recurseNode(node);
            }
            break;
        }
        case OperatorType::SECURE_SQL_INPUT:{
            // first try with given sort order (if any) and empty set unconditionally
            SecureSqlInput* secureSqlInputOp = dynamic_cast<SecureSqlInput*>(op);

            // If it is party scan, just run the node with default sort order
            if(secureSqlInputOp->getInputParty() > 0)
                recurseNode(node);
            else {
                auto sorts = getCollations(op);
                for (auto &sort: sorts) {
                    node->setSortOrder(sort);
                    optimizeTree_operators_[node->getOperatorId()] = node;
                    //std::cout << node->toString() << endl;
                    recurseNode(node);
                }
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
            recurseAgg(node);
            break;
        }
        case OperatorType::SORT:
            // Sort is always same with sort order
            recurseSort(node);
            break;
        default:
            // node is cloned from operators_ map in the optimizeHelper()
            // it's child is pointing operators_ map's child(original child), so need to fix those to point child with new order
            node->setChild(optimizeTree_operators_.at(node->getChild()->getOperatorId()));
            // Same with child's sort order
            node->updateCollation();
            optimizeTree_operators_[node->getOperatorId()] = node;
            //std::cout << node->toString() << endl;
            recurseNode(node);
            break;
    }

    //delete node;
}


template<typename B>
void PlanParser<B>::recurseNode(Operator<B> *op) {
    int op_id = op->getOperatorId();
    Operator<B> *parent = operators_[op_id]->getParent();

    // at the root node
    if(parent == nullptr) {
        size_t plan_cost = op->planCost();
        std::cout << op->printTree() << endl;
        std::cout << "Cost : " << std::to_string(plan_cost) << "\n" << "----------------------------" << std::endl;
        total_plan_cnt_++;

        if(plan_cost < min_plan_cost_) {
            min_plan_cost_ = plan_cost;
            min_cost_plan_string_ = op->printMinCostPlan();
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
        // If it is party scan, skip the first empty order
        rhs_leaf->setSortOrder(sort);
        optimizeTree_operators_[rhs_leaf->getOperatorId()] = rhs_leaf;

        if(join->getType() == OperatorType::KEYED_NESTED_LOOP_JOIN) {
            // For KeyedJoin, sort order is same with foreign key's sort order
            KeyedJoin<B> *kj = (KeyedJoin<B> *)join;

            // join is cloned from operators_ map in the optimizeHelper()
            // it's childs are pointing operators_ map's child(original child), so need to fix those to point child with new order
            Operator<B> *lhs = optimizeTree_operators_.at(kj->getChild(0)->getOperatorId());

            kj->setChild(lhs);
            kj->setChild(rhs_leaf, 1);
            kj->updateCollation();

            optimizeTree_operators_[kj->getOperatorId()] = kj;
            //std::cout << kj->toString() << endl;
            recurseNode(kj);

            // For SMJ, sort order is same with sort key's order
            // Make plan in case of smj.
            SortMergeJoin<B> *smj = new SortMergeJoin(lhs->clone(), rhs_leaf->clone(), kj->foreignKeyChild(), kj->getPredicate());
            smj->setOperatorId(kj->getOperatorId());
            smj->updateCollation();
            optimizeTree_operators_[smj->getOperatorId()] = smj;
            //std::cout << smj->toString() << endl;
            recurseNode(smj);
        }
        else if(join->getType() == OperatorType::SORT_MERGE_JOIN){
            SortMergeJoin<B> *smj = (SortMergeJoin<B> *) join;

            Operator<B> *lhs = optimizeTree_operators_.at(smj->getChild(0)->getOperatorId());

            // join is cloned from operators_ map in the optimizeHelper()
            // it's childs are pointing operators_ map's child(original child), so need to fix those to point child with new order
            smj->setChild(lhs);
            smj->setChild(rhs_leaf, 1);
            smj->updateCollation();
            //std::cout << smj->toString() << endl;

            optimizeTree_operators_[smj->getOperatorId()] = smj;
            recurseNode(smj);

            // Make plan in case of KeyedJoin.
            KeyedJoin<B> *kj = new KeyedJoin(lhs->clone(), rhs_leaf->clone(), smj->foreignKeyChild(), smj->getPredicate());
            kj->setOperatorId(smj->getOperatorId());
            kj->updateCollation();
            optimizeTree_operators_[kj->getOperatorId()] = kj;
            operatorPool.push_back(kj);

            //std::cout << kj->toString() << endl;
            recurseNode(kj);
        }
        else if(join->getType() == OperatorType::MERGE_JOIN){
            MergeJoin<B> *mj = (MergeJoin<B> *) join;

            Operator<B> *lhs = optimizeTree_operators_.at(mj->getChild(0)->getOperatorId());
            Operator<B> *rhs = optimizeTree_operators_.at(mj->getChild(1)->getOperatorId());

            // Childs are pointing original parent, Need to fix those to point new parent.
            mj->setChild(lhs);
            mj->setChild(rhs, 1);
            lhs->setParent(mj);
            rhs->setParent(mj);
            mj->updateCollation();
            //std::cout << smj->toString() << endl;

            optimizeTree_operators_[mj->getOperatorId()] = mj;
            operatorPool.push_back(mj);
            recurseNode(mj);
        }
    }
}

template<typename B>
void PlanParser<B>::recurseAgg(Operator<B> *agg) {

    Operator<B> *child = optimizeTree_operators_.at(agg->getChild()->getOperatorId());

    // Need this cloned_child for second aggregate option, in case if first aggregate plan is deleted
    Operator<B> *cloned_child = child->clone();

    // Nested Loop Aggregate Case
    if (agg->getType() == OperatorType::NESTED_LOOP_AGGREGATE) {
        NestedLoopAggregate<B> *nla = (NestedLoopAggregate<B> *) agg;

        // agg is cloned from operators_ map in the optimizeHelper()
        // it's child is pointing operators_ map's child(original child), so need to fix those to point child with new order
        nla->setChild(child);
        nla->updateCollation();
        optimizeTree_operators_[nla->getOperatorId()] = nla;
        recurseNode(nla);

        std::vector<int32_t> group_by_ordinals = nla->group_by_;
        SortDefinition child_sort = child->getSortOrder();
        SortDefinition effective_sort = nla->effective_sort_;

        // If Child's sort is not compatible with effective_sort, then insert sort before sma.
        if (child_sort != effective_sort) {
            Sort<B> *sort_before_sma = new Sort<B>(cloned_child, effective_sort);
            GroupByAggregate<B> *sma = new GroupByAggregate(sort_before_sma, group_by_ordinals, nla->aggregate_definitions_, true);
            sma->setOperatorId(nla->getOperatorId());
            sma->updateCollation();
            optimizeTree_operators_[sma->getOperatorId()] = sma;
            recurseNode(sma);
        }
        // If Child's sort is compatible with effective_sort, then just insert sma.
        else {
            GroupByAggregate<B> *sma = new GroupByAggregate(cloned_child, group_by_ordinals, nla->aggregate_definitions_, true);
            sma->setOperatorId(nla->getOperatorId());
            sma->updateCollation();
            optimizeTree_operators_[sma->getOperatorId()] = sma;
            recurseNode(sma);
        }
    }
    // SortMergeAggregate Case
    else {
        GroupByAggregate<B> *sma = (GroupByAggregate<B> *) agg;

        SortDefinition child_sort = child->getSortOrder();
        std::vector<int32_t> group_by_ordinals = sma->group_by_;
        SortDefinition effective_sort = sma->effective_sort_;

        // If Child's sort is not compatible with effective_sort, then insert sort before sma.
        if(child_sort != effective_sort){
            Sort<B> *sort_before_sma = new Sort<B>(child, effective_sort);
            sma->setChild(sort_before_sma);
            sma->check_sort_ = true;
        }
        // If Child's sort is compatible with effective_sort, then just insert sma.
        else
            sma->setChild(child);
        sma->updateCollation();
        optimizeTree_operators_[sma->getOperatorId()] = sma;
        recurseNode(sma);

        // Make plan in case of nla.
        NestedLoopAggregate<B> *nla = new NestedLoopAggregate(cloned_child, sma->group_by_, sma->aggregate_definitions_, sma->getJsonOutputCardinality());
        nla->setOperatorId(sma->getOperatorId());
        nla->setChild(cloned_child);
        nla->updateCollation();
        optimizeTree_operators_[nla->getOperatorId()] = nla;
        recurseNode(nla);
    }
}

template<typename B>
void PlanParser<B>::recurseSort(Operator<B> *sort) {
    Operator<B> *child = optimizeTree_operators_.at(sort->getChild()->getOperatorId());

    // If sort is same with child's order, then this sort is not needed
    if(sort->getSortOrder() == child->getSortOrder()) {
        int op_id = sort->getOperatorId();

        // If sort is not root, then need to fix parent's child pointer
        if(op_id < operators_.size()) {
            Operator<B> *parent = sort->getParent();
            parent->setChild(child);
            recurseNode(parent);
        }
        // If sort is root
        else{
            child->setParent(nullptr);
            recurseNode(child);
        }
    }
    // If sort is needed
    else {
        sort->setChild(child);
        sort->updateCollation();
        optimizeTree_operators_[sort->getOperatorId()] = sort;
        recurseNode(sort);
    }
}


template class vaultdb::PlanParser<bool>;
template class vaultdb::PlanParser<emp::Bit>;
