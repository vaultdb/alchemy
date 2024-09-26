#ifndef _BUFFER_POOL_MANAGER_H_
#define _BUFFER_POOL_MANAGER_H_

#include "emp-tool/emp-tool.h"
#include "common/defs.h"
#include "util/emp_manager/emp_manager.h"

using namespace std;

#if  __has_include("emp-sh2pc/emp-sh2pc.h") || __has_include("emp-zk/emp-zk.h")
namespace vaultdb {
    class BufferPoolManager {
    public:


        void initialize(int unpacked_page_size, int num_unpacked_pages, EmpManager *manager) {}
        ~BufferPoolManager() {};
        static BufferPoolManager& getInstance() {
            static BufferPoolManager  instance;
            return instance;
        }

        PageId getPageId(int table_id, int col, int row, int rows_per_page) {
            return {0, 0, 0};
        }


        bool hasUnpackedPage(PageId &page_id) {
            return false;
        }
        // greedily evict first unpinned page in the queue
        // returns the newly-opened slot in the buffer pool
        int evictPage() {
            return 0;
        }

        int evictPageWithLRUK() {
            return 0;
        }

        void loadPage(PageId &pid) {
        }

        void loadPageWithLRUK(PageId &pid) {
        }

        emp::Bit *getUnpackedPagePtr(PageId &pid) {
            return nullptr;
        }

        void clonePage(PageId &src_pid, PageId &dst_pid) {}

        void loadColumn(const int & table_id, const int & col_idx, const int & tuple_cnt,const  int & rows_per_page)  {}

        void flushPage(PageId &pid) { }
        void flushColumn(const int & table_id, const int & col_id) {}

        void removePageSequence(int table_id, int col_idx, int start_row_idx, int pages_to_remove, int rows_per_page) {}

        void addPageSequence(int table_id, int col_idx, int start_row_idx, int pages_to_add, int rows_per_page) {}

        inline int pinnedPageCount() const {
            return -1;
        }

        void removeTable(int table_id) {}

        void markDirty(PageId &pid) {
        }

        void pinPage(PageId &pid) {
        }

        void unpinPage(PageId & pid) {
            }

        EmpManager *emp_manager_ = nullptr;

        // this is the same as SystemConfiguration::bp_page_size_bits_
        int unpacked_page_size_bits_; // in emp::Bits
        int page_cnt_;

        int block_n_;

        // LRU-K
        bool lru_k_enabled_ = false;
        int K = 3; // LRU-K parameter
        int lru_k_time_cursor_ = 0;
        struct LRUKCompare {
            bool operator()(const std::pair<PageId, int> &lhs, const std::pair<PageId, int> &rhs) const {
                return lhs.second > rhs.second;
            }
        };
        std::priority_queue<std::pair<PageId, int>, std::vector<std::pair<PageId, int>>, LRUKCompare> lru_k_min_heap_;

        // setup for unpacked buffer pool
        std::vector<emp::Bit> unpacked_buffer_pool_;
        std::vector<bool> occupied_status_; // 0 - free, 1 - occupied for each slot
        std::queue<PageId> eviction_queue_; // LRU eviction by queue

        std::map<PageId, int> position_map_; // map<pid, slot id in unpacked buffer pool>
        std::map<PageId, bool[2]> page_status_; // map<pid, <pinned_status (0 - unpinned, 1 - pinned), dirty_status (0 - clean, 1 - dirty)>>

        // setup for packed buffer pool
        // map<table_id, map<col_id, a pointer of a vector OMPCPackedWires>>
        std::map<int, std::map<int, OMPCPackedWire*>> packed_pages_;

        void reset();
    private:
         BufferPoolManager() {};

    };
}
#else

#include "emp-rescu/emp-rescu.h"
#define __OMPC_BACKEND__ 1
namespace vaultdb {
    class PackedColumnTable;
    class BufferedColumnTable;

    class BufferPoolManager {
    public:


        EmpManager *emp_manager_ = nullptr;

        int unpacked_page_size_bits_ = 2048; // in emp::Bits, default value
        int page_cnt_;

        // record stats for the buffer pool
        size_t hits_ = 0L;
        size_t misses_ = 0L;
        size_t pack_calls_ = 0L;
        size_t unpack_calls_ = 0L;

        int block_n_; // Bits per wire?

        // setup for unpacked buffer pool
        std::vector<emp::Bit> unpacked_buffer_pool_;
        int clock_hand_position_ = 0;

        // LRU-K
        bool lru_k_enabled_ = false;
        int K = 3; // LRU-K parameter
        int lru_k_time_cursor_ = 0;
        struct LRUKCompare {
            bool operator()(const std::pair<PageId, int> &lhs, const std::pair<PageId, int> &rhs) const {
                return lhs.second > rhs.second;
            }
        };
        std::priority_queue<std::pair<PageId, int>, std::vector<std::pair<PageId, int>>, LRUKCompare> lru_k_min_heap_;


        std::map<PageId, PositionMapEntry> position_map_; // map<pid, slot id in unpacked buffer pool>
        std::map<int, PageId> reverse_position_map_; // given an offset we want to access, what PID is it?

        static BufferPoolManager& getInstance() {
            static BufferPoolManager  instance;
            return instance;
        }

        ~BufferPoolManager() {
            cout << stats() << endl;
        };

