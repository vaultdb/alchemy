#include "buffer_pool_manager.h"
#include <query_table/packed_column_table.h>

using namespace  vaultdb;

#if  __has_include("emp-sh2pc/emp-sh2pc.h") || __has_include("emp-zk/emp-zk.h")
#include "emp-sh2pc/emp-sh2pc.h"
#else
void BufferPoolManager::loadPage(PageId &pid) {

    if(position_map_.find(pid) != position_map_.end()) {
        ++hits_;
        return;
    }

    OMPCPackedWire src = packed_table_catalog_[pid.table_id_]->readPackedWire(pid);
    int target_slot = evictPage();
    ++misses_;
    Bit *dst = unpacked_buffer_pool_.data() + target_slot * unpacked_page_size_bits_;
    emp_manager_->unpack((Bit *) &src, dst, unpacked_page_size_bits_);

    PositionMapEntry p(target_slot, false, false);
    position_map_[pid] = p;
    reverse_position_map_[target_slot] = pid;

}


// greedily evict first unpinned page in the queue
// returns the newly-opened slot in the buffer pool
int BufferPoolManager::evictPage() {


    PositionMapEntry pos;
    //  uninitialized slot, this should only happen when we are warming up the buffer pool
    if(reverse_position_map_.find(clock_hand_position_) == reverse_position_map_.end())  {
        int slot = clock_hand_position_;
        clock_hand_position_ = (clock_hand_position_ + 1) % page_cnt_;
//        cout << "EvictPage initializing slot " << slot << endl;
        return slot;
    };

    int clock_hand_starting_pos = clock_hand_position_;

    PageId pid = reverse_position_map_[clock_hand_position_];
    pos = position_map_.at(pid);

//    cout << "Evicting page " << pid << " at slot " << pos.slot_id_ << endl;
    // first unpinned page
    while(pos.pinned_) {
        clock_hand_position_ = (clock_hand_position_ + 1) % page_cnt_;
        pid = reverse_position_map_[clock_hand_position_];
        pos = position_map_.at(pid);
        if(clock_hand_position_ == clock_hand_starting_pos) {
            // if we have gone through the whole buffer pool and all pages are pinned, then we have a problem
            // we should never have all pages pinned
            throw std::runtime_error("Buffer pool has no unpinned pages!");
        }
    }


    if (pos.dirty_) {
        emp::Bit *src_ptr =  unpacked_buffer_pool_.data() + position_map_.at(pid).slot_id_ * unpacked_page_size_bits_;
        OMPCPackedWire dst(block_n_);
        emp_manager_->pack(src_ptr, (Bit *) &dst, unpacked_page_size_bits_);
        packed_table_catalog_[pid.table_id_]->writePackedWire(pid, dst);
    }

    // remove it from the position map even if it is not dirty
    position_map_.erase(pid);
    reverse_position_map_.erase(pos.slot_id_);

    // increment clock hand one more time for next round
    clock_hand_position_ = (clock_hand_position_ + 1) % page_cnt_;
    return pos.slot_id_;
}

// makes a deep copy of src_pid at dst_pid
void BufferPoolManager::clonePage(PageId &src_pid, PageId &dst_pid) {
    // if src page is unpacked, copy it to dst page in buffer pool
    // mark dst_page as dirty
    if(position_map_.find(src_pid) != position_map_.end()
       && position_map_.at(src_pid).dirty_) {
        //cout << "**Cloning in-memory page from slot " << position_map_[src_pid].slot_id_ << " for " << src_pid <<  '\n';
        emp::Bit *src_page = unpacked_buffer_pool_.data() + position_map_.at(src_pid).slot_id_ * unpacked_page_size_bits_;
        loadPage(dst_pid); // make sure dst page is loaded (if not already in buffer pool
        emp::Bit *dst_page = unpacked_buffer_pool_.data() + position_map_.at(dst_pid).slot_id_ * unpacked_page_size_bits_;
        //cout << "Cloning to slot " << position_map_[dst_pid].slot_id_ << " for " << dst_pid <<  '\n';
        memcpy(dst_page, src_page, unpacked_page_size_bits_);
        position_map_.at(dst_pid).dirty_ = true;
    }
    else {
        PackedColumnTable *src_table = packed_table_catalog_[src_pid.table_id_];
        PackedColumnTable *dst_table = packed_table_catalog_[dst_pid.table_id_];

        int8_t *src_page_ptr = src_table->packed_pages_[src_pid.col_id_].data() + src_pid.page_idx_ * src_table->packed_wire_size_bytes_;
        int8_t *dst_page_ptr = dst_table->packed_pages_[dst_pid.col_id_].data() + dst_pid.page_idx_ * dst_table->packed_wire_size_bytes_;
        memcpy(dst_page_ptr, src_page_ptr, src_table->packed_wire_size_bytes_);
    }

}


void BufferPoolManager::flushPage(const PageId &pid) {
    if(position_map_.find(pid) != position_map_.end() && position_map_.at(pid).dirty_) {

        emp::Bit *src_ptr =  unpacked_buffer_pool_.data() + position_map_.at(pid).slot_id_ * unpacked_page_size_bits_;
        OMPCPackedWire dst(block_n_);
        emp_manager_->pack(src_ptr, (Bit *) &dst, unpacked_page_size_bits_);
        packed_table_catalog_[pid.table_id_]->writePackedWire(pid, dst);
        position_map_.at(pid).dirty_ = false;
    }
}

#endif