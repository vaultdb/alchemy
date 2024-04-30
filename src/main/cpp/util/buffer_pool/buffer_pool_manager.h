#ifndef _BUFFER_POOL_MANAGER_H_
#define _BUFFER_POOL_MANAGER_H_

#include "emp-tool/emp-tool.h"
#include "common/defs.h"
#include "util/emp_manager/emp_manager.h"

using namespace std;

#if  __has_include("emp-sh2pc/emp-sh2pc.h") || __has_include("emp-zk/zk.h")
namespace vaultdb {
    class BufferPoolManager {
    public:
        typedef struct page_id_ {
            int table_id_;
            int col_id_;
            int page_idx_;

            bool operator==(const page_id_ &other) const {
                return table_id_ == other.table_id_ && col_id_ == other.col_id_ && page_idx_ == other.page_idx_;
            }

            bool operator<(const page_id_ &o)  const {
                if(table_id_ < o.table_id_) {
                    return true;
                }
                else if(table_id_ == o.table_id_) {
                    if(col_id_ < o.col_id_) {
                        return true;
                    }
                    else if(col_id_ == o.col_id_) {
                        return page_idx_ < o.page_idx_;
                    }
                }
                return false;
            }

            string toString() const {
                return "(" + to_string(table_id_) + ", " + to_string(col_id_) + ", " + to_string(page_idx_) + ")";
            }

        } PageId;

        // data from same column can be same page - easy to get offset.
        typedef struct unpackd_page_ {
            PageId pid_;
            vector<emp::Bit> page_payload_;
            int access_counters_; // for LRU
            bool pinned_ = false;
        } UnpackedPage;

        typedef struct packd_page_ {
            PageId pid_;
            emp::OMPCPackedWire page_payload_;
            int access_counters_; // for LRU
            bool pinned_ = false;
        } PackedPage;

        BufferPoolManager() {};

        BufferPoolManager(int unpacked_page_size, int num_unpacked_pages, int packed_page_size, int num_packed_pages, EmpManager *manager) : unpacked_page_size_(unpacked_page_size), max_unpacked_pages_(num_unpacked_pages), packed_page_size_(packed_page_size), max_packed_pages_(num_packed_pages), emp_manager_(manager) {
            
        }

        ~BufferPoolManager() {};

        PageId getPageId(int table_id, int col, int row, int rows_per_page) {
            return {0, 0, 0};
        }


        void insertPageIdMap(const PageId &page_id) {
            
        }

        bool hasUnpackedPage(PageId &page_id) {
            return false;
        }

        bool hasPackedPage(PageId &page_id) {
            return false;
        }

        bool hasPage(PageId &page_id) {
            return false;
        }

        bool isUnpackedBufferPoolFull() {
            return false;
        }

        bool isPackedBufferPoolFull() {
            return false;
        }

        void removePageFromUnpackedBufferPool(PageId &pid) {

        }

        int flushUnpackedPage() {
            return -1;
        }

        int getEmptySlot() {
            return -1;
        }

        emp::Bit *getUnpackedPagePtr(PageId &pid) {
            return nullptr;
        }

        void clonePage(PageId &src_pid, PageId &dst_pid) {}

        void initializeColumnPages(int table_id, int col_idx, int tuple_cnt, int rows_per_page) {}

        void removeConsecutivePages(int table_id, int col_idx, int start_row_idx, int pages_to_remove, int rows_per_page) {}

        void addConsecutivePages(int table_id, int col_idx, int start_row_idx, int pages_to_add, int rows_per_page) {}

        void removeUnpackedPagesByTable(int table_id) {}


        EmpManager *emp_manager_ = nullptr;

        int unpacked_page_size_; // in emp::Bits
        int max_unpacked_pages_;
        int packed_page_size_; // in emp::OMPCPackedWire
        int max_packed_pages_;

        int block_n_;

        std::map<PageId, UnpackedPage> unpacked_page_buffer_pool_;
        std::map<PageId, PackedPage> packed_page_buffer_pool_;

    };
}
#else

#include "emp-rescu/emp-rescu.h"
#define __OMPC_BACKEND__ 1
namespace vaultdb {
    class BufferPoolManager {
    public:
        typedef struct page_id_ {
            int table_id_;
            int col_id_;
            int page_idx_;

            bool operator==(const page_id_ &other) const {
                return table_id_ == other.table_id_ && col_id_ == other.col_id_ && page_idx_ == other.page_idx_;
            }

            bool operator<(const page_id_ &o)  const {
                if(table_id_ < o.table_id_) {
                    return true;
                }
                else if(table_id_ == o.table_id_) {
                    if(col_id_ < o.col_id_) {
                        return true;
                    }
                    else if(col_id_ == o.col_id_) {
                        return page_idx_ < o.page_idx_;
                    }
                }
                return false;
            }

            string toString() const {
                return "(" + to_string(table_id_) + ", " + to_string(col_id_) + ", " + to_string(page_idx_) + ")";
            }

        } PageId;

