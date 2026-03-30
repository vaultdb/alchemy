#include "buffer_pool_manager.h"
#include <query_table/buffered_column_table.h>
#include "util/utilities.h"

using namespace  vaultdb;

template<typename B>
int BufferPoolManager::getFreeFrame() {

    // if there's an unused slot, return it
    if (!free_frames_.empty()) {
        int slot = free_frames_.front();
        free_frames_.pop_front();
        return slot;
    }

    auto dst_frame = lru_k_replacer_->evict();

    auto pos = reverse_position_map_.at(dst_frame);
    // cout << "Evicting page " << pos.toString() << "from slot " << dst_frame << '\n';


    if (dst_frame == -1) {
        throw std::runtime_error("Buffer pool is full with pinned pages, cannot evict any pages");
    }


    flushPage<B>(pos);
    position_map_.erase(pos);
    reverse_position_map_.erase(dst_frame);

    return dst_frame;
}

// deep copy of a page from src_pid to dst_pid
template<typename B>
void BufferPoolManager::clonePage(PageId &dst_pid, PageId &src_pid) {
    // if src page is already in cache, copy it to dst page in buffer pool
    // mark dst_page as dirty
    // ordinarily wouldn't allow copy of dirty writes, but b/c this is a read-only workload, we allow it
    loadPage<B>(src_pid);
    int src_slot = position_map_.at(src_pid).slot_id_;
    // make sure it can't get evicted while we are copying it
    pinPage(src_pid);

    loadPage<B>(dst_pid);
    int dst_slot = position_map_.at(dst_pid).slot_id_;

    auto src_page = (buffer_pool_.data() +  src_slot * page_size_bytes_);
    auto dst_page = (buffer_pool_.data() + dst_slot * page_size_bytes_);


    memcpy(dst_page, src_page, page_size_bytes_);


    position_map_.at(dst_pid).dirty_ = true;

    unpinPage(src_pid);
}

template void BufferPoolManager::clonePage<bool>(PageId &src_pid, PageId &dst_pid);
template void BufferPoolManager::clonePage<Bit>(PageId &src_pid, PageId &dst_pid);


template<typename B>
void BufferPoolManager::loadPage(PageId &pid) {
    auto pair = position_map_.find(pid);
    if(pair != position_map_.end()) {
        ++hits_;
        frame_id_t slot = pair->second.slot_id_;
        lru_k_replacer_->recordAccess(slot);
        return;
    }


    int dst_slot = getFreeFrame<B>();
    ++misses_;
    getPage<B>(pid, dst_slot);

    // cout << "    Loaded page " << pid.toString() << " into slot " << dst_slot << " starts with: " << DataUtilities::printByteArray(buffer_pool_.data() + position_map_.at(pid).slot_id_ * page_size_bytes_, 8) << '\n';

}

template void BufferPoolManager::loadPage<bool>(PageId &pid);
template void BufferPoolManager::loadPage<Bit>(PageId &pid);

template<typename B>
void BufferPoolManager::flushPage(const PageId &pid) {
    if(position_map_.find(pid) != position_map_.end() && position_map_.at(pid).dirty_) {
        int slot = position_map_.at(pid).slot_id_;
        auto src =  (buffer_pool_.data() + slot * page_size_bytes_);

        auto tbl = (QueryTable<B> *)  table_catalog_.at(pid.table_id_);
        assert(tbl != nullptr);

        // cout << "Flushing page " << pid.toString() << " from slot " << slot << " starts with: " << DataUtilities::printByteArray(src, 8) << '\n';
        tbl->flushPage(pid, src);
        position_map_.at(pid).dirty_ = false;

    }
}

template void BufferPoolManager::flushPage<bool>(const PageId &pid);
template void BufferPoolManager::flushPage<Bit>(const PageId &pid);


template<typename B>
void BufferPoolManager::getPage(const PageId &pid, const int &slot) {
    assert(table_catalog_.find(pid.table_id_) != table_catalog_.end());

    int8_t *dst =  buffer_pool_.data() + slot * page_size_bytes_;

    auto tbl = (QueryTable<B> *) table_catalog_.at(pid.table_id_);
    assert(tbl != nullptr);

    tbl->getPage(pid, dst);
    position_map_[pid] = PositionMapEntry(slot, false, false);
    reverse_position_map_[slot] = pid;
    lru_k_replacer_->remove(slot);
    lru_k_replacer_->recordAccess(slot);
}


template void BufferPoolManager::getPage<bool>(const PageId &pid, const int & slot);
template void BufferPoolManager::getPage<Bit>(const PageId &pid, const int & slot);
