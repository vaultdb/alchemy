#include "block_nested_loop_join.h"
#include "query_table/query_table.h"
#include "util/system_configuration.h"
#include "query_table/packed_column_table.h"
#include "util/buffer_pool/buffer_pool_manager.h"
#include "expression/visitor/join_equality_condition_visitor.h"
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

    BufferPoolManager &bpm = sys_conf.bpm_;

    auto lhs = (PackedColumnTable *) this->getChild(0)->getOutput();
    auto rhs = (PackedColumnTable *) this->getChild(1)->getOutput();

    this->start_time_ = clock_start();
    this->start_gate_cnt_ = this->system_conf_.andGateCount();

    auto p = (GenericExpression<B> *) this->predicate_;
    JoinEqualityConditionVisitor<B> join_visitor(p->root_);
    vector<pair<uint32_t, uint32_t> > join_idxs = join_visitor.getEqualities(); // lhs, rhs

    auto fk_reln = (foreign_key_input_ == 0) ? lhs : rhs;
    auto pk_reln = (foreign_key_input_ == 0) ? rhs : lhs;
    cout << "***FK (outer) relation has table ID: " << fk_reln->table_id_ << '\n';
    cout << "***PK (inner) relation has table ID: " << pk_reln->table_id_ << '\n';
    fk_reln->pinned_ = true;
    auto output_schema = this->output_schema_;
    auto pk_schema = pk_reln->getSchema();
    auto fk_schema = fk_reln->getSchema();
    int pk_offset = (foreign_key_input_ == 0) ? fk_schema.getFieldCount() : 0;
    int fk_offset = (foreign_key_input_ == 0) ? 0 : pk_schema.getFieldCount();

    // get sunk costs
    int pk_sunk_costs = pk_reln->getSchema().getFieldCount() + 1; // number of cols + dummy tag
    int pinnable_pages = bpm.page_cnt_ - pk_sunk_costs;

    this->output_ = QueryTable<B>::getTable(fk_reln->tuple_cnt_, this->output_schema_,  this->sort_definition_);
    auto output = (PackedColumnTable *) this->output_;

    // initialize FK rows in output table
    for(int i = 0; i < fk_schema.getFieldCount(); ++i) {
        output->cloneColumn(i + fk_offset, fk_reln, i);
    }

    vector<int> ordinals_to_pin;

    // in output schema - using the dst schema as a stand-in for FK ones
    //  this won't create duplicate entries because FK is mutually exclusive with PK
    for(auto idx : join_idxs) {
        int ord = (foreign_key_input_ == 0) ? idx.first : idx.second;
        ordinals_to_pin.push_back(ord);
        assert(output->wires_per_field_[ord] == 1); // does not handle large objects that span > 1 wire
    }

    for(int i = 0; i < pk_schema.getFieldCount(); ++i) {
        ordinals_to_pin.push_back(pk_offset + i);
        assert(output->wires_per_field_[pk_offset + i] == 1); // does not handle large objects that span > 1 wire
    }


    // find largest column in variable costs (not inner scan) - this will determine our stride size
    int stride_size = 63336; // min_fields_per_page, just initializing it to something high to get started
    int row_width_bits = 2; // dummy tags for FK and output relations

    // both sides of a join equality have the same size
    // ignore the dummy tag for max because all fields have a size >= this
    for(auto ord : ordinals_to_pin) {
        int fields_per_page = bpm.unpacked_page_size_bits_ / output_schema.getField(ord).size();
        if(fields_per_page < stride_size) stride_size = fields_per_page;
        row_width_bits += output_schema.getField(ord).size();
    }

    assert(stride_size > 0);

    // if a field's count / page is not evenly divisible by our stride side (it has no common multiple), allocate one page of headroom
    for(auto ord : ordinals_to_pin) {
        int fields_per_page = bpm.unpacked_page_size_bits_ / output_schema.getField(ord).size();
        if(fields_per_page % stride_size > 0)  {
            --pinnable_pages;
        }
    }

    // if we can pin greedily
    long rows_per_block = (pinnable_pages * bpm.unpacked_page_size_bits_) / row_width_bits;
    // make it align with strides
    rows_per_block -= rows_per_block % stride_size;
    cout << "Starting with rows per block " << rows_per_block << '\n';
    // dummy tags
    int dummy_tag_pages_per_block = rows_per_block / output->fields_per_wire_[-1] + (rows_per_block % output->fields_per_wire_[-1] > 0);
    int pages_to_pin = dummy_tag_pages_per_block * 2; // two dummy tags, FK and output

    // count up for ordinals
    for(auto ord : ordinals_to_pin) {
        pages_to_pin += rows_per_block / output->fields_per_wire_[ord] + (rows_per_block % output->fields_per_wire_[ord] > 0);
    }

    // step down one stride if we have too many pages
    while(pages_to_pin > pinnable_pages) {
        rows_per_block -= stride_size;
        dummy_tag_pages_per_block = rows_per_block / output->fields_per_wire_[-1] + (rows_per_block % output->fields_per_wire_[-1] > 0);
        pages_to_pin = dummy_tag_pages_per_block * 2;
        for(auto ord : ordinals_to_pin) {
            pages_to_pin += rows_per_block / output->fields_per_wire_[ord] + (rows_per_block % output->fields_per_wire_[ord] > 0);
        }
    }

    cout << "Final rows per block: " << rows_per_block << '\n';
    cout << "BNLJ blocks: " << (output->tuple_cnt_ / rows_per_block) + (output->tuple_cnt_ % rows_per_block > 0) << endl;

    cout << "Starting with BPM stats: " << bpm.stats();
    int cursor = 0; // first row idx in block
    while((cursor + rows_per_block) < output->tuple_cnt_) {
        for(auto entry : join_idxs) {
            pinRowRange(fk_reln, (foreign_key_input_ == 0) ? entry.first : (entry.second - pk_offset),
                        cursor, rows_per_block);
        }

        // pin dummy tags
        pinRowRange(fk_reln, -1, cursor, rows_per_block);

        for(int i = -1; i < pk_schema.getFieldCount(); ++i) {
            pinRowRange(output, i + pk_offset, cursor, rows_per_block);
        }

        // iterate through and join on all rows in pinned range
        joinRowRange((QueryTable<B> *) fk_reln, (QueryTable<B> *)  pk_reln, cursor, rows_per_block);

        // unpin this block's pages
        for(auto entry : join_idxs) {
            unpinRowRange(fk_reln, (foreign_key_input_ == 0) ? entry.first : (entry.second - pk_offset),
                        cursor, rows_per_block);
        }
        // unpin dummy tags
        unpinRowRange(fk_reln, -1, cursor, rows_per_block);

        for(int i = -1; i < pk_schema.getFieldCount(); ++i) {
            unpinRowRange(output, i + pk_offset, cursor, rows_per_block);
        }


        cout << "At the end of block " << cursor / rows_per_block << " BPM stats: " << bpm.stats();
        cursor += rows_per_block;
//        if(cursor / rows_per_block  == 1) exit(-1);
    }

    // handle any leftovers here
    int rows_remaining = output->tuple_cnt_ - cursor;
    // pin just in case inner relation is very large

    for(auto entry : join_idxs) {
        pinRowRange(fk_reln, (foreign_key_input_ == 0) ? entry.first : (entry.second - pk_offset),
                    cursor, rows_remaining);
    }
    // pin dummy tags too
    pinRowRange(fk_reln, -1, cursor, rows_remaining);

    for(int i = -1; i < pk_schema.getFieldCount(); ++i) {
        pinRowRange(output, i + pk_offset, cursor, rows_remaining);
    }


    cout << "After pinning block pages have bpm stats " << bpm.stats();

    joinRowRange((QueryTable<B> *) fk_reln, (QueryTable<B> *)  pk_reln, cursor, rows_remaining);
    cout << "After joining rows have bpm stats " << bpm.stats();

    for(auto entry : join_idxs) {
        unpinRowRange(fk_reln, (foreign_key_input_ == 0) ? entry.first : (entry.second - pk_offset),
                    cursor, rows_remaining);
    }
    unpinRowRange(fk_reln, -1, cursor, rows_remaining);

    for(int i = -1; i < pk_schema.getFieldCount(); ++i) {
        unpinRowRange(output, i + pk_offset, cursor, rows_remaining);
    }

    cout << "Finishing BNLJ with BPM stats: " << bpm.stats() << '\n';
    fk_reln->pinned_ = false;
    return this->output_;

}




