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


        void initialize(int unpacked_page_size, int num_unpacked_pages, int packed_page_size, int num_packed_pages, EmpManager *manager);
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
        }

        void loadPage(PageId &pid) {
        }

        emp::Bit *getUnpackedPagePtr(PageId &pid) {
            return nullptr;
        }

        void clonePage(PageId &src_pid, PageId &dst_pid) {}

        void loadColumn(const int & table_id, const int & col_idx, const int & tuple_cnt,const  int & rows_per_page)  {}

        void removePageSequence(int table_id, int col_idx, int start_row_idx, int pages_to_remove, int rows_per_page) {}

        void addPageSequence(int table_id, int col_idx, int start_row_idx, int pages_to_add, int rows_per_page) {}

        void removeUnpackedPagesByTable(int table_id) {}

        void markDirty(PageId &pid) {
        }

        void pinPage(PageId &pid) {
        }

        void unpinPage(PageId & pid) {
            }

        EmpManager *emp_manager_ = nullptr;

        int unpacked_page_size_bits_; // in emp::Bits
        int page_cnt_;
        int packed_page_size_wires_; // in emp::OMPCPackedWires
        int max_packed_page_cnt_;

        int block_n_;

        // setup for unpacked buffer pool
        std::vector<emp::Bit> unpacked_buffer_pool_;
        std::vector<bool> occupied_status_; // 0 - free, 1 - occupied for each slot
        std::queue<PageId> eviction_queue_; // LRU eviction by queue

        std::map<PageId, int> position_map_; // map<pid, slot id in unpacked buffer pool>
        std::map<PageId, bool[2]> page_status_; // map<pid, <pinned_status (0 - unpinned, 1 - pinned), dirty_status (0 - clean, 1 - dirty)>>

        // setup for packed buffer pool
        // map<table_id, map<col_id, a pointer of a vector OMPCPackedWires>>
        std::map<int, std::map<int, OMPCPackedWire*>> packed_buffer_pool_;

    private:
         BufferPoolManager() {};

    };
}
#else

#include "emp-rescu/emp-rescu.h"
#define __OMPC_BACKEND__ 1
namespace vaultdb {
    class BufferPoolManager {
    public:


        EmpManager *emp_manager_ = nullptr;

        int unpacked_page_size_bits_; // in emp::Bits
        int page_cnt_;
        int packed_page_size_wires_; // in emp::OMPCPackedWires
        int max_packed_page_cnt_;

        int block_n_; // Bits per wire?

        // setup for unpacked buffer pool
        std::vector<emp::Bit> unpacked_buffer_pool_;
        int clock_hand_position_ = 0;


        std::map<PageId, PositionMapEntry> position_map_; // map<pid, slot id in unpacked buffer pool>
        std::map<int, PageId> reverse_position_map_; // given an offset we want to access, what PID is it?  Needed to maintain constant time lookups


        // JMR: why do we need this here?  If we have a system catalog or what have you
        // then we can just maintain a map of <table_id, PackedColumnTable*> and look it up directly.
        std::map<int, std::map<int, OMPCPackedWire *> > packed_buffer_pool_;

        static BufferPoolManager& getInstance() {
            static BufferPoolManager  instance;
            return instance;
        }

        ~BufferPoolManager() {};

