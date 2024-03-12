#include "left_keyed_join.h"

#include "query_table/query_table.h"
#include <util/data_utilities.h>
#include <util/field_utilities.h>

using namespace vaultdb;

template<typename B>
LeftKeyedJoin<B>::LeftKeyedJoin(Operator<B> *foreign_key, Operator<B> *primary_key, Expression<B> *predicate, const SortDefinition & sort)
        : Join<B>(foreign_key, primary_key, predicate, sort) {
            this->output_cardinality_ = foreign_key->getOutputCardinality();
            this->updateCollation();
        }



template<typename B>
LeftKeyedJoin<B>::LeftKeyedJoin(QueryTable<B> *foreign_key, QueryTable<B> *primary_key, Expression<B> *predicate, const SortDefinition & sort)
        : Join<B>(foreign_key, primary_key, predicate, sort) {

            this->output_cardinality_ = foreign_key->tuple_cnt_;
            this->updateCollation();
}


template<typename B>
QueryTable<B> *LeftKeyedJoin<B>::runSelf() {

    QueryTable<B> *lhs_table = this->getChild(0)->getOutput(); // foreign key
    lhs_table->pinned_ = true;
    QueryTable<B> *rhs_table = this->getChild(1)->getOutput(); // primary key

    this->start_time_ = clock_start();
    this->start_gate_cnt_ = this->system_conf_.andGateCount();


    uint32_t output_tuple_cnt = lhs_table->tuple_cnt_; // foreignKeyTable = foreign key
    this->output_ =  QueryTable<B>::getTable(output_tuple_cnt, this->output_schema_, this->sort_definition_);

    B selected, to_update, lhs_dummy_tag, rhs_dummy_tag, dst_dummy_tag;
    int rhs_col_offset = this->output_->getSchema().getFieldCount() - rhs_table->getSchema().getFieldCount();
    // each foreignKeyTable tuple can have at most one match from primaryKeyTable relation
    for(uint32_t i = 0; i < lhs_table->tuple_cnt_; ++i) {

        lhs_dummy_tag = lhs_table->getField(i, -1).template getValue<B>();
        this->output_->cloneRow(i, 0, lhs_table, i); // Join<B>::write_left(this->output_, i, lhs_table, i);
        dst_dummy_tag = lhs_dummy_tag; // carry over previous dummy tag for left-join simulation

        for(uint32_t j = 0; j < rhs_table->tuple_cnt_; ++j) {
            rhs_dummy_tag = rhs_table->getField(j, -1).template getValue<B>();
            selected = Join<B>::predicate_->call(lhs_table, i, rhs_table, j).template getValue<B>();

            to_update = selected & (!lhs_dummy_tag) & (!rhs_dummy_tag);
            dst_dummy_tag = FieldUtilities::select(to_update, false, dst_dummy_tag);
            this->output_->cloneRow(to_update, i, rhs_col_offset, rhs_table, j);   //Join<B>::write_right(to_update, Operator<B>::output_, i, rhs_table, j);
       }

        this->output_->setDummyTag(i, dst_dummy_tag);

    }

    lhs_table->pinned_ = false; // operator will automatically delete children
    return this->output_;

}



template class vaultdb::LeftKeyedJoin<bool>;
template class vaultdb::LeftKeyedJoin<emp::Bit>;

