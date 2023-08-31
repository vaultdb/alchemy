#include <parser/plan_deparser.h>
#include <boost/property_tree/json_parser.hpp>
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
#include <operators/support/aggregate_id.h>
#include <boost/algorithm/string.hpp>


using namespace vaultdb;


template<typename B>
void PlanDeparser<B>::deparseTree() {

    deparseTreeHelper(root_);
    pt::ptree base_node;
    base_node.add_child("rels", rels_);
    stringstream ss;
    pt::write_json(ss, base_node);
    json_plan_ = ss.str();

    vector<string> search_strs = {"\"dummy-tag\"", "\"input-party\"", "\"input-limit\"", "\"field\"", "\"input\"", "\"foreign-key\"", "\"nullable\"", " \"literal\"", "precision", "\"scale\""};

   json_plan_ = Utilities::eraseValueQuotes(json_plan_, search_strs);

}


template<typename B>
void PlanDeparser<B>::deparseTreeHelper(const Operator<B> *node) {
    if(node->getChild() != nullptr) deparseTreeHelper(node->getChild());
    if(node->getChild(1) != nullptr) deparseTreeHelper(node->getChild(1));

    auto ptree = deparseNode(node);
    rels_.push_back( std::make_pair("", ptree));
}


template<typename B>
pt::ptree PlanDeparser<B>::deparseNode(const Operator<B> *node) {

    switch(node->getType()) {
        case OperatorType::TABLE_INPUT:
            return deparseTableInput(node);
        case OperatorType::SECURE_SQL_INPUT:
            return deparseSecureSqlInput(node);
        case OperatorType::ZK_SQL_INPUT:
            return deparseZkSqlInput(node);
        case OperatorType::SQL_INPUT:
            return deparseSqlInput(node);
        case OperatorType::CSV_INPUT:
            return deparseCsvInput(node);
        case OperatorType::FILTER:
            return deparseFilter(node);
        case OperatorType::PROJECT:
            return deparseProject(node);
        case OperatorType::NESTED_LOOP_JOIN:
            return deparseNestedLoopJoin(node);
        case OperatorType::KEYED_NESTED_LOOP_JOIN:
            return deparseKeyedNestedLoopJoin(node);
        case OperatorType::SORT_MERGE_JOIN:
            return deparseSortMergeJoin(node);
        case OperatorType::MERGE_JOIN:
            return deparseMergeJoin(node);
        case OperatorType::SORT:
            return deparseSort(node);
        case OperatorType::SHRINKWRAP:
            return deparseShrinkwrap(node);
        case OperatorType::SCALAR_AGGREGATE:
            return deparseScalarAggregate(node);
        case OperatorType::SORT_MERGE_AGGREGATE:
            return deparseSortMergeAggregate(node);
        case OperatorType::NESTED_LOOP_AGGREGATE:
            return deparseNestedLoopAggregate(node);
        case OperatorType::UNION:
            return deparseUnion(node);
        default:
           return pt::ptree();
    }

}

template<typename B>
pt::ptree PlanDeparser<B>::deparseCollation(const SortDefinition &sort) {
    pt::ptree  collation;
    for(auto col_sort : sort) {
        pt::ptree col;
        col.put("field", col_sort.first);
        col.put("direction", col_sort.second == SortDirection::ASCENDING ? "ASCENDING" : "DESCENDING");
        collation.push_back(std::make_pair("", col));
    }
    return collation;
}

template<typename B>
pt::ptree PlanDeparser<B>::deparseSchema(const QuerySchema &schema) {
    pt::ptree column_defs;
    for(int i = 0; i < schema.getFieldCount(); ++i) {
        pt::ptree col;
        QueryFieldDesc field = schema.getField(i);
        col.put("type", TypeUtilities::getJSONTypeString(field.getType()));
        if(field.getType() == FieldType::STRING || field.getType() == FieldType::SECURE_STRING) {
            col.put("precision", field.getStringLength());
        }
        col.put("name", field.getName());

        column_defs.push_back(std::make_pair("", col));
    }
    return column_defs;
}


