#include "block_nested_loop_join.h"
#include "query_table/query_table.h"
#include "util/system_configuration.h"
#include "query_table/packed_column_table.h"
#include "util/buffer_pool/buffer_pool_manager.h"
#include "cmath"
#include <util/field_utilities.h>

using namespace vaultdb;

template<typename B>
BlockNestedLoopJoin<B>::BlockNestedLoopJoin(Operator<B> *lhs, Operator<B> *rhs, const int & fkey, Expression<B> *predicate,
                                           const SortDefinition &sort)
        : Join<B>(lhs, rhs, predicate, sort), foreign_key_input_(fkey) {
            this->output_cardinality_ = lhs->getOutputCardinality() * rhs->getOutputCardinality();
            this->updateCollation();
}

template<typename B>
BlockNestedLoopJoin<B>::BlockNestedLoopJoin(QueryTable<B> *lhs, QueryTable<B> *rhs, const int & fkey, Expression<B> *predicate,
                                           const SortDefinition &sort)
        : Join<B>(lhs, rhs, predicate, sort), foreign_key_input_(fkey) {
            this->output_cardinality_ = lhs->tuple_cnt_ * rhs->tuple_cnt_;

    this->updateCollation();
}

template<typename B>
QueryTable<B> *BlockNestedLoopJoin<B>::runSelf() {
    SystemConfiguration &sys_conf = SystemConfiguration::getInstance();
    assert(sys_conf.storageModel() == StorageModel::PACKED_COLUMN_STORE);
    assert(sys_conf.bp_enabled_);

    BufferPoolManager &bpm_ = sys_conf.bpm_;

    PackedColumnTable *lhs = (PackedColumnTable *) Operator<B>::getChild(0)->getOutput();
    lhs->pinned_ = true;
    PackedColumnTable *rhs = (PackedColumnTable *) Operator<B>::getChild(1)->getOutput();

    this->start_time_ = clock_start();
    this->start_gate_cnt_ = this->system_conf_.andGateCount();

    // Larger table should be outer table
    PackedColumnTable *outer_table;
    PackedColumnTable *inner_table;

    // TODO: update this for compound join predicates using:
    //   auto p = (GenericExpression<B> *) this->predicate_;
    //    JoinEqualityConditionVisitor<B> join_visitor(p->root_);
    //    vector<pair<uint32_t, uint32_t> > join_idxs = join_visitor.getEqualities(); // lhs, rhs

    ExpressionNode<Bit> *lhs_key_node = ((GenericExpression<Bit> *) this->predicate_)->root_->lhs_;
    ExpressionNode<Bit> *rhs_key_node = ((GenericExpression<Bit> *) this->predicate_)->root_->rhs_;

    vector<int> outer_key_ordinals;
    vector<int> inner_key_ordinals;

    bool is_lhs_outer = foreign_key_input_ == 0 ? true : false;

    if(is_lhs_outer) {
        outer_table = lhs;
        inner_table = rhs;

        outer_key_ordinals.push_back(lhs_key_node->output_schema_.getOrdinal());
        inner_key_ordinals.push_back(rhs_key_node->output_schema_.getOrdinal());
    } else {
        outer_table = rhs;
        inner_table = lhs;

        outer_key_ordinals.push_back(rhs_key_node->output_schema_.getOrdinal());
        inner_key_ordinals.push_back(lhs_key_node->output_schema_.getOrdinal());
    }

    uint32_t output_tuple_cnt = outer_table->tuple_cnt_; // foreignKeyTable = foreign key
    this->output_ = QueryTable<B>::getTable(output_tuple_cnt, this->output_schema_,  this->sort_definition_);

    B selected, to_update, lhs_dummy_tag, rhs_dummy_tag, dst_dummy_tag;
    int rhs_col_offset = is_lhs_outer ? this->output_->getSchema().getFieldCount() - inner_table->getSchema().getFieldCount() : inner_table->getSchema().getFieldCount();

    // Calculate sunk costs for inner table and headrooms for outer and output tables
    // Estimate possible number of pages to pin in the unpacked buffer pool
    int inner_table_sunk_cost_pages = inner_table->getSchema().getFieldCount() + 1; // number of cols + dummy tag
    int outer_table_sunk_cost_pages = outer_key_ordinals.size() + 1; // headrooms for outer table: key col + dummy tag
    int output_table_sunk_cost_pages = inner_table_sunk_cost_pages; // headrooms for output table: number of cols in the inner table + dummy tag

    int pages_possible_to_pin = bpm_.page_cnt_ - inner_table_sunk_cost_pages - outer_table_sunk_cost_pages - output_table_sunk_cost_pages;

    // Estimate possible number of rows to pin in the unpacked buffer pool
    int row_bits = 0;

    // outer keys
    for(int i = 0; i < outer_key_ordinals.size(); ++i) {
        row_bits += outer_table->getSchema().getField(outer_key_ordinals[i]).size();
    }

    row_bits++; // outer dummy tag

    // inner cols for output
    for(int i = 0; i < inner_table->getSchema().getFieldCount(); ++i) {
        row_bits += inner_table->getSchema().getField(i).size();
    }

    row_bits++; // inner dummy tag

    int rows_possible_to_pin = std::floor(static_cast<double>(pages_possible_to_pin * bpm_.unpacked_page_size_bits_) / row_bits);

    // Calculate pages to pin for each col
//    vector<int> outer_pinned_pages;
//    for(int i = 0; i < outer_key_ordinals.size(); ++i) {
//        outer_pinned_pages.push_back(std::ceil(static_cast<double>(rows_possible_to_pin) / outer_table->fields_per_wire_[outer_key_ordinals[i]]));
//        cout << "outer_table->fields_per_wire_[outer_key_ordinals[i]]: " << outer_table->fields_per_wire_[outer_key_ordinals[i]] << endl;
//        cout << "outer_pinned_pages[i]: " << outer_pinned_pages[i] << endl;
//    }
//
//    outer_pinned_pages[outer_pinned_pages.size() - 1] = std::ceil(static_cast<double>(rows_possible_to_pin) / outer_table->fields_per_wire_[-1]);
//    cout << "outer_pinned_pages[outer_pinned_pages.size() - 1]: " << outer_pinned_pages[outer_pinned_pages.size() - 1] << endl;
//
//    vector<int> output_pinned_pages;
//    for(int i = 0; i < inner_table->getSchema().getFieldCount(); ++i) {
//        output_pinned_pages.push_back(std::ceil(static_cast<double>(rows_possible_to_pin) / inner_table->fields_per_wire_[i]));
//        cout << "inner_table->fields_per_wire_[i]: " << inner_table->fields_per_wire_[i] << endl;
//        cout << "output_pinned_pages[i]: " << output_pinned_pages[i] << endl;
//    }
//
//    output_pinned_pages[output_pinned_pages.size() - 1] = std::ceil(static_cast<double>(rows_possible_to_pin) / inner_table->fields_per_wire_[-1]);
//    cout << "output_pinned_pages[output_pinned_pages.size() - 1]: " << output_pinned_pages[output_pinned_pages.size() - 1] << endl;

    // block nested loop join
    bool loop = true;

    int outer_block_start_idx = 0;
    int outer_block_end_idx = rows_possible_to_pin;

    // outer loop
    while(loop) {
        // check if the outer block reaches the end
        if(rows_possible_to_pin > (outer_table->tuple_cnt_ - outer_block_start_idx)) {
            outer_block_end_idx = outer_table->tuple_cnt_;
            loop = false;
        }

        // pin outer pages
        vector<PageId> outer_pinned_pids;
        for(int i = 0; i < outer_key_ordinals.size(); ++i) {
            int outer_pinned_pages = std::ceil(static_cast<double>(outer_block_end_idx - outer_block_start_idx) / outer_table->fields_per_wire_[outer_key_ordinals[i]]);

            for(int j = 0; j < outer_pinned_pages; ++j) {
                PageId pid = bpm_.getPageId(outer_table->table_id_, outer_key_ordinals[i],
                                            outer_block_start_idx + j * outer_table->fields_per_wire_[outer_key_ordinals[i]],
                                            outer_table->fields_per_wire_[outer_key_ordinals[i]]);
                bpm_.loadPage(pid);
                bpm_.pinPage(pid);
                outer_pinned_pids.push_back(pid);
            }
        }

        // pin outer dummy tags
        int outer_dummy_pinned_pages = std::ceil(static_cast<double>(outer_block_end_idx - outer_block_start_idx) / outer_table->fields_per_wire_[-1]);

        for(int i = 0; i < outer_dummy_pinned_pages; ++i) {
            PageId pid = bpm_.getPageId(outer_table->table_id_, -1, outer_block_start_idx + i * outer_table->fields_per_wire_[-1], outer_table->fields_per_wire_[-1]);
            bpm_.loadPage(pid);
            bpm_.pinPage(pid);
            outer_pinned_pids.push_back(pid);
        }

        // inner loop
        for(int outer_block_idx = outer_block_start_idx; outer_block_idx < outer_block_end_idx; ++outer_block_idx) {
            if(is_lhs_outer) {
                lhs_dummy_tag = ((QueryTable<B> *) outer_table)->getField(outer_block_idx,-1).template getValue<B>();
                this->output_->cloneRow(outer_block_idx, 0, (QueryTable<B> *) outer_table, outer_block_idx);
                dst_dummy_tag = B(true);

                for(int inner_block_idx = 0; inner_block_idx < inner_table->tuple_cnt_; ++inner_block_idx) {
                    rhs_dummy_tag = ((QueryTable<B> *) inner_table)->getField(inner_block_idx, -1).template getValue<B>();
                    selected = Join<B>::predicate_->call((QueryTable<B> *) outer_table, outer_block_idx, (QueryTable<B> *) inner_table, inner_block_idx).template getValue<B>();

                    to_update = selected & (!lhs_dummy_tag) & (!rhs_dummy_tag);
                    dst_dummy_tag = FieldUtilities::select(to_update, false, dst_dummy_tag);
                    this->output_->cloneRow(to_update, outer_block_idx, rhs_col_offset, (QueryTable<B> *) inner_table, inner_block_idx);
                }

                this->output_->setDummyTag(outer_block_idx, dst_dummy_tag);
            }
            else {
                rhs_dummy_tag = outer_table->getField(outer_block_idx,-1).template getValue<B>();
                this->output_->cloneRow(outer_block_idx, rhs_col_offset, (QueryTable<B> *) outer_table, outer_block_idx);
                dst_dummy_tag = B(true);

                for(int inner_block_idx = 0; inner_block_idx < outer_table->tuple_cnt_; ++inner_block_idx) {
                    lhs_dummy_tag = inner_table->getField(inner_block_idx, -1).template getValue<B>();
                    selected = Join<B>::predicate_->call((QueryTable<B> *) inner_table, inner_block_idx, (QueryTable<B> *) outer_table, outer_block_idx).template getValue<B>();
                    to_update = selected & (!lhs_dummy_tag) & (!rhs_dummy_tag);
                    dst_dummy_tag = FieldUtilities::select(to_update, false, dst_dummy_tag);
                    this->output_->cloneRow(to_update, outer_block_idx, 0, (QueryTable<B> *) inner_table, inner_block_idx);
                }
            }
        }

        // unpin outer pages and dummy tags
        for(int i = 0; i < outer_pinned_pids.size(); ++i) {
            bpm_.unpinPage(outer_pinned_pids[i]);
        }

        // update block idxs
        outer_block_start_idx = outer_block_end_idx;
        outer_block_end_idx += rows_possible_to_pin;
    }

    return this->output_;
}

template class vaultdb::BlockNestedLoopJoin<bool>;
template class vaultdb::BlockNestedLoopJoin<emp::Bit>;