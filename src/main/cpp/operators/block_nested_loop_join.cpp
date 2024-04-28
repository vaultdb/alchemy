#include "block_nested_loop_join.h"
#include "query_table/query_table.h"
#include "util/system_configuration.h"
#include "query_table/packed_column_table.h"
#include "util/buffer_pool/buffer_pool_manager.h"
#include "cmath"

using namespace vaultdb;

template<typename B>
BlockNestedLoopJoin<B>::BlockNestedLoopJoin(Operator<B> *lhs, Operator<B> *rhs, Expression<B> *predicate,
                                           const SortDefinition &sort)
        : Join<B>(lhs, rhs, predicate, sort) {
            this->output_cardinality_ = lhs->getOutputCardinality() * rhs->getOutputCardinality();
            this->updateCollation();
}

template<typename B>
BlockNestedLoopJoin<B>::BlockNestedLoopJoin(QueryTable<B> *lhs, QueryTable<B> *rhs, Expression<B> *predicate,
                                           const SortDefinition &sort)
        : Join<B>(lhs, rhs, predicate, sort) {
            this->output_cardinality_ = lhs->tuple_cnt_ * rhs->tuple_cnt_;

    this->updateCollation();
}

template<typename B>
QueryTable<B> *BlockNestedLoopJoin<B>::runSelf() {
    SystemConfiguration &sys_conf = SystemConfiguration::getInstance();
    assert(sys_conf.storageModel() == StorageModel::PACKED_COLUMN_STORE);
    assert(sys_conf.bp_enabled_);

    BufferPoolManager *bpm = sys_conf.bpm_;

    PackedColumnTable *lhs = (PackedColumnTable *) Operator<B>::getChild(0)->getOutput();
    lhs->pinned_ = true;
    PackedColumnTable *rhs = (PackedColumnTable *) Operator<B>::getChild(1)->getOutput();

    this->start_time_ = clock_start();
    this->start_gate_cnt_ = this->system_conf_.andGateCount();

    B selected, dst_dummy_tag, lhs_dummy_tag;
    this->output_ = QueryTable<B>::getTable(lhs->tuple_cnt_ * rhs->tuple_cnt_, this->output_schema_,
                                           this->sort_definition_);

    int cursor = 0;
    int rhs_col_offset = this->output_->getSchema().getFieldCount() - rhs->getSchema().getFieldCount();

    // calculate the largest fields per wire
    int outer_largest_fields_per_wire = 0;
    int inner_largest_fields_per_wire = 0;

    for(int i = 0; i < lhs->getSchema().getFieldCount(); ++i) {
        if(lhs->fields_per_wire_[i] > outer_largest_fields_per_wire) {
            outer_largest_fields_per_wire = lhs->fields_per_wire_[i];
        }
    }

    for(int i = 0; i < rhs->getSchema().getFieldCount(); ++i) {
        if(rhs->fields_per_wire_[i] > inner_largest_fields_per_wire) {
            inner_largest_fields_per_wire = rhs->fields_per_wire_[i];
        }
    }

    // calculate the size of inner/outer block
    int outer_block_fields_size = INT_MAX;
    int inner_block_fields_size = INT_MAX;

    for(int i = 0; i < lhs->getSchema().getFieldCount(); ++i){
        int fields_size_to_match_largest_fields = (outer_largest_fields_per_wire / lhs->fields_per_wire_[i]) * lhs->fields_per_wire_[i];

        if(fields_size_to_match_largest_fields < outer_block_fields_size) {
            outer_block_fields_size = fields_size_to_match_largest_fields;
        }
    }

    for(int i = 0; i < rhs->getSchema().getFieldCount(); ++i){
        int fields_size_to_match_largest_fields = (inner_largest_fields_per_wire / rhs->fields_per_wire_[i]) * rhs->fields_per_wire_[i];

        if(fields_size_to_match_largest_fields < inner_block_fields_size) {
            inner_block_fields_size = fields_size_to_match_largest_fields;
        }
    }

    // calculate the number of pages
    vector<int> outer_block_pages;
    vector<int> inner_block_pages;

    for(int i = 0; i < lhs->getSchema().getFieldCount(); ++i){
        int page_cnts = (outer_block_fields_size > lhs->fields_per_wire_[i]) ? (outer_block_fields_size / lhs->fields_per_wire_[i]) : 1;
        outer_block_pages.push_back(page_cnts);
    }

    for(int i = 0; i < rhs->getSchema().getFieldCount(); ++i){
        int page_cnts = (inner_block_fields_size > rhs->fields_per_wire_[i]) ? (inner_block_fields_size / rhs->fields_per_wire_[i]) : 1;
        inner_block_pages.push_back(page_cnts);
    }

    // block nested loop join
    vector<int> outer_col_page_offsets(outer_block_pages.size(), 0);
    vector<int> inner_col_page_offsets(inner_block_pages.size(), 0);

    for(int i = 0; i < lhs->tuple_cnt_; i += outer_block_fields_size) {
        std::vector<BufferPoolManager::PageId> pinned_pages;
        // get unpacked pages for each col and pin those pages
        for(int outer_col_idx = 0; outer_col_idx < lhs->getSchema().getFieldCount(); ++outer_col_idx) {
            int outer_page_cnts = outer_block_pages[outer_col_idx];
            int max_page = ceil(static_cast<double>(lhs->tuple_cnt_) / lhs->fields_per_wire_[outer_col_idx]);

            for(int page_offset = 0; page_offset < outer_page_cnts; ++page_offset) {
                // check if the page is valid (up to maximum number of pages)
                if(outer_col_page_offsets[outer_col_idx] + page_offset < max_page) {
                    BufferPoolManager::PageId pid = bpm->getPageId(lhs->table_id_, outer_col_idx,
                                                                   (outer_col_page_offsets[outer_col_idx] + page_offset) *
                                                                   lhs->fields_per_wire_[outer_col_idx],
                                                                   lhs->fields_per_wire_[outer_col_idx]);

                    emp::Bit *unpacked_page_ptr = bpm->getUnpackedPagePtr(pid);
                    bpm->page_status_[pid] = {true, bpm->page_status_[pid][1]};
                    pinned_pages.push_back(pid);
                }
            }

            outer_col_page_offsets[outer_col_idx] += outer_page_cnts;
        }

        if(i + outer_block_fields_size > lhs->tuple_cnt_) {
            outer_block_fields_size = lhs->tuple_cnt_ - i;
        }

        for(int j = 0; j < rhs->tuple_cnt_; j += inner_block_fields_size) {
            if(j + inner_block_fields_size > rhs->tuple_cnt_) {
                inner_block_fields_size = rhs->tuple_cnt_ - j;
            }

            // join for each block
            for(int outer_block_idx = 0; outer_block_idx < outer_block_fields_size; ++outer_block_idx) {
                lhs_dummy_tag = ((QueryTable<B> *) lhs)->getDummyTag(i + outer_block_idx);

                for(int inner_block_idx = 0; inner_block_idx < inner_block_fields_size; ++inner_block_idx) {
                    this->output_->cloneRow(cursor, 0, (QueryTable<B> *) lhs, i + outer_block_idx);
                    this->output_->cloneRow(cursor, rhs_col_offset, (QueryTable<B> *) rhs, j + inner_block_idx);
                    selected = Join<B>::predicate_->call((QueryTable<B> *) lhs, i + outer_block_idx, (QueryTable<B> *) rhs, j + inner_block_idx).template getValue<B>();
                    dst_dummy_tag = (!selected) | lhs_dummy_tag | ((QueryTable<B> *) rhs)->getDummyTag(j + inner_block_idx);
                    Operator<B>::output_->setDummyTag(cursor, dst_dummy_tag);
                    ++cursor;
                }
            }
        }

        // unpin page
        for(int i = 0; i < pinned_pages.size(); ++i) {
            bpm->page_status_[pinned_pages[i]] = {false, bpm->page_status_[pinned_pages[i]][1]};
        }
    }

    lhs->pinned_ = false;

    return this->output_;
}

template class vaultdb::BlockNestedLoopJoin<bool>;
template class vaultdb::BlockNestedLoopJoin<emp::Bit>;