template<typename B>
void BlockNestedLoopJoin<B>::joinRowRange(QueryTable<B> *fk_reln, QueryTable<B> *pk_reln, int start_row, int row_cnt) {
    int cutoff = start_row + row_cnt;
    int pk_offset = (foreign_key_input_ == 0) ? fk_reln->getSchema().getFieldCount() : 0;
    int start_pinned_page_cnt = bpm_.pinnedPageCount();
    cout << "***Joining block, starting with " << start_pinned_page_cnt << " pinned pages!\n";

    // each column in inner loop pins the page it is working on to prevent other columns from evicting it
    // incrementally "shift" pin as we work through the rows
    for(int i = -1; i < pk_reln->getSchema().getFieldCount(); ++i) {
        auto pid = PageId{((PackedColumnTable *) pk_reln)->table_id_, i, 0};
        bpm_.loadAndPinPage(pid);
    }


    for(int j = 0; j < pk_reln->tuple_cnt_; ++j) {
        B pk_dummy_tag = pk_reln->getField(j, -1).template getValue<B>();
        B dst_dummy_tag = true; // dummy by default, no matches found yet

        // If we reach the end of a page on one column, shift its pin over to next page in this column
        // this "shielded scan" protects us from the vagaries of our eviction strategy
        for(int k = -1; k < pk_reln->getSchema().getFieldCount(); ++k) {
            if(j % ((PackedColumnTable *)pk_reln)->fields_per_wire_[k] == 0 && j > 0) {
                auto pid = PageId{((PackedColumnTable *) pk_reln)->table_id_, k, (j-1) / ((PackedColumnTable *)pk_reln)->fields_per_wire_[k]};
                bpm_.unpinPage(pid);
                ++pid.page_idx_;
                bpm_.loadAndPinPage(pid);
            }
        }
        // compare j'th PK row to all in block
        for(int i = start_row; i < cutoff; ++i) {
            B fk_dummy_tag = fk_reln->getField(i, -1).template getValue<B>();
            B selected = (foreign_key_input_ == 0)
                         ? Join<B>::predicate_->call(fk_reln, i, pk_reln, j).template getValue<B>()
                         : Join<B>::predicate_->call(pk_reln, j, fk_reln, i).template getValue<B>();

            B to_update = selected & (!fk_dummy_tag) & (!pk_dummy_tag);
            dst_dummy_tag = FieldUtilities::select(to_update, false, dst_dummy_tag);
            this->output_->cloneRow(to_update, i, pk_offset, pk_reln, j);
        }
    }

    // unpin all last rows in PK
    int last_idx = pk_reln->tuple_cnt_ - 1;
    for(int i = -1; i < pk_reln->getSchema().getFieldCount(); ++i) {
        int last_page_idx = last_idx / ((PackedColumnTable *) pk_reln)->fields_per_wire_[i];
        auto pid = PageId{((PackedColumnTable *) pk_reln)->table_id_, i, last_page_idx};
        bpm_.unpinPage(pid);
    }

    int end_pinned_page_cnt = bpm_.pinnedPageCount();
    cout << "Start pinned page count: " << start_pinned_page_cnt << " end: " << end_pinned_page_cnt << '\n';
    assert(start_pinned_page_cnt == end_pinned_page_cnt);


}