        string stats() const {
            stringstream s;
            auto reqs = hits_ + misses_;
            s << "Buffer pool requests: " << reqs << " hit rate: " << hits_ << "/" << reqs << ": " << (float) hits_ / ((float) (reqs) ) << ", pack calls: " << pack_calls_ << " unpack calls: " << unpack_calls_ <<  " pinned pages: " << pinnedPageCount() << endl;
            return s.str();
        }

        void initialize(int unpacked_page_bits, int unpacked_page_cnt, EmpManager *manager)  {
            unpacked_page_size_bits_ = unpacked_page_bits;
            page_cnt_ = unpacked_page_cnt;
            emp_manager_ = manager;
            // block size of each packed wire based on unpacked page size
            block_n_ = unpacked_page_size_bits_ / 128 + (unpacked_page_size_bits_ % 128 != 0);
            // initialize unpacked page buffer
            unpacked_buffer_pool_ = std::vector<emp::Bit>(unpacked_page_size_bits_ * page_cnt_, emp::Bit(0));

        }


        inline static PageId getPageId(int table_id, int col, int row, int rows_per_page) {
           return {table_id, col, row / rows_per_page};
        }

        emp::Bit *getUnpackedPagePtr(PageId &pid) {
            loadPage(pid);
            return unpacked_buffer_pool_.data() + position_map_.at(pid).slot_id_ * unpacked_page_size_bits_;
        }


        int evictPage();

        int evictPageWithLRUK();



        void clonePage(PageId &src_pid, PageId &dst_pid);


        inline void markDirty(PageId &pid) {
            position_map_.at(pid).dirty_ = true;
        }

        // page pinning and unpinning only needed when we need a page to persist beyond the current scope / calling function
        // since we are not writing this for concurrent queries at this time, we don't need to set/unset this for every getField/setField call
        // Examples of when we need to pin pages: BlockNestedLoopJoin, NestedLoopAggregate output buffer
        inline void pinPage(PageId &pid) {
            position_map_.at(pid).pinned_ = true;
        }

        inline void unpinPage(PageId &pid) {
            position_map_.at(pid).pinned_ = false;
        }


        void loadPage(PageId &pid);

        void loadPageWithLRUK(PageId &pid);

        void loadColumn(const int & table_id, const int & col_idx, const int & tuple_cnt, const  int & rows_per_page) {
            int page_cnt = tuple_cnt / rows_per_page + (tuple_cnt % rows_per_page != 0);

            PageId pid(table_id, col_idx, 0);

            for(int i = 0; i < page_cnt; ++i) {
                loadPage(pid);
                ++pid.page_idx_;
            }
        }

        void flushPage(const PageId &pid);

        void flushTable(const int & table_id) {
            for(auto pos = position_map_.begin(); pos != position_map_.end(); ++pos) {
                if(pos->first.table_id_ == table_id && pos->second.dirty_) {
                    flushPage(pos->first);
                }
            }
        }

        // for use in cloneColumn
        // flushes column to packed pages and marks them as not dirty
        void flushColumn(const int & table_id, const int & col_id) {
            for(auto pos = position_map_.begin(); pos != position_map_.end(); ++pos) {
                if(pos->first.table_id_ == table_id && pos->first.col_id_ == col_id) { // flush it to packed pages
                    flushPage(pos->first);
                }
            }
        }

        void removePageSequence(int table_id, int col_idx, int start_row_idx, int pages_to_remove, int rows_per_page) {
            int start_page_idx = start_row_idx / rows_per_page;
            PageId pid = {table_id, col_idx, start_page_idx};
            for(int i = 0; i < pages_to_remove; ++i) {
                unpinPage(pid);
                ++pid.page_idx_;
            }
        }

        void addPageSequence(int table_id, int col_idx, int start_row_idx, int pages_to_add, int rows_per_page) {
            int start_page_idx = start_row_idx / rows_per_page;
            PageId pid = {table_id, col_idx, start_page_idx};
            for(int i = 0; i < pages_to_add; ++i) {
                loadPage(pid);
                ++pid.page_idx_;
            }
        }

        inline int pinnedPageCount() const {
           int counter = 0;
           for(auto entry : position_map_) {
               if(entry.second.pinned_) {
                   ++counter;
               }
           }

           return counter;
        }

        void removeTable(int target_table_id) {
            for (auto pos = position_map_.begin(); pos != position_map_.end(); ++pos) {
                if (pos->first.table_id_ == target_table_id) {
                    PageId p = pos->first;
                    // mark the slot for eviction
                    unpinPage(p);
                    // this is only necessary when we have a table backed up by persistent storage
                    // otherwise we have no way of accessing this temp table again
                    // need a way to differentiate between these 2 cases
                    //flushPage(p);
                }
            }
            // signal that the PackedColumnTable is no longer memory-resident
            tables_catalog_.erase(target_table_id);

        }

        // for use at the end of each unit test to clear out any references to tables that we are deleting
        void reset() {
            // TODO: flush dirty pages.  this will eventually push these to non-volatile storage as needed
            // right now we are calling this too late to do this (after the parent objects (PackedColumnTables) are deleted)
            // This should probably be part of PackedColumnTable's constructor
            position_map_.clear();
            reverse_position_map_.clear();
            tables_catalog_.clear();
            clock_hand_position_ = 0;
        }

        inline void registerTable(int table_id, QueryTable<Bit> *table) {
            tables_catalog_[table_id] = table;
        }


    // needed for singleton setup
    private:
        BufferPoolManager() { }


        std::map<int, QueryTable<Bit> *> tables_catalog_;




    };
} // namespace vaultdb

#endif

#endif
