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
#include <operators/shrinkwrap.h>
#include <util/logger.h>
#include <operators/support/aggregate_id.h>
#include <boost/algorithm/string.hpp>


using namespace vaultdb;


template<typename B>
void PlanDeparser<B>::deparseTree() {

    // make a pass to ensure that all operator IDs are unique


    deparseTreeHelper(root_);
    pt::ptree base_node;
    base_node.add_child("rels", rels_);
    stringstream ss;
    pt::write_json(ss, base_node);
    json_plan_ = ss.str();

    vector<string> search_strs = {"\"dummy-tag\"", "\"input-party\"", "\"input-limit\"", "\"field\"", "\"input\"", "\"foreign-key\"", "\"nullable\"", " \"literal\"", "precision", "\"scale\"", "\"output-cardinality\"", "\"no-op\""};

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
    writeHeader(input_node, input, "LogicalValues");
//    auto schema = deparseSchema(in->getOutputSchema());
//    input_node.add_child("type", schema);
    input_node.put("sql", in->input_query_);
    input_node.put<bool>("dummy-tag", in->has_dummy_tag_);
    input_node.put("db-name", in->db_name_);
    input_node.put("input-party", in->input_party_);
    input_node.put("input-limit", in->input_tuple_limit_);
    input_node.put("outputFields", generateFieldList(in->getOutputSchema()));
    if(!in->getSortOrder().empty()) {
        auto collation = deparseCollation(in->getSortOrder());
        input_node.add_child("collation", collation);
    }
    return input_node;
}

template<typename B>
pt::ptree PlanDeparser<B>::deparseSqlInput(const Operator<B> *input) {
    assert(input->getType() == OperatorType::SQL_INPUT);
    auto in = (SqlInput *) input;
    pt::ptree input_node;

    writeHeader(input_node, input, "LogicalValues");
    // auto schema = deparseSchema(in->getOutputSchema());
    //input_node.add_child("type", schema);


    input_node.put("sql", in->input_query_);
    input_node.put<bool>("dummy-tag", in->dummy_tagged_);
    input_node.put("outputFields", generateFieldList(in->getOutputSchema()));
    input_node.put("db-name", in->db_name_);
    input_node.put<int>("input-limit", in->tuple_limit_);
    if(!in->getSortOrder().empty()) {
        auto collation = deparseCollation(in->getSortOrder());
        input_node.add_child("collation", collation);
    }
    return input_node;

}
template<typename B>
pt::ptree PlanDeparser<B>::deparseSortMergeJoin(const Operator<B> *input) {
    assert(input->getType() == OperatorType::SORT_MERGE_JOIN);
    auto smj = (SortMergeJoin<B> *) input;
    return deparseJoin(smj, "sort-merge-join", smj->foreignKeyChild());
}

template<typename B>
pt::ptree PlanDeparser<B>::deparseMergeJoin(const Operator<B> *input) {
    assert(input->getType() == OperatorType::MERGE_JOIN);
    auto mj = (MergeJoin<B> *) input;
    return deparseJoin(mj, "merge-join", -1);
}

template<typename B>
pt::ptree PlanDeparser<B>::deparseNestedLoopAggregate(const Operator<B> *input) {
    assert(input->getType() == OperatorType::NESTED_LOOP_AGGREGATE);
    auto nla = (NestedLoopAggregate<B> *) input;
    return deparseGroupByAggregate(nla, "nested-loop-aggregate");
 }

template<typename B>
pt::ptree PlanDeparser<B>::deparseShrinkwrap(const Operator<B> *input) {
    assert(input->getType() == OperatorType::SHRINKWRAP);
    auto shrinkwrap = (Shrinkwrap<B> *) input;

    pt::ptree shrinkwrap_node;
    writeHeader(shrinkwrap_node, input, "LogicalShrinkwrap");


    shrinkwrap_node.put("output-cardinality", shrinkwrap->getOutputCardinality());
    return shrinkwrap_node;
}

template<typename B>
pt::ptree PlanDeparser<B>::deparseUnion(const Operator<B> *input) {
    assert(input->getType() == OperatorType::UNION);
    auto union_op = (Union<B> *) input;
    ptree union_node;
    writeHeader(union_node, input, "LogicalUnion");


    return union_node;
}

template<typename B>
pt::ptree PlanDeparser<B>::deparseKeyedNestedLoopJoin(const Operator<B> *input) {
    assert(input->getType() == OperatorType::KEYED_NESTED_LOOP_JOIN);
    auto kj = (KeyedJoin<B> *) input;
    return deparseJoin(kj, "nested-loop-join", kj->foreignKeyChild());
}



template<typename B>
pt::ptree PlanDeparser<B>::deparseZkSqlInput(const Operator<B> *input) {
    assert(input->getType() == OperatorType::ZK_SQL_INPUT);
    auto in = (ZkSqlInput *) input;
    pt::ptree input_node;

    writeHeader(input_node, input, "LogicalValues");
    // auto schema = deparseSchema(in->getOutputSchema());
    //input_node.add_child("type", schema);


    input_node.put("sql", in->input_query_);
    input_node.put<bool>("dummy-tag", in->has_dummy_tag_);
    input_node.put("outputFields", generateFieldList(in->getOutputSchema()));
    input_node.put("db-name", in->db_name_);
    input_node.put<int>("input-limit", in->input_tuple_limit_);
    if(!in->getSortOrder().empty()) {
        auto collation = deparseCollation(in->getSortOrder());
        input_node.add_child("collation", collation);
    }
    return input_node;
}

