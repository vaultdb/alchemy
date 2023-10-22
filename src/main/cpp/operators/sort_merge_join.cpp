#include "sort_merge_join.h"
#include "query_table/table_factory.h"
#include "operators/project.h"
#include "expression/visitor/join_equality_condition_visitor.h"
#include "operators/sort.h"
#include "util/data_utilities.h"
#include "util/field_utilities.h"
#include "query_table/field/field_factory.h"
#include "util/system_configuration.h"
#include "util/logger.h"

using namespace vaultdb;
using namespace Logging;

template<typename B>
SortMergeJoin<B>::SortMergeJoin(Operator<B> *lhs, Operator<B> *rhs, Expression<B> *predicate,
                                const SortDefinition &sort) : Join<B>(lhs, rhs, predicate, sort) {
    setup();
}

template<typename B>
void SortMergeJoin<B>::setup() {
    is_secure_ = std::is_same_v<B, emp::Bit>;
    int_field_type_ = is_secure_ ? FieldType::SECURE_INT : FieldType::INT;
    bool_field_type_ = is_secure_ ? FieldType::SECURE_BOOL : FieldType::BOOL;

    auto p = (GenericExpression<B> *) this->predicate_;
    JoinEqualityConditionVisitor<B> join_visitor(p->root_);
    join_idxs_  = join_visitor.getEqualities();

    one_ = FieldFactory<B>::getOne(int_field_type_);
    zero_ = FieldFactory<B>::getZero(int_field_type_);
    bit_packed_ = SystemConfiguration::getInstance().bitPackingEnabled();

    updateCollation();

    max_intermediate_cardinality_ =  this->getChild(0)->getOutputCardinality() * this->getChild(1)->getOutputCardinality();

    if(is_secure_ && bit_packed_) {
        int card_bits = ceil(log2(max_intermediate_cardinality_)) + 1; // + 1 for sign bit
        emp::Integer zero_tmp(card_bits, 0, emp::PUBLIC);
        emp::Integer one_tmp(card_bits, 1, emp::PUBLIC);
        zero_ = Field<B>(int_field_type_, zero_tmp);
        one_ = Field<B>(int_field_type_, one_tmp);
    }

}

template<typename B>
QueryTable<B> *SortMergeJoin<B>::runSelf() {
    QueryTable<B> *lhs = this->getChild(0)->getOutput();
    lhs->pinned_ = true;
    QueryTable<B> *rhs = this->getChild(1)->getOutput();

    this->start_time_ = clock_start();

    QuerySchema lhs_schema = lhs->getSchema();
    QuerySchema rhs_schema = rhs->getSchema();
    QuerySchema out_schema = this->output_schema_;

	Logger* log = get_log();

    log->write("LHS: " + lhs->toString(true), Level::INFO);
	log->write("RHS: " + rhs->toString(true), Level::INFO);

	pair<QueryTable<B> *, QueryTable<B> *> augmented =  augmentTables(lhs, rhs);
    QueryTable<B> *s1, *s2;

//    cout << "augmented sample: " << DataUtilities::printTable(augmented.second, 5, false) << endl;
    s1 = obliviousExpand(augmented.first, true);

	log->write("LHS augmented: " + augmented.first->toString(true), Level::INFO);
	log->write("RHS augmented: " + augmented.second->toString(true), Level::INFO);

	s1 = obliviousExpand(augmented.first, true);
	s2 = obliviousExpand(augmented.second, false);

	log->write("LHS expanded: " + s1->toString(true), Level::INFO);
	log->write("RHS expanded: " + s2->toString(true), Level::INFO);
    delete augmented.first;
	delete augmented.second;

    this->output_ = TableFactory<B>::getTable(max_intermediate_cardinality_, out_schema);

    size_t lhs_field_cnt = lhs_schema.getFieldCount();
    QueryTable<B> *lhs_reverted = revertProjection(s1, lhs_field_mapping_, true);
    QueryTable<B> *rhs_reverted = revertProjection(s2, rhs_field_mapping_, false);

    delete s1;
    delete s2;

    for(int i = 0; i < max_intermediate_cardinality_; i++) {
        B dummy_tag = lhs_reverted->getDummyTag(i) | rhs_reverted->getDummyTag(i);
        this->output_->cloneRow(!dummy_tag, i, 0, lhs_reverted, i);
        this->output_->cloneRow(!dummy_tag, i, lhs_field_cnt, rhs_reverted, i);
        this->output_->setDummyTag(i, dummy_tag);
    }

    delete lhs_reverted;
    delete rhs_reverted;

    lhs->pinned_ = false;
    this->output_->setSortOrder(this->sort_definition_);

	log->write("Output: " + this->output_->toString(true), Level::INFO);
    return this->output_;

}

template<typename B>
pair<QueryTable<B> *, QueryTable<B> *>  SortMergeJoin<B>::augmentTables(QueryTable<B> *lhs, QueryTable<B> *rhs) {
}

template<typename B>
QueryTable<B> *SortMergeJoin<B>::unionAndSortTables() {
}

template<typename B>
QueryTable<B> *SortMergeJoin<B>::projectJoinKeyToFirstAttr(QueryTable<B> *src, vector<int> join_cols, const int & is_lhs) {
}

template<typename B>
void SortMergeJoin<B>::initializeAlphas(QueryTable<B> *dst) {
}

template<typename B>
QueryTable<B> *SortMergeJoin<B>::obliviousDistribute(QueryTable<B> *input, size_t target_size) {
}

template<typename B>
QueryTable<B> *SortMergeJoin<B>::obliviousExpand(QueryTable<B> *input, bool is_lhs) {
}

template<typename B>
QueryTable<B> *SortMergeJoin<B>::revertProjection(QueryTable<B> *src, const map<int, int> &expr_map,
                                                       const bool &is_lhs) const {
}

template class vaultdb::SortMergeJoin<bool>;
template class vaultdb::SortMergeJoin<emp::Bit>;