template<typename B>
pt::ptree  PlanDeparser<B>::deparseSecureSqlInput(const Operator<B> *input) {
    assert(input->getType() == OperatorType::SECURE_SQL_INPUT);
    auto in = (SecureSqlInput *) input;
    pt::ptree input_node;
    input_node.put("id", input->getOperatorId());
    input_node.put("relOp", "LogicalValues");
    auto schema = deparseSchema(in->getOutputSchema());
    input_node.add_child("type", schema);
    input_node.put("sql", in->input_query_);
    input_node.put<bool>("dummy-tag", in->has_dummy_tag_);
    input_node.put("db-name", in->db_name_);
    input_node.put("input-party", in->input_party_);
    input_node.put("input-limit", in->input_tuple_limit_);
    auto collation = deparseCollation(in->getSortOrder());
    input_node.add_child("collation", collation);
    return input_node;
}

template<typename B>
pt::ptree PlanDeparser<B>::deparseSqlInput(const Operator<B> *input) {
    assert(input->getType() == OperatorType::SQL_INPUT);
    auto in = (SqlInput *) input;
    pt::ptree input_node;

    input_node.put("id", input->getOperatorId());
    input_node.put("relOp", "LogicalValues");
    auto schema = deparseSchema(in->getOutputSchema());
    input_node.add_child("type", schema);

    input_node.put("sql", in->input_query_);
    input_node.put<bool>("dummy-tag", in->dummy_tagged_);
    input_node.put("db-name", in->db_name_);
    input_node.put<int>("input-limit", in->tuple_limit_);
    auto collation = deparseCollation(in->getSortOrder());
    input_node.add_child("collation", collation);
    return input_node;

}
template<typename B>
pt::ptree PlanDeparser<B>::deparseSortMergeJoin(const Operator<B> *input) {
    return boost::property_tree::ptree();
}

template<typename B>
pt::ptree PlanDeparser<B>::deparseMergeJoin(const Operator<B> *input) {
    return boost::property_tree::ptree();
}

template<typename B>
pt::ptree PlanDeparser<B>::deparseNestedLoopAggregate(const Operator<B> *input) {
    return boost::property_tree::ptree();
}

template<typename B>
pt::ptree PlanDeparser<B>::deparseShrinkwrap(const Operator<B> *input) {
    return boost::property_tree::ptree();
}

template<typename B>
pt::ptree PlanDeparser<B>::deparseUnion(const Operator<B> *input) {
    return boost::property_tree::ptree();
}

template<typename B>
pt::ptree PlanDeparser<B>::deparseKeyedNestedLoopJoin(const Operator<B> *input) {
    return boost::property_tree::ptree();
}



template<typename B>
pt::ptree PlanDeparser<B>::deparseZkSqlInput(const Operator<B> *input) {
    return boost::property_tree::ptree();
}

template<typename B>
pt::ptree PlanDeparser<B>::deparseCsvInput(const Operator<B> *input) {
    return boost::property_tree::ptree();
}

template<typename B>
pt::ptree PlanDeparser<B>::deparseScalarAggregate(const Operator<B> *agg) {
    return boost::property_tree::ptree();
}

template<typename B>
pt::ptree PlanDeparser<B>::deparseTableInput(const Operator<B> *input) {
    return boost::property_tree::ptree();
}

template<typename B>
pt::ptree PlanDeparser<B>::deparseProject(const Operator<B> *project) {
    return boost::property_tree::ptree();
}

template<typename B>
pt::ptree PlanDeparser<B>::deparseFilter(const Operator<B> *filter) {
    return boost::property_tree::ptree();
}

template<typename B>
pt::ptree PlanDeparser<B>::deparseNestedLoopJoin(const Operator<B> *join) {
    return boost::property_tree::ptree();
}

template<typename B>
pt::ptree PlanDeparser<B>::deparseSort(const Operator<B> *sort) {
    return boost::property_tree::ptree();
}

template<typename B>
pt::ptree PlanDeparser<B>::deparseSortMergeAggregate(const Operator<B> *agg) {
    assert(agg->getType() == OperatorType::SORT_MERGE_AGGREGATE);
    auto sma = (GroupByAggregate<B> *) agg;
    pt::ptree sma_node;
    sma_node.put("id", sma->getOperatorId());
    sma_node.put("relOp", "LogicalAggregate");
    sma_node.put("operator-algorithm", "sort-merge-aggregate");
    sma_node.put<int>("cardinality-bound", sma->getOutputCardinality());

    pt::ptree group_bys;
    for(auto col : sma->group_by_) {
        pt::ptree col_ordinal;
        col_ordinal.put_value(col);
        group_bys.push_back(std::make_pair("", col_ordinal));
    }

    sma_node.add_child("group", group_bys);
    return sma_node;
}

template class vaultdb::PlanDeparser<bool>;
template class vaultdb::PlanDeparser<emp::Bit>;
