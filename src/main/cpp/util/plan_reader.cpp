#include "plan_reader.h"
#include "utilities.h"
#include "data_utilities.h"

#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>
#include <iostream>
#include <sstream>
#include <support/aggregate_id.h>

#include <operators/sql_input.h>
#include <operators/secure_sql_input.h>
#include <operators/sort.h>
#include <operators/group_by_aggregate.h>
#include <operators/scalar_aggregate.h>

using namespace vaultdb;
using boost::property_tree::ptree;


template<typename B>
PlanReader<B>::PlanReader(const std::string &db_name, std::string plan_name) : db_name_(db_name) {
    std::string sql_file = Utilities::getCurrentWorkingDirectory() + "/conf/plans/queries-" + plan_name + ".sql";
    std::string plan_file = Utilities::getCurrentWorkingDirectory() + "/conf/plans/mpc-" + plan_name + ".json";

    parseSqlInputs(sql_file);
    parseSecurePlan(plan_file);


}

template<typename B>
PlanReader<B>::PlanReader(const string &db_name, std::string plan_name, NetIO * netio, const int & party ):  db_name_(db_name), netio_(netio), party_(party)
{
    std::string sql_file = Utilities::getCurrentWorkingDirectory() + "/config/plans/queries-" + plan_name + ".sql";
    std::string plan_file = Utilities::getCurrentWorkingDirectory() + "/config/plans/mpc-" + plan_name + ".json";

    parseSqlInputs(sql_file);
    parseSecurePlan(plan_file);

}


template<typename B>
void PlanReader<B>::parseSqlInputs(const std::string & sql_file) {

    vector<std::string> lines = DataUtilities::readTextFile(sql_file);

    std::string query;
    int query_id = 0;
    bool init = false;
    B has_dummy = false;

    for(vector<std::string>::iterator pos = lines.begin(); pos != lines.end(); ++pos) {
        if((*pos).substr(0, 2) == "--") { // starting a new query
            if(init) { // skip the first one
                has_dummy = (query.find("dummy_tag") != std::string::npos);
                operators_[query_id] = createInputOperator(query, has_dummy);

            }

            query = "";
            std::string query_id_str = (*pos).substr(3, (*pos).size() - 3);
            query_id = std::atoi(query_id_str.c_str());
            init = true;
        }
        else {
            query += *pos + "\n";
        }
    }

    // output the last one
    has_dummy = (query.find("dummy") != std::string::npos);
    std::cout << "Parsed input: " << query << " dummy tag? " << has_dummy << std::endl;
    operators_[query_id] = createInputOperator(query, has_dummy);
}

template<typename B>
void PlanReader<B>::parseSecurePlan(const string & plan_file) {
    stringstream ss;
    std::vector<std::string> json_lines = DataUtilities::readTextFile(plan_file);
    for(std::vector<std::string>::iterator pos = json_lines.begin(); pos != json_lines.end(); ++pos)
        ss << *pos << endl;

    boost::property_tree::ptree pt;
    boost::property_tree::read_json(ss, pt);

    //using boost::property_tree::ptree;

    BOOST_FOREACH(boost::property_tree::ptree::value_type &v, pt.get_child("rels."))
                {
                    assert(v.first.empty()); // array elements have no names

                    boost::property_tree::ptree inputs = v.second.get_child("id");
                    int operator_id = v.second.get_child("id").template get_value<int>();
                    string op_name =  (std::string) v.second.get_child("relOp").data();
                    std::cout << "***Parsed op " << operator_id << ": " << op_name << std::endl;
                    parseOperator(operator_id, op_name, v.second);
                }
}



template<typename B>
void PlanReader<B>::parseOperator(const int &operator_id, const string &op_name, const ptree & tree) {
    if(op_name == "LogicalValues") return; // handled in createInput
    if(op_name == "LogicalSort") return parseSort(operator_id, tree);
    if(op_name == "LogicalAggregate") return parseAggregate(operator_id, tree);
    if(op_name == "LogicalJoin") return parseJoin(operator_id, tree);
    if(op_name == "LogicalProjection") return parseProjection(operator_id, tree);
    if(op_name == "LogicalFilter") return parseFilter(operator_id, tree);

    throw std::invalid_argument("Unknown operator type: " + op_name);

}