        // data from same column can be same page - easy to get offset.
        typedef struct unpackd_page_ {
            PageId pid_;
            vector<emp::Bit> page_payload_;
            int access_counters_; // for LRU
            bool pinned_ = false;
        } UnpackedPage;

        typedef struct packd_page_ {
            PageId pid_;
            emp::OMPCPackedWire page_payload_;
            int access_counters_; // for LRU
            bool pinned_ = false;
        } PackedPage;

        BufferPoolManager() {};

        BufferPoolManager(int unpacked_page_size, int num_unpacked_pages, int packed_page_size, int num_packed_pages, EmpManager *manager) : unpacked_page_size_(unpacked_page_size), max_unpacked_pages_(num_unpacked_pages), packed_page_size_(packed_page_size), max_packed_pages_(num_packed_pages), emp_manager_(manager) {
            // block size of each packed wire based on unpacked page size
            block_n_ = unpacked_page_size_ / 128 + (unpacked_page_size_ % 128 != 0);

            // initialize unpacked page buffer
            unpacked_buffer_pool_ = std::vector<emp::Bit>(unpacked_page_size_ * max_unpacked_pages_, emp::Bit(0));
            occupied_status_ = std::vector<bool>(max_unpacked_pages_, false);
        }

        ~BufferPoolManager() {};

        static inline PageId getPageId(int table_id, int col, int row, int rows_per_page) {
            return {table_id, col, row / rows_per_page};
        }


        bool hasUnpackedPage(PageId &page_id) {
            //return unpacked_page_buffer_pool_.find(page_id) != unpacked_page_buffer_pool_.end();
            return unpacked_page_slots_.find(page_id) != unpacked_page_slots_.end();
        }

        bool hasPackedPage(PageId &page_id) {
            return packed_page_buffer_pool_.find(page_id) != packed_page_buffer_pool_.end();
        }

        bool hasPage(PageId &page_id) {
            return hasUnpackedPage(page_id) || hasPackedPage(page_id);
        }

        bool isUnpackedBufferPoolFull() const {
            //return unpacked_page_buffer_pool_.size() >= max_unpacked_pages_;
            return unpacked_page_slots_.size() >= max_unpacked_pages_;
        }

        bool isPackedBufferPoolFull() const {
            return packed_page_buffer_pool_.size() >= max_packed_pages_;
        }

        void removePageFromUnpackedBufferPool(PageId &pid) {
            occupied_status_[unpacked_page_slots_[pid]] = false;
            unpacked_page_slots_.erase(pid);
            page_status_.erase(pid);
        }

        int flushUnpackedPage() {
            int slot_idx = -1;

            if(isUnpackedBufferPoolFull()) {
                PageId evicted_pid = eviction_queue_.front();

                // if pid is not existed, pick another one
                // if the page is pinned, pick another one and push pinned page into end of queue.
                bool has_unpacked_page = hasUnpackedPage(evicted_pid);
                bool is_pinned = has_unpacked_page ? page_status_[evicted_pid][0] : false;
                while(!has_unpacked_page || is_pinned) {
                    if(has_unpacked_page && is_pinned) {
                        eviction_queue_.push(evicted_pid);
                    }

                    eviction_queue_.pop();
                    evicted_pid = eviction_queue_.front();

                    has_unpacked_page = hasUnpackedPage(evicted_pid);
                    is_pinned = has_unpacked_page ? page_status_[evicted_pid][0] : false;
                }

                slot_idx = unpacked_page_slots_[evicted_pid];

                // if the page is dirty, pack it and store in packed buffer pool
                // if not, we dont need to pack it
                if(page_status_[evicted_pid][1]) {
                    emp::Bit *evicted_page = unpacked_buffer_pool_.data() + slot_idx * unpacked_page_size_;

                    PackedPage evicted_packed_page;
                    evicted_packed_page.pid_ = evicted_pid;

                    emp::OMPCPackedWire evicted_pack_wire(block_n_);
                    emp_manager_->pack((Bit *) evicted_page, (Bit *) &evicted_pack_wire, unpacked_page_size_);
                    evicted_packed_page.page_payload_ = evicted_pack_wire;
                    evicted_packed_page.access_counters_ = 0;

                    packed_page_buffer_pool_[evicted_pid] = evicted_packed_page;
                }

                eviction_queue_.pop();
                removePageFromUnpackedBufferPool(evicted_pid);
            }

            return slot_idx;
        }

        int getEmptySlot() {
            int empty_slot_idx = flushUnpackedPage();

            if(empty_slot_idx == -1) {
                for(int i = 0; i < max_unpacked_pages_; ++i) {
                    if(!occupied_status_[i]) {
                        return i;
                    }
                }

                throw std::runtime_error("No empty slot found in unpacked buffer pool");
            }

            return empty_slot_idx;
        }