template<typename B>
void BlockNestedLoopJoin<B>::pinRowRange(PackedColumnTable *table, const int & col_ordinal, const int & start_row, const int & row_cnt) {
    PageId start_pid = bpm_.getPageId(table->table_id_, col_ordinal, start_row, table->fields_per_wire_[col_ordinal]);
    PageId end_pid = bpm_.getPageId(table->table_id_, col_ordinal, start_row + row_cnt - 1, table->fields_per_wire_[col_ordinal]);
    cout << "Pinning page range: " << start_pid << "..." << end_pid << '\n';

    auto pid = start_pid;
    bpm_.loadAndPinPage(pid);

    while(pid.page_idx_ < end_pid.page_idx_) {
        ++pid.page_idx_;
        bpm_.loadAndPinPage(pid);
    }
}

template<typename B>
void BlockNestedLoopJoin<B>::unpinRowRange(PackedColumnTable *table, const int & col_ordinal, const int & start_row, const int & row_cnt) {
    PageId start_pid = bpm_.getPageId(table->table_id_, col_ordinal, start_row, table->fields_per_wire_[col_ordinal]);
    PageId end_pid = bpm_.getPageId(table->table_id_, col_ordinal, start_row + row_cnt - 1, table->fields_per_wire_[col_ordinal]);

    auto pid = start_pid;
    bpm_.unpinPage(pid);
    cout << "Unpinning page range: " << start_pid << "..." << end_pid << '\n';

    while(pid.page_idx_ < end_pid.page_idx_) {
        ++pid.page_idx_;
        bpm_.unpinPage(pid);
    }
}

template class vaultdb::BlockNestedLoopJoin<bool>;
template class vaultdb::BlockNestedLoopJoin<emp::Bit>;