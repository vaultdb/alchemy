#include "operators/merge_input.h"
#include "query_table/query_table.h"


MergeInput::MergeInput(const string &db, const string &sql, const bool &dummy_tag,
                       const size_t &input_tuple_cnt, const SortDefinition &def) : Operator<Bit>(def) {
    // only needed for truncation, otherwise just use SecureSqlInput
    assert(input_tuple_cnt > 0);
    // Alice
    lhs_ = new SecureSqlInput(db, sql, dummy_tag, 1, input_tuple_cnt, def);
    // Bob
    rhs_ = new SecureSqlInput(db, sql, dummy_tag, 2,  input_tuple_cnt,def);
    this->output_cardinality_ = input_tuple_cnt;
    this->output_schema_ = lhs_->getOutputSchema();

}

QueryTable<Bit> *MergeInput::runSelf() {
    SecureTable *lhs = lhs_->run();
    lhs->pinned_ = true;
    SecureTable *rhs = rhs_->run();
    int tuple_cnt = lhs->tuple_cnt_;

    // ensure they line up
    assert (tuple_cnt == rhs->tuple_cnt_);

    this->output_ =  QueryTable<Bit>::getTable(lhs->tuple_cnt_, this->output_schema_, this->sort_definition_);

    for(int i = 0; i < tuple_cnt; ++i) {
        Bit lhs_dummy = lhs->getDummyTag(i);
        Bit rhs_dummy = rhs->getDummyTag(i);
        //if lhs is not dummy, then write lhs
        output_->cloneRow(!lhs_dummy, i, 0, lhs, i);
        output_->cloneRow(!rhs_dummy, i, 0, rhs, i);
        // if both are dummies, then write dummy tag as true
        output_->setDummyTag(i, lhs_dummy & rhs_dummy);
    }
    lhs->pinned_ = false;
    return this->output_;
}
