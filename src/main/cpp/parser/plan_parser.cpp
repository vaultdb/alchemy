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
#include <operators/keyed_sort_merge_join.h>
#include <operators/merge_join.h>
#include <operators/basic_join.h>
#include <operators/filter.h>
#include <operators/project.h>
#include <parser/expression_parser.h>
#include <operators/shrinkwrap.h>
#include <operators/table_scan.h>
#include <operators/union.h>
#include <operators/left_keyed_join.h>
#include <operators/packed_table_scan.h>

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

}

template<typename B>
PlanParser<B>::PlanParser(const string &db_name, const string & json, const int &limit, const bool read_from_file) : db_name_(db_name), input_limit_(limit), zk_plan_(false), json_only_(true) {

    if(read_from_file)
        parseSecurePlan(json);
    else
        parseSecurePlanString(json);

}

template<typename B>
PlanParser<B>::PlanParser(const std::string &db_name, const std::string &json_file, const int &party, const int &limit, const bool read_from_file) : db_name_(db_name), party_(party), input_limit_(limit), zk_plan_(false), json_only_(true) {
    if(read_from_file)
        parseSecurePlan(json_file);
    else
        parseSecurePlanString(json_file);
}

template<typename B>
Operator<B> *PlanParser<B>::parse(const string &db_name, const string &json_file, const int &limit) {
    PlanParser p(db_name, json_file, limit, true);
    return p.getRoot();
}

template<typename B>
Operator<B> *PlanParser<B>::parse(const std::string & db_name, const string & sql_file, const string & json_file, const int & limit) {
    PlanParser p(db_name, sql_file, json_file, limit);
    return p.root_;
}

