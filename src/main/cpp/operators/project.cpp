#include "project.h"
#include <query_table/field/field_factory.h>
#include <query_table/secure_tuple.h>  // do not delete this - need for template specialization
#include <expression/expression_node.h>
#include <expression/generic_expression.h>
#include <util/data_utilities.h>


using namespace vaultdb;






template<typename B>
std::shared_ptr<QueryTable<B> > Project<B>::runSelf() {

    std::shared_ptr<QueryTable<B> > src_table = Operator<B>::children_[0]->getOutput();
    uint32_t tuple_cnt = src_table->getTupleCount();



    Operator<B>::output_ = std::shared_ptr<QueryTable<B> >(new QueryTable<B>(tuple_cnt, Operator<B>::output_schema_, Operator<B>::getSortOrder()));


    for(uint32_t i = 0; i < tuple_cnt; ++i) {
        QueryTuple<B> src_tuple = src_table->getTuple(i);
        QueryTuple<B> dst_tuple = Operator<B>::output_->getTuple(i);
        project_tuple(dst_tuple, src_tuple);
    }


    return Operator<B>::output_;
}


template<typename B>
void Project<B>::project_tuple(QueryTuple<B> &dst_tuple, QueryTuple<B> &src_tuple) const {
    dst_tuple.setDummyTag(src_tuple.getDummyTag());

    auto expr_pos = expressions_.begin();


    // exec all expressions
    while(expr_pos != expressions_.end()) {
        uint32_t dst_ordinal = expr_pos->first;
        Expression<B> *expression = expr_pos->second.get();
        Field<B> field_value = expression->call(src_tuple);
        dst_tuple.setField(dst_ordinal, field_value);
        ++expr_pos;
    }


}


template<typename B>
void Project<B>::setup() {

    Operator<B> *child = Operator<B>::getChild();
    SortDefinition src_sort_order = child->getSortOrder();
    QuerySchema src_schema = child->getOutputSchema();

    assert(expressions_.size() > 0);

    uint32_t col_count = expressions_.size();

    QuerySchema dst_schema;

    for(auto expr_pos =  expressions_.begin(); expr_pos != expressions_.end(); ++expr_pos) {
        if(expr_pos->second->kind() == ExpressionKind::INPUT_REF) {
            GenericExpression<B> *expr = (GenericExpression<B> *) expr_pos->second.get();
            InputReferenceNode<B> *node = (InputReferenceNode<B> *) expr->root_.get();
            column_mappings_.template emplace_back(node->read_idx_, expr_pos->first);
        }
    }

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
        if(!dst_schema.fieldInitialized(dst_ordinal)) {
            FieldType dst_type = expr_pos->second->getType();
            std::string dst_name = expr_pos->second->getAlias();
            QueryFieldDesc dst_field_desc = QueryFieldDesc(dst_ordinal, dst_name, "", dst_type, 0); // 0 because string expressions are not yet implemented
            dst_schema.putField(dst_field_desc);
        }
    }

    dst_schema.initializeFieldOffsets();

    // confirm that all ordinals are defined
    for(uint32_t i = 0; i < col_count; ++i) {
        assert(expressions_.find(i) != expressions_.end());
    }


    SortDefinition  dst_sort;

    // *** Check to see if order-by carries over
    for(ColumnSort sort : src_sort_order) {
        int32_t src_ordinal = sort.first;
        bool found = false;
        if(src_ordinal == -1) {
            found = true; // dummy tag automatically carries over
            dst_sort.push_back(sort);
        }
        for(ProjectionMapping mapping : column_mappings_) {
            if(mapping.first == src_ordinal) {
                dst_sort.push_back(ColumnSort (mapping.second, sort.second));
                found = true;
                break;
            }
        } // end search for mapping
        if(!found) {
            break;
        } // broke the sequence of mappings
    }

    Operator<B>::output_schema_ = dst_schema;
    Operator<B>::setSortOrder(dst_sort);



}



// **** ExpressionMapBuilder **** //
template<typename B>
ExpressionMapBuilder<B>::ExpressionMapBuilder(const QuerySchema &input_schema) : src_schema_(input_schema) {}


template<typename B>
void ExpressionMapBuilder<B>::addMapping(const uint32_t &src_idx, const uint32_t &dst_idx) {
    shared_ptr<ExpressionNode<B> > node(new InputReferenceNode<B>(src_idx));
    shared_ptr<GenericExpression<B> > expr(new GenericExpression<B>(node, src_schema_));

    expressions_[dst_idx] = expr;
}

template<typename B>
void ExpressionMapBuilder<B>::addExpression(const shared_ptr<Expression<B>> &expression, const uint32_t &dst_idx) {
    expressions_[dst_idx] = expression;
}



template class vaultdb::Project<bool>;
template class vaultdb::Project<emp::Bit>;


template class vaultdb::ExpressionMapBuilder<bool>;
template class vaultdb::ExpressionMapBuilder<emp::Bit>;