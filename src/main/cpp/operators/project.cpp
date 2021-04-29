#include "project.h"
#include <query_table/field/field_factory.h>
#include <query_table/secure_tuple.h>  // do not delete this - need for template specialization
#include <expression/expression_node.h>
#include <expression/generic_expression.h>
#include <util/data_utilities.h>


using namespace vaultdb;

// can't fully initialize schemas yet, don't have child schema

template<typename B>
Project<B>::Project(Operator<B> *child) : Operator<B>(child) {}

template<typename B>
Project<B>::Project(shared_ptr<QueryTable<B> > src) : Operator<B>(src) {}

template<typename B>
Project<B>::Project(Operator<B> *child, std::map<uint32_t, shared_ptr<Expression<B> > > expression_map) : Operator<B>(child), expressions_(expression_map) {


}

template<typename B>
Project<B>::Project(shared_ptr<QueryTable<B> > child, std::map<uint32_t, shared_ptr<Expression<B> > > expression_map) : Operator<B>(child), expressions_(expression_map) {


}


template<typename B>
std::shared_ptr<QueryTable<B> > Project<B>::runSelf() {

    std::shared_ptr<QueryTable<B> > src_table = Operator<B>::children_[0]->getOutput();

    SortDefinition src_sort_order = src_table->getSortOrder();
    QuerySchema src_schema = *src_table->getSchema();

    assert(expressions_.size() > 0);

    uint32_t col_count = expressions_.size();

    QuerySchema dst_schema = QuerySchema(col_count); // re-initialize it
    uint32_t tuple_cnt_ = src_table->getTupleCount();


    // propagate names and specs in column mappings
    for(ProjectionMapping mapping_idx : column_mappings_) {
        uint32_t src_ordinal = mapping_idx.first;
        uint32_t dst_ordinal = mapping_idx.second;
        QueryFieldDesc src_field_desc = src_schema.getField(src_ordinal);
        QueryFieldDesc dst_field_desc(src_field_desc, dst_ordinal);
        dst_schema.putField(dst_field_desc);

        assert(expressions_.find(dst_ordinal) != expressions_.end());
        std::shared_ptr<Expression<B> > expression = expressions_[dst_ordinal];
        expression->setType(src_field_desc.getType());
        expression->setAlias(src_field_desc.getName());

    }


    // set up dst schema for new expressions
    for(auto expr_pos =  expressions_.begin(); expr_pos != expressions_.end(); ++expr_pos) {
        uint32_t dst_ordinal = expr_pos->first;
        QueryFieldDesc dst_field_desc = dst_schema.getField(dst_ordinal);

        if(dst_field_desc.getType() == FieldType::INVALID) { //  not initialized yet
            FieldType dst_type = expr_pos->second->getType();
            std::string dst_name = expr_pos->second->getAlias();

            dst_field_desc = QueryFieldDesc(dst_ordinal, dst_name, "", dst_type, 0); // 0 because string expressions are not yet implemented
            dst_schema.putField(dst_field_desc);
        }
    }


    // confirm that all ordinals are defined
    for(uint32_t i = 0; i < col_count; ++i) {
        assert(expressions_.find(i) != expressions_.end());
    }


    // Only carry over the order by for ones that exist in the output schema
    SortDefinition  dst_sort;
    std::cout << "Project source sort order: " << DataUtilities::printSortDefinition(src_sort_order) << " source schema: " << src_schema << std::endl;
    std::cout << "projection cols: ";
    for(ProjectionMapping m : column_mappings_) {
        std::cout << "<" << m.first << ", " << m.second << "> ";
    }
    std::cout << std::endl;

    // TODO: fix this to account for partial sort carry-over
    // *** Check to see if order-by carries over
    bool sort_carry_over = true;
    for(ColumnSort sort : src_sort_order) {
        uint32_t src_ordinal = sort.first;
        bool found = false;
        for(ProjectionMapping mapping : column_mappings_) {
            if(mapping.first == src_ordinal) {
                dst_sort.push_back(ColumnSort (mapping.second, sort.second));
                found = true;
            }
        } // end search for mapping
        if(!found) { sort_carry_over = false; }
    }

    // *** Done defining schema and verifying setup
    std::cout << "Project sort contains " << dst_sort.size() << " cols. " << std::endl;
    Operator<B>::output = std::shared_ptr<QueryTable<B> >(new QueryTable<B>(tuple_cnt_, dst_schema, dst_sort));


    for(uint32_t i = 0; i < tuple_cnt_; ++i) {
        QueryTuple<B> src_tuple = src_table->getTuple(i);
        QueryTuple<B> dst_tuple = Operator<B>::output->getTuple(i);
        project_tuple(dst_tuple, src_tuple);
    }

    return Operator<B>::output;
}


template<typename B>
void Project<B>::project_tuple(QueryTuple<B> &dst_tuple, QueryTuple<B> &src_tuple) const {
    dst_tuple.setDummyTag(src_tuple.getDummyTag());

   auto exprPos = expressions_.begin();


    // exec all expressions
    while(exprPos != expressions_.end()) {
        uint32_t dst_ordinal = exprPos->first;
        Expression<B> *expression = exprPos->second.get();
        Field<B> field_value = expression->call(src_tuple);
        dst_tuple.setField(dst_ordinal, field_value);
        ++exprPos;
    }


}
template<typename B>
void Project<B>::addColumnMappings(const ProjectionMappingSet &mapSet) {
    for(ProjectionMapping mapping: mapSet)
    {
        addColumnMapping(mapping.first, mapping.second);
    }

}

template<typename B>
void Project<B>::addExpression(const shared_ptr<Expression<B>> &expression, const uint32_t &dstOrdinal) {
    expressions_[dstOrdinal] = expression;
}

template<typename B>
void Project<B>::addColumnMapping(const uint32_t &src_ordinal, const uint32_t &dst_ordinal) {
    ProjectionMapping m(src_ordinal, dst_ordinal);
    column_mappings_.template emplace_back(m);
}

template<typename B>
void Project<B>::addInputReference(const uint32_t & src_ordinal, const uint32_t & dst_ordinal) {

    shared_ptr<ExpressionNode<B> > read_expression_node(new InputReferenceNode<B>(src_ordinal));
    shared_ptr<GenericExpression<B> > read_expression(new GenericExpression<B>(read_expression_node, "unknown", FieldType::UNKNOWN));

    expressions_[dst_ordinal] = read_expression;
}


template class vaultdb::Project<bool>;
template class vaultdb::Project<emp::Bit>;