template<typename B>
void PlanReader<B>::parseSort(const int &operator_id, const boost::property_tree::ptree &sort_tree) {

    boost::property_tree::ptree sort_payload = sort_tree.get_child("collation");
    SortDefinition sort_definition;

    // TODO: conversion for dummy tag, idx = -1 ?
    for (ptree::const_iterator it = sort_payload.begin(); it != sort_payload.end(); ++it) {
        ColumnSort cs;
        cs.first = it->second.get_child("field").get_value<int>(); // field_idx
        std::string direction_str =    it->second.get_child("direction").get_value<std::string>();
        cs.second = (direction_str == "ASCENDING") ? SortDirection::ASCENDING : SortDirection::DESCENDING;
        sort_definition.push_back(cs);
    }


    // TODO: handle LIMIT


    const shared_ptr<Operator<B> > child = getChildOperator(operator_id);
    operators_[operator_id] = shared_ptr<Operator<B> > (new Sort<B>(child.get(), sort_definition));

}

template<typename B>
shared_ptr<Operator<bool>> PlanReader<B>::createInputOperator(const string &sql, const bool &has_dummy_tag) {
    return  std::shared_ptr<Operator<bool> >(new SqlInput(db_name_, sql, has_dummy_tag));
}

template<typename B>
shared_ptr<Operator<emp::Bit>> PlanReader<B>::createInputOperator(const string &sql, const emp::Bit &has_dummy_tag) {
    bool dummy_tag = has_dummy_tag.template reveal(); // just a loop with above, so no security - just aligns template defs
    return  std::shared_ptr<Operator<emp::Bit> >(new SecureSqlInput(db_name_, sql, dummy_tag, netio_, party_));
}



template<typename B>
void PlanReader<B>::parseAggregate(const int &operator_id, const boost::property_tree::ptree &aggregate_json) {

    // parse the aggregators
    std::vector<int> group_by_ordinals;
    vector<ScalarAggregateDefinition> aggregators;
    assert(aggregate_json.count("group") > 0);
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

    const shared_ptr<Operator<B> > child = getChildOperator(operator_id);


    if(!group_by_ordinals.empty()) {
     // instantiate group by aggregate
        operators_[operator_id] = shared_ptr<Operator<B> >(new GroupByAggregate<B>(child.get(), group_by_ordinals, aggregators));
    }
    else {
        operators_[operator_id] = shared_ptr<Operator<B> >(new ScalarAggregate<B>(child.get(), aggregators));
    }


}

template<typename B>
void PlanReader<B>::parseJoin(const int &operator_id, const ptree &pt) {
 throw; // not yet implemented
}

template<typename B>
void PlanReader<B>::parseFilter(const int &operator_id, const ptree &pt) {
    throw; // not yet implemented

}

template<typename B>
void PlanReader<B>::parseProjection(const int &operator_id, const ptree &pt) {
    throw; // not yet implemented

}


// *** Utilities ***

// child is always the "-1" operator if unspecified
template<typename B>
const std::shared_ptr<Operator<B> > PlanReader<B>::getChildOperator(const int &my_operator_id) const {
    int child_id = my_operator_id - 1;
    assert(operators_.count(child_id) != 0); // the key exists
   return operators_.at(child_id);
}




template<typename B>
void PlanReader<B>::print(const boost::property_tree::ptree &pt, const std::string &prefix) {

    ptree::const_iterator end = pt.end();
    for (ptree::const_iterator it = pt.begin(); it != end; ++it) {
        std::cout << prefix <<  it->first << ": " << it->second.get_value<std::string>() << std::endl;
        print(it->second, prefix + "   ");
    }
}



template class vaultdb::PlanReader<bool>;
template class vaultdb::PlanReader<emp::Bit>;