template<typename B>
Operator<B> *PlanParser<B>::parseJSONString(const string &db_name, const string &json_string, const int &limit) {

    PlanParser p(db_name, json_string, limit, false);
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

                bool plain_has_dummy =  (query.find("dummy_tag") != std::string::npos);
                has_dummy = B(plain_has_dummy);
                query_id = get<0>(input_parameters);
                auto collation = get<1>(input_parameters);
                int input_party = get<2>(input_parameters);
                operators_[query_id] = createInputOperator(query, collation, has_dummy, plain_has_dummy, input_limit_, input_party);

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
    bool plain_dummy_tag =  (query.find("dummy_tag") != std::string::npos);

    query_id = get<0>(input_parameters);
    auto collation = get<1>(input_parameters);
    int input_party = get<2>(input_parameters);

    operators_[query_id] = createInputOperator(query, collation, has_dummy, plain_dummy_tag, input_limit_, input_party);
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
void PlanParser<B>::parseSecurePlanString(const string & json_plan) {
    stringstream ss;
    ss << json_plan;

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
    if(op_name == "VaultDBTableScan")  op = parseTableScan(operator_id, tree);
    if(op_name == "PackedTableScan") op = parsePackedTableScan(operator_id, tree);
    if(op_name == "LogicalUnion") op = parseUnion(operator_id, tree);
    if(op_name == "ShrinkWrap")  op = parseShrinkwrap(operator_id, tree);
    if(op_name == "LogicalValues") {
        if (json_only_) {
            op = parseLocalScan(operator_id, tree);
        } else {
            return;  // parsed elsewhere
        }
    }

    if(op != nullptr) {
        if(operators_.find(operator_id) != operators_.end())
            throw std::invalid_argument("Operator ID already exists: " + std::to_string(operator_id));
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
Operator<bool> *PlanParser<B>::createInputOperator(const string &sql, const SortDefinition &collation, const bool &has_dummy_tag, const bool & plain_has_dummy_tag, const int & input_limit, const int & input_party) {
    size_t limit = (input_limit < 0) ? 0 : input_limit;
    // no input party in plaintext
    return new SqlInput(db_name_, sql, plain_has_dummy_tag, collation, limit);
}

template<typename B>
Operator<emp::Bit> *PlanParser<B>::createInputOperator(const string &sql, const SortDefinition &collation, const emp::Bit &has_dummy_tag, const bool & plain_has_dummy_tag, const int & input_limit, const int & input_party) {
    size_t limit = (input_limit < 0) ? 0 : input_limit;

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
            SortMergeAggregate<B> *sma;
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
                    if (sma->sortCompatible(cur_sort_order)) {
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
            SortDefinition effective_sort = sma_vector_[i]->effective_sort_;

            SortMergeAggregate<B> *real_sma = new SortMergeAggregate<B>(child, group_by_ordinals, aggregators, effective_sort, sma_vector_[i]->getCardinalityBound());
            operators_[agg_id_[i]] = real_sma;

            if(agg_id_[i] + 1 == (int) operators_.size())
                root_ = real_sma;

        } else {  // type == NestedLoopAggregate
            group_by_ordinals = nla_vector_[i]->group_by_;
            aggregators = nla_vector_[i]->aggregate_definitions_;
            int cardBound = nla_vector_[i]->getOutputCardinality();
            SortDefinition effective_sort = nla_vector_[i]->effective_sort_;

            NestedLoopAggregate<B> *real_nla = new NestedLoopAggregate<B>(child, group_by_ordinals, aggregators, effective_sort, cardBound);
            operators_[agg_id_[i]] = real_nla;

            if(agg_id_[i] + 1 == (int) operators_.size())
                root_ = real_nla;
        }

        operators_.at(agg_id_[i])->setOperatorId(agg_id_[i]);

        // set the parent of the next operator to this one
        if (agg_id_[i] + 1 < (int) operators_.size()){
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
    bool check_sort = true;

    if(aggregate_json.count("checkSort") > 0) {
        check_sort= aggregate_json.get_child("checkSort").template get_value<bool>();
    }
    else if(aggregate_json.count("check-sort") > 0) {
        check_sort= aggregate_json.get_child("check-sort").template get_value<bool>();
    }


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


    SortDefinition effective_sort;
    if(aggregate_json.count("effective-collation") > 0) {
        boost::property_tree::ptree sort_payload = aggregate_json.get_child("effective-collation");
//        int limit = -1;

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
        s.is_distinct = !(it->second.get_child("distinct").template get_value<std::string>() == "false");

        assert(!s.is_distinct); // distinct not yet implemented

        aggregators.push_back(s);
    }

    Operator<B> *child = getChildOperator(operator_id, aggregate_json);

    if(!group_by_ordinals.empty()) {

        // Use Cost Model to calculate NLA and SMA, pick more better one.
        if (agg_algo == "auto") {
            SortMergeAggregate<B> *sma;
            NestedLoopAggregate<B> *nla;

            // for sma, check if sort is needed
            SortDefinition child_sort = child->getSortOrder();
            SortDefinition group_by_sort;

            bool sort_needed = false;


            bool condition_met = false;
            if (!child_sort.empty()) {
                if (child_sort[0].first == -1) {
                    // Check if child_sort.size() == effective_sort.size() + 1 and compare the second entry onwards of child_sort with effective_sort
                    if (child_sort.size() == effective_sort.size() + 1 &&
                        std::equal(child_sort.begin() + 1, child_sort.end(), effective_sort.begin())) {
                        condition_met = true;
                    }
                }
                else if (child_sort.size() == effective_sort.size() &&
                         std::equal(child_sort.begin(), child_sort.end(), effective_sort.begin())) {
                    condition_met = true;
                }
            }

            // if child sort is equal with effective_sort, we don't need to sort before SMA
            if(condition_met){
                sma = new SortMergeAggregate<B>(child->clone(), group_by_ordinals, aggregators, effective_sort,
                                                cardinality_bound);
                sort_vector_.push_back(nullptr);
                sma->effective_sort_ = effective_sort;
            }
                // Else, needs to check if this is the same with group by orders
            else {
                // if sort is needed, and current order is not the same with group by orders
                if (!SortMergeAggregate<B>::sortCompatible(child_sort, group_by_ordinals) && check_sort) {
                    sort_needed = true;
                    // insert sort
                    for (uint32_t idx: group_by_ordinals) {
                        group_by_sort.template emplace_back(ColumnSort(idx, SortDirection::ASCENDING));
                    }
                    Sort<B> *sort_before_sma = new Sort<B>(child->clone(), group_by_sort);
                    sma = new SortMergeAggregate<B>(sort_before_sma->clone(), group_by_ordinals, aggregators,
                                                    effective_sort, cardinality_bound);
                    sort_vector_.push_back(sort_before_sma);
                    sma->effective_sort_ = effective_sort;
                }
                    // if sort is not needed
                else {
                    sma = new SortMergeAggregate<B>(child->clone(), group_by_ordinals, aggregators, effective_sort,
                                                    cardinality_bound);
                    sort_vector_.push_back(nullptr);
                    sma->effective_sort_ = effective_sort;
                }
            }

            // create cloned nla
            nla = new NestedLoopAggregate<B>(child->clone(), group_by_ordinals, aggregators, effective_sort,
                                             cardinality_bound);

            sma_vector_.push_back(sma);
            nla_vector_.push_back(nla);

            agg_id_.push_back(operator_id);

            size_t sma_cost = OperatorCostModel::operatorCost((SecureOperator *) sma);
            size_t nla_cost = OperatorCostModel::operatorCost((SecureOperator *) nla);

            string selected_agg = (sma_cost < nla_cost) ? "sort-merge-aggregate" : "nested-loop-aggregate";

            Logger* log = get_log();
            log->write("Operator (" + std::to_string(operator_id) + "). " +
                       "sma cost : " + std::to_string(sma_cost) +
                       ", nla cost : " + std::to_string(nla_cost) +
                       ", agg type : " + selected_agg, Level::DEBUG);

            delete nla;
            delete sma;

            if (selected_agg == "sort-merge-aggregate") {
                if(sort_needed){
                    Sort<B> *sort_before_sma = new Sort<B>(child, group_by_sort);
                    return new SortMergeAggregate<B>(sort_before_sma, group_by_ordinals, aggregators,
                                                     effective_sort, cardinality_bound);
                }
                else
                    return new SortMergeAggregate<B>(child, group_by_ordinals, aggregators,
                                                     effective_sort, cardinality_bound);
            }
            else
                return new NestedLoopAggregate<B>(child, group_by_ordinals, aggregators, effective_sort,
                                                  cardinality_bound);
        } // end auto aggregate setup
        SortDefinition child_sort = child->getSortOrder();

        if (cardinality_bound > 0 && (agg_algo == "nested-loop-aggregate" || agg_algo == ""))
            return new NestedLoopAggregate<B>(child, group_by_ordinals, aggregators, effective_sort, cardinality_bound);

        // default to SMA
        if (check_sort && !GroupByAggregate<B>::sortCompatible(child_sort, group_by_ordinals, effective_sort)) {
            // insert sort
            SortDefinition sma_sort;
            for (uint32_t idx: group_by_ordinals) {
                sma_sort.template emplace_back(ColumnSort(idx, SortDirection::ASCENDING));
            }

            child = new Sort<B>(child, sma_sort);
            support_ops_.template emplace_back(child);
        }
        return new SortMergeAggregate<B>(child, group_by_ordinals, aggregators, effective_sort,  cardinality_bound);
    } // end group-by case
    else {
        return new ScalarAggregate<B>(child, aggregators);
    }


}

template<typename B>
Operator<B> *PlanParser<B>::parseJoin(const int &operator_id, const ptree &join_tree) {
    Logger *log = get_log();
    string join_algo;
    string join_type = "inner";


    boost::property_tree::ptree join_condition_tree = join_tree.get_child("condition");

    ptree input_list = join_tree.get_child("inputs.");
    ptree::const_iterator it = input_list.begin();
    int lhs_id = it->second.get_value<int>();
    Operator<B> *lhs  = operators_.at(lhs_id);
    ++it;
    int rhs_id = it->second.get_value<int>();
    Operator<B> *rhs  = operators_.at(rhs_id);
    auto storage_model = SystemConfiguration::getInstance().storageModel();

    Expression<B> *join_condition = ExpressionParser<B>::parseExpression(join_condition_tree, lhs->getOutputSchema(), rhs->getOutputSchema());

    if(join_tree.count("operator-algorithm") > 0)
        join_algo = join_tree.get_child("operator-algorithm").template get_value<string>();

    // check it is a valid join algo spec (if specified)
    assert(join_algo == "keyed-sort-merge-join" || join_algo == "nested-loop-join" || join_algo == "merge-join" || join_algo == "auto" || join_algo == "cost-keyed-sort-merge-join" || join_algo.empty());

    if(join_tree.count("joinType") > 0)
        join_type = join_tree.get_child("joinType").template get_value<string>();

    // only one algo supported for simulated left-join
    if(join_type == "left") {
        return new LeftKeyedJoin<B>(lhs, rhs, join_condition);
    }

    // if fkey designation exists, use this to create keyed join
    if(join_tree.count("foreignKey") > 0 || join_tree.count("foreign-key") > 0) {
        int foreign_key = (join_tree.count("foreignKey") > 0) ? join_tree.get_child("foreignKey").template get_value<int>()
                                                              : join_tree.get_child("foreign-key").template get_value<int>();

        if (join_algo == "auto") {

            auto smj = new KeyedSortMergeJoin<B>(lhs->clone(), rhs->clone(), foreign_key, join_condition->clone());
            auto nlj = new KeyedJoin<B>(lhs->clone(), rhs->clone(), foreign_key, join_condition->clone());
            size_t smj_cost = OperatorCostModel::operatorCost((SecureOperator *) smj);
            size_t nlj_cost = OperatorCostModel::operatorCost((SecureOperator *) nlj);

            auto join_key_idxs = smj->joinKeyIdxs();

            bool lhs_sort_compatible = smj->sortCompatible(lhs);
            bool rhs_sort_compatible = smj->sortCompatible(rhs);

            delete nlj;
            delete smj;

            Operator<B> *lhs_sorter, *rhs_sorter;
            SortDefinition lhs_sort, rhs_sort;

            if(!rhs_sort_compatible){
                int lhs_col_cnt = lhs->getOutputSchema().getFieldCount();
                lhs_sort = lhs->getSortOrder();

                for (int i = 0; i < join_key_idxs.size(); ++i) {
                    int idx = join_key_idxs[i].second;
                    // if lhs_sort is blank
                    if(!lhs_sort.empty())
                        rhs_sort.emplace_back(ColumnSort(idx - lhs_col_cnt, lhs_sort[i].second));
                    else
                        rhs_sort.emplace_back(ColumnSort(idx - lhs_col_cnt, SortDirection::ASCENDING));
                }

                rhs_sorter = new Sort<B>(rhs->clone(), rhs_sort);
            }
            if(!lhs_sort_compatible){
                rhs_sort = rhs->getSortOrder();

                for (int i = 0; i < join_key_idxs.size(); ++i) {
                    int idx = join_key_idxs[i].first;
                    if(!rhs_sort.empty())
                        lhs_sort.emplace_back(ColumnSort(idx, rhs_sort[i].second));
                    else
                        lhs_sort.emplace_back(ColumnSort(idx, SortDirection::ASCENDING));
                }

                lhs_sorter = new Sort<B>(lhs->clone(), lhs_sort);
            }

            if(!(lhs_sort_compatible && rhs_sort_compatible)){
                Operator<B>* smj_presorted;

                if(!lhs_sort_compatible && rhs_sort_compatible)
                    smj_presorted = new KeyedSortMergeJoin<B>(lhs_sorter, rhs->clone(), foreign_key,
                                                              join_condition->clone());
                else if(lhs_sort_compatible && !rhs_sort_compatible)
                    smj_presorted = new KeyedSortMergeJoin<B>(lhs->clone(), rhs_sorter, foreign_key,
                                                              join_condition->clone());
                else if(!lhs_sort_compatible && !rhs_sort_compatible)
                    smj_presorted = new KeyedSortMergeJoin<B>(lhs_sorter, rhs_sorter, foreign_key,
                                                              join_condition->clone());

                smj_cost = OperatorCostModel::operatorCost((SecureOperator *) smj_presorted);

                string selected_join = (smj_cost < nlj_cost) ? "keyed-sort-merge-join" : "nested-loop-join";

                log->write("Operator (" + std::to_string(operator_id) + "). " +
                           "smj cost : " + std::to_string(smj_cost) +
                           ", nlj cost : " + std::to_string(nlj_cost) +
                           ", join type : " + selected_join, Level::DEBUG);

                if (selected_join == "keyed-sort-merge-join") {
                    if(!lhs_sort_compatible && rhs_sort_compatible) {
                        lhs_sorter = new Sort<B>(lhs, lhs_sort);
                        return new KeyedSortMergeJoin<B>(lhs_sorter, rhs, foreign_key,
                                                         join_condition->clone());
                    }
                    else if(lhs_sort_compatible && !rhs_sort_compatible) {
                        rhs_sorter = new Sort<B>(rhs, rhs_sort);
                        return new KeyedSortMergeJoin<B>(lhs, rhs_sorter, foreign_key,
                                                         join_condition->clone());
                    }
                    else if(!lhs_sort_compatible && !rhs_sort_compatible)
                        lhs_sorter = new Sort<B>(lhs, lhs_sort);
                    rhs_sorter = new Sort<B>(rhs, rhs_sort);
                    return new KeyedSortMergeJoin<B>(lhs_sorter, rhs_sorter, foreign_key,
                                                     join_condition->clone());
                }
                else {
                    return new KeyedJoin<B>(lhs, rhs, foreign_key, join_condition);
                }
            }

            string selected_join = (smj_cost < nlj_cost) ? "keyed-sort-merge-join" : "nested-loop-join";

            log->write("Operator (" + std::to_string(operator_id) + "). " +
                       "smj cost : " + std::to_string(smj_cost) +
                       ", nlj cost : " + std::to_string(nlj_cost) +
                       ", join type : " + selected_join, Level::DEBUG);

            if (selected_join == "keyed-sort-merge-join") {
                return new KeyedSortMergeJoin<B>(lhs, rhs, foreign_key, join_condition);
            }
            else {
                return new KeyedJoin<B>(lhs, rhs, foreign_key, join_condition);
            }

        } // end join-algorithm="auto"

        if (join_algo == "keyed-sort-merge-join") {
            return new KeyedSortMergeJoin<B>(lhs, rhs, foreign_key, join_condition);
        }
        else if (join_algo == "cost-keyed-sort-merge-join") {
            auto smj = new KeyedSortMergeJoin<B>(lhs->clone(), rhs->clone(), foreign_key, join_condition->clone());
            size_t smj_cost = OperatorCostModel::operatorCost((SecureOperator *) smj);
            size_t smj_presorted_cost;

            auto join_key_idxs = smj->joinKeyIdxs();

            bool lhs_sort_compatible = smj->sortCompatible(lhs);
            bool rhs_sort_compatible = smj->sortCompatible(rhs);

            delete smj;

            Operator<B> *lhs_sorter, *rhs_sorter;
            SortDefinition lhs_sort, rhs_sort;

            if(!rhs_sort_compatible){
                int lhs_col_cnt = lhs->getOutputSchema().getFieldCount();
                lhs_sort = lhs->getSortOrder();

                for (int i = 0; i < join_key_idxs.size(); ++i) {
                    int idx = join_key_idxs[i].second;
                    // if lhs_sort is blank
                    if(!lhs_sort.empty())
                        rhs_sort.emplace_back(ColumnSort(idx - lhs_col_cnt, lhs_sort[i].second));
                    else
                        rhs_sort.emplace_back(ColumnSort(idx - lhs_col_cnt, SortDirection::ASCENDING));
                }

                rhs_sorter = new Sort<B>(rhs->clone(), rhs_sort);
            }
            if(!lhs_sort_compatible){
                rhs_sort = rhs->getSortOrder();

                for (int i = 0; i < join_key_idxs.size(); ++i) {
                    int idx = join_key_idxs[i].first;
                    if(!rhs_sort.empty())
                        lhs_sort.emplace_back(ColumnSort(idx, rhs_sort[i].second));
                    else
                        lhs_sort.emplace_back(ColumnSort(idx, SortDirection::ASCENDING));
                }

                lhs_sorter = new Sort<B>(lhs->clone(), lhs_sort);
            }

            std::cout << "Operator (" + std::to_string(operator_id) + "). " +
                         "smj cost : " + std::to_string(smj_cost) +
                         ", smj presorted cost : ";

            if(!(lhs_sort_compatible && rhs_sort_compatible)) {
                Operator<B> *smj_presorted;

                if (!lhs_sort_compatible && rhs_sort_compatible) {
                    // TODO : HERE THERE ARE SOME ISSUE, BECAUSE FOR THE SORT OPT BLOCK
                    // NEED TO CONSIDER THAT LHS/RHS IS INPUT AND SORTED, THEN IT HAS MORE COST THAN INPUT WITH NO SORTED.
                    // BUT ITS HARD TO COMPARE WITH NO SORTED ONE
                    smj_presorted = new KeyedSortMergeJoin<B>(lhs_sorter, rhs->clone(), foreign_key,
                                                              join_condition->clone());
                    smj_presorted_cost = OperatorCostModel::operatorCost((SecureOperator *) smj_presorted);
                    std::cout << std::to_string(smj_presorted_cost) + "(smj cost) + " ;
                    size_t lhs_sort_cost = OperatorCostModel::operatorCost((SecureOperator *) lhs_sorter);
                    smj_presorted_cost += lhs_sort_cost;
                    std::cout << std::to_string(lhs_sort_cost) + "(lhs sort cost) = ";
                } else if (lhs_sort_compatible && !rhs_sort_compatible) {
                    smj_presorted = new KeyedSortMergeJoin<B>(lhs->clone(), rhs_sorter, foreign_key,
                                                              join_condition->clone());
                    smj_presorted_cost = OperatorCostModel::operatorCost((SecureOperator *) smj_presorted);
                    std::cout << std::to_string(smj_presorted_cost) + "(smj cost) + " ;
                    size_t rhs_sort_cost = OperatorCostModel::operatorCost((SecureOperator *) rhs_sorter);
                    smj_presorted_cost += rhs_sort_cost;
                    std::cout << std::to_string(rhs_sort_cost) + "(rhs sort cost) = ";
                } else if (!lhs_sort_compatible && !rhs_sort_compatible) {
                    smj_presorted = new KeyedSortMergeJoin<B>(lhs_sorter, rhs_sorter, foreign_key,
                                                              join_condition->clone());
                    smj_presorted_cost = OperatorCostModel::operatorCost((SecureOperator *) smj_presorted);
                    std::cout << std::to_string(smj_presorted_cost) + "(smj cost) + " ;
                    size_t lhs_sort_cost = OperatorCostModel::operatorCost((SecureOperator *) lhs_sorter);
                    size_t rhs_sort_cost = OperatorCostModel::operatorCost((SecureOperator *) rhs_sorter);
                    smj_presorted_cost += lhs_sort_cost;
                    smj_presorted_cost += rhs_sort_cost;
                    std::cout << std::to_string(lhs_sort_cost) + "(lhs sort cost) + "
                                 + std::to_string(rhs_sort_cost) + "(rhs sort cost) = ";
                }

                string selected_join = (smj_cost < smj_presorted_cost) ? "keyed-sort-merge-join" : "cost-keyed-sort-merge-join";

                std::cout << std::to_string(smj_presorted_cost) +
                             ", join type : " + selected_join;

                if (selected_join == "cost-keyed-sort-merge-join") {
                    if(!lhs_sort_compatible && rhs_sort_compatible) {
                        std::cout << ", lhs not sort compatible, add sort to lhs, rhs sort compatible\n";
                        lhs_sorter = new Sort<B>(lhs, lhs_sort);
                        return new KeyedSortMergeJoin<B>(lhs_sorter, rhs, foreign_key,
                                                         join_condition->clone());
                    }
                    else if(lhs_sort_compatible && !rhs_sort_compatible) {
                        std::cout << ", rhs not sort compatible, add sort to rhs, lhs sort compatible\n";
                        rhs_sorter = new Sort<B>(rhs, rhs_sort);
                        return new KeyedSortMergeJoin<B>(lhs, rhs_sorter, foreign_key,
                                                         join_condition->clone());
                    }
                    else if(!lhs_sort_compatible && !rhs_sort_compatible) {
                        std::cout << ", lhs, rhs both not sort compatible, add sort to lhs, rhs\n";
                        lhs_sorter = new Sort<B>(lhs, lhs_sort);
                        rhs_sorter = new Sort<B>(rhs, rhs_sort);
                        return new KeyedSortMergeJoin<B>(lhs_sorter, rhs_sorter, foreign_key,
                                                         join_condition->clone());
                    }
                }
                else {
                    std::cout << endl;
                    return new KeyedSortMergeJoin<B>(lhs, rhs, foreign_key, join_condition);
                }
            }

            return new KeyedSortMergeJoin<B>(lhs, rhs, foreign_key, join_condition);
        }
        else { // if algorithm unspecified but FK, use KeyedJoin
            return new KeyedJoin<B>(lhs, rhs, foreign_key, join_condition);
        }

    } // end pk-fk join

    if (join_algo == "merge-join") {
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
    if(output_names.size() == (size_t) schema.getFieldCount()) {
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
    return createInputOperator(sql, SortDefinition(), B(false), false, input_limit_);
}

template<typename B>
Operator<B> *PlanParser<B>::parseTableScan(const int & operator_id, const boost::property_tree::ptree &seq_scan_tree) {
    string table_name;
    int local_tuple_limit = input_limit_;
    if(seq_scan_tree.count("table") > 0)
        table_name = seq_scan_tree.get_child("table").template get_value<string>();
    assert(table_name != "");

    if(seq_scan_tree.count("input-limit") > 0)
        local_tuple_limit = seq_scan_tree.get_child("input-limit").template get_value<int>();

    return new TableScan<B>(table_name, local_tuple_limit);
}

template<typename B>
Operator<B> *PlanParser<B>::parsePackedTableScan(const int & operator_id, const boost::property_tree::ptree &packed_table_scan_tree) {
    std::string db_name;
    std::string table_name;
    std::string src_path = Utilities::getCurrentWorkingDirectory();
    std::string packed_pages_path = src_path + "/packed_pages/";

    if(packed_table_scan_tree.count("db") == 1){
        db_name = packed_table_scan_tree.get_child("db").begin()->second.data();
    }

    if(packed_table_scan_tree.count("table") == 1) {
        table_name = packed_table_scan_tree.get_child("table").begin()->second.data();
    }

    return new PackedTableScan<B>(db_name, table_name, packed_pages_path, party_);
}

template<typename B>
Operator<B> *PlanParser<B>::parseLocalScan(const int & operator_id, const boost::property_tree::ptree &local_scan_tree) {
    string sql = "";
    int input_party = 0;
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
    if(op_algo == "merge-input" && std::is_same_v<B, Bit> && local_tuple_limit > 0 && SystemConfiguration::getInstance().emp_mode_ == EmpMode::SH2PC) {
        return createMergeInput(merge_sql, dummy_tag, local_tuple_limit, sort_def, plain_has_dummy_tag);
    }
    return createInputOperator(sql, sort_def, plain_has_dummy_tag, dummy_tag, local_tuple_limit, input_party);

}


template<typename B>
Operator<B> *PlanParser<B>::parseShrinkwrap(const int & operator_id, const boost::property_tree::ptree &pt) {
    int child_op_id = operator_id - 1; // default
    if(pt.count("inputs") > 0) {
        ptree input_list = pt.get_child("inputs.");
        ptree::const_iterator it = input_list.begin();
        child_op_id = it->second.get_value<int>();
    }

    Operator<B> *op = operators_.at(child_op_id);

    size_t output_cardinality;

    if(pt.count("output-cardinality") > 0)
        output_cardinality =  pt.get_child("output_cardinality").template get_value<int>();

    return new Shrinkwrap<B>(op, output_cardinality);
}



template<typename B>
Operator<B> *PlanParser<B>::parseUnion(const int &operator_id, const ptree &union_tree) {

    ptree input_list = union_tree.get_child("inputs.");
    ptree::const_iterator it = input_list.begin();
    int lhs_id = it->second.get_value<int>();
    Operator<B> *lhs  = operators_.at(lhs_id);
    ++it;
    int rhs_id = it->second.get_value<int>();
    Operator<B> *rhs  = operators_.at(rhs_id);

    assert(lhs->getOutputSchema() == rhs->getOutputSchema());
    return new Union<B>(lhs, rhs);

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


template class vaultdb::PlanParser<bool>;
template class vaultdb::PlanParser<emp::Bit>;