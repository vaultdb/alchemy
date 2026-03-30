#ifndef _BUFFER_POOL_MANAGER_H_
#define _BUFFER_POOL_MANAGER_H_

#include <list>

#include "emp-tool/emp-tool.h"
#include "common/defs.h"
#include "util/emp_manager/emp_manager.h"
#include <sys/mman.h>


using namespace std;

#include "lru_k_replacer.h"

namespace vaultdb {

    // simple buffer pool with LRU-K eviction policy
    // pages are just a byte array, abstracting away whether we are in bool or Bit mode.
    // modeled loosely on BusTub: https://github.com/cmu-db/bustub
   class BufferPoolManager {
    public:
        long page_size_bytes_ = 2048; // in bytes default value
        long page_cnt_;
        LRUKReplacer *lru_k_replacer_ = nullptr;

        // record stats for the buffer pool
        size_t hits_ = 0L;
        size_t misses_ = 0L;

        std::vector<int8_t> buffer_pool_;


        std::unordered_map<PageId, PositionMapEntry> position_map_; // map<pid, slot id>
        std::unordered_map<int, PageId> reverse_position_map_; // given an offset we want to access, what PID is it?
        list<int> free_frames_; // list of unused frames in the buffer pool

        static BufferPoolManager& getInstance() {
            static BufferPoolManager instance;
            return instance;
        }

        ~BufferPoolManager() {
            cout << stats() << endl;
            munlock(buffer_pool_.data(), buffer_pool_.size());
        };

        string stats() const {
            stringstream s;
            auto reqs = hits_ + misses_;
            s << "Buffer pool requests: " << reqs << " hit rate: " << hits_ << "/" << reqs << ": " << (float) hits_ / ((float) (reqs) ) << ", pinned pages: " << pinnedPageCount() << endl;
            return s.str();
        }

        void initialize(long page_size_bytes, long page_cnt) {
            page_size_bytes_ = page_size_bytes;
            page_cnt_ = page_cnt;
            buffer_pool_ = vector<int8_t>(page_size_bytes_ * page_cnt_, 0);
            if (lru_k_replacer_ != nullptr) {
                delete lru_k_replacer_;
            }
            lru_k_replacer_ = new LRUKReplacer(page_cnt);

            free_frames_.clear();
            for (int i = 0; i < page_cnt_; ++i) {
                free_frames_.push_back(i);
            }

            mlock(buffer_pool_.data(), buffer_pool_.size());

        }


        inline static PageId getPageId(int table_id, int col, int row, int rows_per_page) {
           return {table_id, col, row / rows_per_page};
        }

        template<typename B>
        int8_t *getPagePtr(PageId pid) {
            loadPage<B>(pid);
            return (buffer_pool_.data() + position_map_.at(pid).slot_id_ * page_size_bytes_);
        }


       template<typename B>
       int getFreeFrame();

       template<typename B>
        void clonePage(PageId &src_pid, PageId &dst_pid);


        inline void markDirty(PageId &pid) {
            position_map_.at(pid).dirty_ = true;
        }

        // page pinning and unpinning only needed when we need a page to persist beyond the current scope / calling function
        // since we are not writing this for concurrent queries at this time, we don't need to set/unset this for every getField/setField call
        // Examples of when we need to pin pages: BlockNestedLoopJoin, NestedLoopAggregate output buffer
        inline void pinPage(PageId &pid) {
            auto pair = position_map_.find(pid);
            if (pair != position_map_.end()) {
                pair->second.pinned_ = true;
                // lru_k_replacer_->setEvictable(pair->second.slot_id_, false);
            }
        }

        inline void unpinPage(PageId &pid) {
            auto pair = position_map_.find(pid);
            if (pair != position_map_.end()) {
                pair->second.pinned_ = false;
                // lru_k_replacer_->setEvictable(pair->second.slot_id_, true);
            }
        }

       template<typename B>
        void loadPage(PageId &pid);

       template<typename B>
       void loadColumn(const int & table_id, const int & col_idx, const int & tuple_cnt, const  int & rows_per_page) {
            int page_cnt = tuple_cnt / rows_per_page + ((tuple_cnt % rows_per_page) > 0);

            PageId pid(table_id, col_idx, 0);

            for(int i = 0; i < page_cnt; ++i) {
                loadPage<B>(pid);
                ++pid.page_idx_;
            }
        }

       template<typename B>
        void flushPage(const PageId &pid);

       template<typename B>
        void getPage(const PageId & pid, const int & slot );

       template<typename B>
       void flushTable(const int & table_id) {
           if (table_catalog_.find(table_id) == table_catalog_.end()) {
                return; // no such table
            }

            for(auto pos = position_map_.begin(); pos != position_map_.end(); ++pos) {
                if(pos->first.table_id_ == table_id && pos->second.dirty_) {
                    flushPage<B>(pos->first);
                }
            }
        }

        // for use in cloneColumn
        // flushes column to packed pages and marks them as not dirty
       template<typename B>
       void flushColumn(const int & table_id, const int & col_id) {
            for(auto pos = position_map_.begin(); pos != position_map_.end(); ++pos) {
                if(pos->first.table_id_ == table_id && pos->first.col_id_ == col_id && pos->second.dirty_) { // flush it to disk
                    flushPage<B>(pos->first);
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

       template<typename B>
       void addPageSequence(int table_id, int col_idx, int start_row_idx, int pages_to_add, int rows_per_page) {
            int start_page_idx = start_row_idx / rows_per_page;
            PageId pid = {table_id, col_idx, start_page_idx};
            for(int i = 0; i < pages_to_add; ++i) {
                loadPage<B>(pid);
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

       template<typename B>
        void removeTable(int target_table_id) {
           vector<PageId> to_remove;
            for (auto pos = position_map_.begin(); pos != position_map_.end(); ++pos) {
                if (pos->first.table_id_ == target_table_id) {
                    PageId p = pos->first;
                    // mark the slot for eviction
                    unpinPage(p);
                    auto slot = position_map_[p].slot_id_;
                    to_remove.push_back(p);

                    flushPage<B>(p);
                    reverse_position_map_.erase(slot);
                }
            }

           for (int i = 0; i < to_remove.size(); ++i) {
                auto to_free = position_map_[to_remove[i]].slot_id_;
               free_frames_.push_back(to_free);
               position_map_.erase(to_remove[i]);
            }
            // signal that the table is no longer available
            table_catalog_.erase(target_table_id);

        }

        // for use at the end of each unit test to clear out any references to tables that we are deleting
       template<typename B>
       void reset() {

           for (auto pos = position_map_.begin(); pos != position_map_.end(); ++pos) {
                if (pos->second.dirty_) {
                    flushPage<B>(pos->first);
                }
            }
            position_map_.clear();
            reverse_position_map_.clear();
            table_catalog_.clear();
           free_frames_.clear();
           for (int i = 0; i < page_cnt_; ++i) {
                free_frames_.push_back(i);
            }
        }

       template<typename B>
        inline void registerTable(QueryTable<B> *table) {
            table_catalog_[table->table_id_] = table;
        }


    // needed for singleton setup
    private:
        BufferPoolManager() { }

        std::map<int, void *> table_catalog_; // contains pointers to QueryTable objects, indexed by table_id.  `void *` to support PlainTable and SecureTable






    };
}
#endif