        void initialize(int unpacked_page_size, int num_unpacked_pages, int packed_page_size, int num_packed_pages, EmpManager *manager)  {

            unpacked_page_size_bits_ = unpacked_page_size;
            page_cnt_ = num_unpacked_pages;
            packed_page_size_wires_ = packed_page_size;
            max_packed_page_cnt_ = num_packed_pages;
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


        // greedily evict first unpinned page in the queue
        // returns the newly-opened slot in the buffer pool
        int evictPage() {


           PositionMapEntry pos;
           //  uninitialized slot, this should only happen when we are warming up the buffer pool
           if(reverse_position_map_.find(clock_hand_position_) == reverse_position_map_.end())  {
               int slot = clock_hand_position_;
               clock_hand_position_ = (clock_hand_position_ + 1) % page_cnt_;
               cout << "Evict page setting us to initalize slot " << slot << '\n';
               return slot;
           };

            int clock_hand_starting_pos = clock_hand_position_;

            PageId pid = reverse_position_map_[clock_hand_position_];
            pos = position_map_.at(pid);

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

           cout << "Evicting page at slot " << pos.toString() << ", pid " << pid.toString() << '\n';

        if (pos.dirty_) {
            emp::Bit *src_ptr =  unpacked_buffer_pool_.data() + position_map_.at(pid).slot_id_ * unpacked_page_size_bits_;
            emp::OMPCPackedWire *dst_ptr = packed_buffer_pool_[pid.table_id_][pid.col_id_] + pid.page_idx_;
            emp_manager_->pack(src_ptr, (Bit *) dst_ptr, unpacked_page_size_bits_);
        }

        // remove it from the position map even if it is not dirty
        position_map_.erase(pid);
        reverse_position_map_.erase(pos.slot_id_);

        // increment clock hand one more time for next round
        clock_hand_position_ = (clock_hand_position_ + 1) % page_cnt_;
        return pos.slot_id_;
     }


        void loadPage(PageId &pid) {

            if(position_map_.find(pid) != position_map_.end()) {
                return;
            }

            OMPCPackedWire *src_ptr = packed_buffer_pool_[pid.table_id_][pid.col_id_] + pid.page_idx_;
            int target_slot = evictPage();
            cout << "loading page: " << pid.toString() <<  " to slot " << target_slot << '\n';

            Bit *dst_ptr = unpacked_buffer_pool_.data() + target_slot * unpacked_page_size_bits_;
            emp_manager_->unpack((Bit *) src_ptr, dst_ptr, unpacked_page_size_bits_);

            PositionMapEntry p(target_slot, true, false);
            position_map_[pid] = p;
            reverse_position_map_[target_slot] = pid;

        }



        // makes a deep copy of src_pid at dst_pid
        void clonePage(PageId &src_pid, PageId &dst_pid) {
           // if src page is unpacked, copy it to dst page in buffer pool
           // mark dst_page as dirty
           if(position_map_.find(src_pid) != position_map_.end()
              && position_map_.at(src_pid).dirty_) {
               emp::Bit *src_page_ptr = unpacked_buffer_pool_.data() + position_map_.at(src_pid).slot_id_ * unpacked_page_size_bits_;
               loadPage(dst_pid); // make sure dst page is loaded (if not already in buffer pool
               emp::Bit *dst_page_ptr = unpacked_buffer_pool_.data() + position_map_.at(dst_pid).slot_id_ * unpacked_page_size_bits_;

               memcpy(dst_page_ptr, src_page_ptr, unpacked_page_size_bits_);

               position_map_.at(dst_pid).dirty_ = true;

           }
           else {
                emp::OMPCPackedWire *src_page_ptr = packed_buffer_pool_[src_pid.table_id_][src_pid.col_id_] + src_pid.page_idx_;
                emp::OMPCPackedWire *dst_page_ptr = packed_buffer_pool_[dst_pid.table_id_][dst_pid.col_id_] + dst_pid.page_idx_;
                *dst_page_ptr = *src_page_ptr;
           }

        }

        inline void markDirty(PageId &pid) {
            position_map_.at(pid).dirty_ = true;
        }

        inline void pinPage(PageId &pid) {
            position_map_.at(pid).pinned_ = true;
            // if it is in the buffer pool queue, we need to skip it
        }

        inline void unpinPage(PageId &pid) {
            // if we are toggling it from pinned to unpinned, then we need to add it to the eviction queue
            if(position_map_[pid].pinned_) {
                position_map_.at(pid).pinned_ = false;

            }
        }

        void loadColumn(const int & table_id, const int & col_idx, const int & tuple_cnt, const  int & rows_per_page) {
            int page_cnt = tuple_cnt / rows_per_page + (tuple_cnt % rows_per_page != 0);

            PageId pid(table_id, col_idx, 0);

            for(int i = 0; i < page_cnt; ++i) {
                loadPage(pid);
                ++pid.page_idx_;
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

        void removeUnpackedPagesByTable(int target_table_id) {
            for (auto pos = position_map_.begin(); pos != position_map_.end(); ++pos) {
                if (pos->first.table_id_ == target_table_id) {
                    PageId p = pos->first;
                    // enqueue the slot for eviction
                    unpinPage(p);
                }

            }
        }

    // needed for singleton setup
    private:
        BufferPoolManager() {}






    };
} // namespace vaultdb

#endif

#endif
