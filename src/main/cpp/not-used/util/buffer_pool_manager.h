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

        void flushUnpackedPageByLRU() {

        }

        UnpackedPage getUnpackedPage(PageId &page_id) {
            UnpackedPage up;
            return up;
        }

        void flushPagesGivenTableId(int table_id) {}

        void clonePage(PageId &src_pid, PageId &dst_pid) {}

        void initializeColumnPages(int table_id, int col_idx, int tuple_cnt, int rows_per_page) {}

        void removeConsecutivePages(int table_id, int col_idx, int start_row_idx, int pages_to_remove, int rows_per_page) {}

        void addConsecutivePages(int table_id, int col_idx, int start_row_idx, int pages_to_add, int rows_per_page) {}



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
            block_n_ = unpacked_page_size_ / 128 + (unpacked_page_size_ % 128 != 0);
        }

        ~BufferPoolManager() {};

        static inline PageId getPageId(int table_id, int col, int row, int rows_per_page) {
            return {table_id, col, row / rows_per_page};
        }


        bool hasUnpackedPage(PageId &page_id) {
            return unpacked_page_buffer_pool_.find(page_id) != unpacked_page_buffer_pool_.end();
        }

        bool hasPackedPage(PageId &page_id) {
            return packed_page_buffer_pool_.find(page_id) != packed_page_buffer_pool_.end();
        }

        bool hasPage(PageId &page_id) {
            return hasUnpackedPage(page_id) || hasPackedPage(page_id);
        }

        bool isUnpackedBufferPoolFull() const {
            return unpacked_page_buffer_pool_.size() >= max_unpacked_pages_;
        }

        bool isPackedBufferPoolFull() const {
            return packed_page_buffer_pool_.size() >= max_packed_pages_;
        }

        void flushUnpackedPageByLRU() {
            if(isUnpackedBufferPoolFull()) {
                int min_counter = INT_MAX;
                PageId oldest_key;
                for (auto &[key, page]: unpacked_page_buffer_pool_) {
                    if(page.pinned_) {
                        continue;
                    }
                    if (page.access_counters_ < min_counter) {
                        min_counter = page.access_counters_;
                        oldest_key = key;
                    }
                }
                UnpackedPage oldest_unpacked_page = unpacked_page_buffer_pool_[oldest_key];

                // TODO: flush to disk if necessary
//                if(isPackedBufferPoolFull()) {
//                    throw std::runtime_error("Packed buffer pool is full.");
//                }

                PackedPage packed_page;
                packed_page.pid_ = oldest_unpacked_page.pid_;

                emp::OMPCPackedWire pack_wire(block_n_);
                emp_manager_->pack(oldest_unpacked_page.page_payload_.data(), (Bit *) &pack_wire, unpacked_page_size_);
                packed_page.page_payload_ = pack_wire;
                packed_page.access_counters_ = 0;

                packed_page_buffer_pool_[oldest_key] = packed_page;

                unpacked_page_buffer_pool_.erase(oldest_key);

                //cout << "Oldest page - " << getPageIdKey(oldest_unpacked_page.pid_) << " - flushed to packed buffer pool" << endl;
            }
        }

        UnpackedPage getUnpackedPage(PageId &page_id) {
            if(!hasPage(page_id)) {
                flushUnpackedPageByLRU();

                UnpackedPage up;
                up.pid_ = page_id;
                up.page_payload_ = std::vector<emp::Bit>(unpacked_page_size_, emp::Bit(0));
                up.access_counters_ = 0;
                unpacked_page_buffer_pool_[up.pid_] = up;

                return up;
            }

            if(hasUnpackedPage(page_id)) {
                return unpacked_page_buffer_pool_[page_id];
            }
            else {
                PackedPage packed_page;
                if(hasPackedPage(page_id)) {
                    // Make space for unpacked page if unpacked buffer pool is full
                    flushUnpackedPageByLRU();

                    // Unpack the page to unpacked buffer pool
                    packed_page = packed_page_buffer_pool_[page_id];
                }
                else {
                    // TODO: get packed page from disk
                    throw std::runtime_error("disk read not implemented yet");
                }

                UnpackedPage unpacked_page;
                unpacked_page.pid_ = page_id;
                unpacked_page.page_payload_ = std::vector<emp::Bit>(unpacked_page_size_, emp::Bit(0));
                emp_manager_->unpack((Bit *) &packed_page.page_payload_, unpacked_page.page_payload_.data(), unpacked_page_size_);
                unpacked_page.access_counters_ = 0;
                unpacked_page_buffer_pool_[unpacked_page.pid_] = unpacked_page;

                return unpacked_page;
            }
        }

        void flushPagesGivenTableId(int table_id) {
            for(auto it = unpacked_page_buffer_pool_.begin(); it != unpacked_page_buffer_pool_.end();) {
                if(it->first.table_id_ == table_id) {
                    unpacked_page_buffer_pool_.erase(it++);
                }
                else {
                    ++it;
                }
            }

            for(auto it = packed_page_buffer_pool_.begin(); it != packed_page_buffer_pool_.end();) {
                if(it->first.table_id_ == table_id) {
                    packed_page_buffer_pool_.erase(it++);
                }
                else {
                    ++it;
                }
            }
        }

        void clonePage(PageId &src_pid, PageId &dst_pid) {
            if(!hasPage(src_pid)) {
                throw std::runtime_error("Source page not found for pid: " + src_pid.toString());
            }

            if(hasPage(dst_pid)) {
                throw std::runtime_error("Destination page already exists: " + dst_pid.toString());
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
                UnpackedPage src_page = unpacked_page_buffer_pool_[src_pid];
                src_page.pinned_ = true;
                unpacked_page_buffer_pool_[src_pid] = src_page;

                UnpackedPage dst_page = getUnpackedPage(dst_pid);
                dst_page.page_payload_ = src_page.page_payload_;
                dst_page.access_counters_ = src_page.access_counters_;
                unpacked_page_buffer_pool_[dst_pid] = dst_page;

                src_page.pinned_ = false;
                unpacked_page_buffer_pool_[src_pid] = src_page;
            }
        }

        void initializeColumnPages(int table_id, int col_idx, int tuple_cnt, int rows_per_page) {
            for(int i = 0; i < tuple_cnt; i += rows_per_page) {
                PageId pid = getPageId(table_id, col_idx, i, rows_per_page);
                UnpackedPage up = getUnpackedPage(pid);
                // TODO: pin the pages if necessary
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
                    unpacked_page_buffer_pool_.erase(pid);
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
                UnpackedPage up = getUnpackedPage(pid);
                ++pid.page_idx_;
                // TODO: pin the pages if necessary
            }
        }



        EmpManager *emp_manager_ = nullptr;

        int unpacked_page_size_; // in emp::Bits
        int max_unpacked_pages_;
        int packed_page_size_; // in emp::OMPCPackedWires
        int max_packed_pages_;

        int block_n_;

        std::map<PageId, UnpackedPage> unpacked_page_buffer_pool_;
        std::map<PageId, PackedPage> packed_page_buffer_pool_;

    };
}

#endif

#endif