template<typename B>
pt::ptree PlanDeparser<B>::deparseCsvInput(const Operator<B> *input) {
    throw; // not yet implemented

}

template<typename B>
pt::ptree PlanDeparser<B>::deparseScalarAggregate(const Operator<B> *agg) {
    throw; // not yet implemented

}

template<typename B>
pt::ptree PlanDeparser<B>::deparseTableInput(const Operator<B> *input) {
    //  not yet implemented, this operator is largely for debugging anyway
    throw;
}

template<typename B>
pt::ptree PlanDeparser<B>::deparseProject(const Operator<B> *project) {
    assert(project->getType() == OperatorType::PROJECT);
    auto proj = (Project<B> *) project;

    pt::ptree project_node;
    project_node.put("id", proj->getOperatorId());
    project_node.put("relOp", "LogicalProject");

    project_node.put("inputFields", generateFieldList(proj->getChild()->getOutputSchema()));

    QuerySchema output_schema = proj->getOutputSchema();
    project_node.put("outputFields", generateFieldList(output_schema));

    ptree field_names, exprs;
    for(int i = 0; i < output_schema.getFieldCount(); ++i) {
        string field_name = output_schema.getField(i).getName();
        field_names.put_value(field_name);
    }
    project_node.add_child("fields", field_names);

    auto expression_map = proj->getExpressions();
    for(auto pos : expression_map) {
        ptree deparsed = ExpressionDeparser<B>::deparse(pos.second);
        exprs.push_back(std::make_pair("", deparsed));
    }

    project_node.add_child("exprs", exprs);
    return project_node;
}

template<typename B>
pt::ptree PlanDeparser<B>::deparseFilter(const Operator<B> *filter) {
    assert(filter->getType() == OperatorType::FILTER);

   pt:ptree filter_node;
    filter_node.put("id", filter->getOperatorId());
    filter_node.put("relOp", "LogicalFilter");
    filter_node.put("inputFields", generateFieldList(filter->getChild()->getOutputSchema()));
    filter_node.put("outputFields", generateFieldList(filter->getOutputSchema()));

    auto deparsed = ExpressionDeparser<B>::deparse(((Filter<B> *)filter)->predicate_);
    filter_node.add_child("condition", deparsed);

    return filter_node;

}

template<typename B>
pt::ptree PlanDeparser<B>::deparseNestedLoopJoin(const Operator<B> *join) {
    assert(join->getType() == OperatorType::NESTED_LOOP_JOIN);
    auto nlj = (BasicJoin<B> *) join;
    return deparseJoin(nlj, "nested-loop-join", -1);
}

template<typename B>
pt::ptree PlanDeparser<B>::deparseSort(const Operator<B> *sort) {
    assert(sort->getType() == OperatorType::SORT);
    auto sort_op = (Sort<B> *) sort;
    pt::ptree sort_node;

    writeHeader(sort_node, sort, "LogicalSort");
    auto collation = deparseCollation(sort_op->getSortOrder());
    sort_node.add_child("collation", collation);

    int sort_limit = sort_op->getLimit();
    if(sort_limit > 0) {
        PlainField limit_field = PlainField(FieldType::INT, sort_limit);
        LiteralNode<bool> fetch_literal(limit_field);
        ptree expr_json = ExpressionDeparser<bool>::deparse(&fetch_literal);
        sort_node.add_child("fetch", expr_json);
    }


    return sort_node;
}

template<typename B>
pt::ptree PlanDeparser<B>::deparseSortMergeAggregate(const Operator<B> *agg) {
    assert(agg->getType() == OperatorType::SORT_MERGE_AGGREGATE);
    auto sma = (SortMergeAggregate<B> *) agg;
    return deparseGroupByAggregate(sma, "sort-merge-aggregate");
}



template<typename B>
void PlanDeparser<B>::validateTree() {
    map<int, int> known_ids;
    validateTreeHelper(root_, known_ids);
}


// check for duplicate operator IDs and replace them as needed
template<typename B>
void PlanDeparser<B>::validateTreeHelper(Operator<B> *node, map<int, int> & known_ids) {
        if(known_ids.find(node->getOperatorId()) != known_ids.end()) {
            int max_id = 0;
            for(auto pos : known_ids) {
                max_id = std::max(max_id, pos.second);
            }
        int new_id = max_id + 1;
        node->setOperatorId(new_id);
    }
    known_ids[node->getOperatorId()] = 1;


    if(node->getChild() != nullptr) validateTreeHelper(node->getChild(), known_ids);
    if(node->getChild(1) != nullptr) validateTreeHelper(node->getChild(1), known_ids);

}


template class vaultdb::PlanDeparser<bool>;
template class vaultdb::PlanDeparser<emp::Bit>;