        emp::Bit *getUnpackedPagePtr(PageId &pid) {
            if(!hasPage(pid)) {
                int empty_slot_idx = getEmptySlot();

                unpacked_page_slots_[pid] = empty_slot_idx;
                page_status_[pid] = {false, false};
                occupied_status_[empty_slot_idx] = true;

                eviction_queue_.push(pid);

                return unpacked_buffer_pool_.data() + empty_slot_idx * unpacked_page_size_;
            }

            if(hasUnpackedPage(pid)) {
                return unpacked_buffer_pool_.data() + unpacked_page_slots_[pid] * unpacked_page_size_;
            }
            else {
                PackedPage packed_page = packed_page_buffer_pool_[pid];

                int empty_slot_idx = getEmptySlot();
                emp_manager_->unpack((Bit *) &packed_page.page_payload_, unpacked_buffer_pool_.data() + empty_slot_idx * unpacked_page_size_, unpacked_page_size_);
                occupied_status_[empty_slot_idx] = true;

                unpacked_page_slots_[pid] = empty_slot_idx;
                page_status_[pid] = {false, false};

                eviction_queue_.push(pid);

                return unpacked_buffer_pool_.data() + empty_slot_idx * unpacked_page_size_;
            }
        }

        void clonePage(PageId &src_pid, PageId &dst_pid) {
            if(!hasPage(src_pid)) {
                throw std::runtime_error("Source page not found for pid: " + src_pid.toString());
            }

            if(hasPackedPage(src_pid)) {
                PackedPage src_page = packed_page_buffer_pool_[src_pid];
                PackedPage dst_page;
                dst_page.pid_ = dst_pid;
                dst_page.page_payload_ = src_page.page_payload_;
                dst_page.access_counters_ = src_page.access_counters_;
                packed_page_buffer_pool_[dst_pid] = dst_page;
            }
            else {
                emp::Bit *src_page_ptr = unpacked_buffer_pool_.data() + unpacked_page_slots_[src_pid] * unpacked_page_size_;
                emp::Bit *dst_page_ptr = getUnpackedPagePtr(dst_pid);

                // TODO: Why does memcpy not work?
                //memcpy(dst_page_ptr, src_page_ptr, unpacked_page_size_);
                for(int i = 0; i < unpacked_page_size_; ++i) {
                    *(dst_page_ptr + i) = *(src_page_ptr + i);
                }

                eviction_queue_.push(src_pid);
                eviction_queue_.push(dst_pid);

                page_status_[dst_pid] = {false, true};
            }
        }

        void initializeColumnPages(int table_id, int col_idx, int tuple_cnt, int rows_per_page) {
            for(int i = 0; i < tuple_cnt; i += rows_per_page) {
                PageId pid = getPageId(table_id, col_idx, i, rows_per_page);
                emp::Bit *page_ptr = getUnpackedPagePtr(pid);
            }
        }

        void removeConsecutivePages(int table_id, int col_idx, int start_row_idx, int pages_to_remove, int rows_per_page) {
            int start_page_idx = start_row_idx / rows_per_page;
            PageId pid = {table_id, col_idx, start_page_idx};
            for(int i = 0; i < pages_to_remove; ++i) {
                if(!hasPage(pid)) {
                    throw std::runtime_error("Page not found for pid: " + pid.toString());
                }

                if(hasUnpackedPage(pid)) {
                    removePageFromUnpackedBufferPool(pid);
                }
                else {
                    packed_page_buffer_pool_.erase(pid);
                }
                ++pid.page_idx_;
            }
        }

        void addConsecutivePages(int table_id, int col_idx, int start_row_idx, int pages_to_add, int rows_per_page) {
            int start_page_idx = start_row_idx / rows_per_page;
            PageId pid = {table_id, col_idx, start_page_idx};
            for(int i = 0; i < pages_to_add; ++i) {
                emp::Bit *page_ptr = getUnpackedPagePtr(pid);
                ++pid.page_idx_;
            }
        }

        void removeUnpackedPagesByTable(int table_id) {
            std::vector<PageId> pids_to_remove;

            for(auto [pid, slot_id] : unpacked_page_slots_) {
                if(pid.table_id_ == table_id) {
                    pids_to_remove.push_back(pid);
                }
            }

            for(auto pid : pids_to_remove) {
                removePageFromUnpackedBufferPool(pid);
            }
        }



        EmpManager *emp_manager_ = nullptr;

        int unpacked_page_size_; // in emp::Bits
        int max_unpacked_pages_;
        int packed_page_size_; // in emp::OMPCPackedWires
        int max_packed_pages_;

        int block_n_;

        std::map<PageId, PackedPage> packed_page_buffer_pool_;

        std::vector<emp::Bit> unpacked_buffer_pool_;
        std::vector<bool> occupied_status_; // 0 - free, 1 - occupied for each slot
        std::queue<PageId> eviction_queue_; // LRU eviction by queue

        std::map<PageId, int> unpacked_page_slots_; // map<pid, slot id in unpacked buffer pool>
        std::map<PageId, std::vector<bool>> page_status_; // map<pid, <pinned_status (0 - unpinned, 1 - pinned), dirty_status (0 - clean, 1 - dirty)>>

    };
}

#endif

#endif
