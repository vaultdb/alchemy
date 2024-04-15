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
            int start_row_id_;
            int end_row_id_;
        } PageId;

        // data from same column can be same page - easy to get offset.
        typedef struct unpackd_page_ {
            PageId pid_;
            vector<emp::Bit> page_payload_;
            int access_counters_; // for LRU
            bool pined = false;
        } UnpackedPage;

        typedef struct packd_page_ {
            PageId pid_;
            emp::OMPCPackedWire page_payload_;
            int access_counters_; // for LRU
            bool pined = false;
        } PackedPage;

        BufferPoolManager() {};

        BufferPoolManager(int unpacked_page_size, int num_unpacked_pages, int packed_page_size, int num_packed_pages, EmpManager *manager) : unpacked_page_size_(unpacked_page_size), num_unpacked_pages_(num_unpacked_pages), packed_page_size_(packed_page_size), num_packed_pages_(num_packed_pages), emp_manager_(manager) {
            
        }

        ~BufferPoolManager() {};

        PageId getPageId(int table_id, int col, int row, int rows_per_page) {
            return {0, 0, 0, 0};
        }

        string getPageIdKey(const PageId &page_id) {
            return "";
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



        EmpManager *emp_manager_ = nullptr;

        int unpacked_page_size_; // in emp::Bits
        int num_unpacked_pages_;
        int packed_page_size_; // in emp::OMPCPackedWire
        int num_packed_pages_;

        int block_n_;

        std::map<string, PageId> page_id_map_;
        std::map<string, UnpackedPage> unpacked_page_buffer_pool_;
        std::map<string, PackedPage> packed_page_buffer_pool_;

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
            int start_row_id_;
            int end_row_id_;
        } PageId;

        // data from same column can be same page - easy to get offset.
        typedef struct unpackd_page_ {
            PageId pid_;
            vector<emp::Bit> page_payload_;
            int access_counters_; // for LRU
            bool pined = false;
        } UnpackedPage;

        typedef struct packd_page_ {
            PageId pid_;
            emp::OMPCPackedWire page_payload_;
            int access_counters_; // for LRU
            bool pined = false;
        } PackedPage;

        BufferPoolManager() {};

        BufferPoolManager(int unpacked_page_size, int num_unpacked_pages, int packed_page_size, int num_packed_pages, EmpManager *manager) : unpacked_page_size_(unpacked_page_size), num_unpacked_pages_(num_unpacked_pages), packed_page_size_(packed_page_size), num_packed_pages_(num_packed_pages), emp_manager_(manager) {
            block_n_ = unpacked_page_size_ / 128 + (unpacked_page_size_ % 128 != 0);
        }

        ~BufferPoolManager() {};

        PageId getPageId(int table_id, int col, int row, int rows_per_page) {
            int start_row_id = row - (row % rows_per_page);
            int end_row_id = start_row_id + rows_per_page - 1;
            return {table_id, col, start_row_id, end_row_id};
        }

        string getPageIdKey(const PageId &page_id) {
            return std::to_string(page_id.table_id_) + "," + std::to_string(page_id.col_id_) + "," + std::to_string(page_id.start_row_id_) + "," + std::to_string(page_id.end_row_id_);
        }

        void insertPageIdMap(const PageId &page_id) {
            page_id_map_[getPageIdKey(page_id)] = page_id;
        }

        bool hasUnpackedPage(PageId &page_id) {
            return unpacked_page_buffer_pool_.find(getPageIdKey(page_id)) != unpacked_page_buffer_pool_.end();
        }

        bool hasPackedPage(PageId &page_id) {
            return packed_page_buffer_pool_.find(getPageIdKey(page_id)) != packed_page_buffer_pool_.end();
        }

        bool hasPage(PageId &page_id) {
            return hasUnpackedPage(page_id) || hasPackedPage(page_id);
        }

        bool isUnpackedBufferPoolFull() {
            return unpacked_page_buffer_pool_.size() >= num_unpacked_pages_;
        }

        bool isPackedBufferPoolFull() {
            return packed_page_buffer_pool_.size() >= num_packed_pages_;
        }

        void flushUnpackedPageByLRU() {
            if(isUnpackedBufferPoolFull()) {
                int least_counters = INT_MAX;
                string oldest_key;
                for (auto &[key, page]: unpacked_page_buffer_pool_) {
                    if(page.pined) {
                        continue;
                    }
                    if (page.access_counters_ < least_counters) {
                        least_counters = page.access_counters_;
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
                unpacked_page_buffer_pool_[getPageIdKey(up.pid_)] = up;

                return up;
            }

            if(hasUnpackedPage(page_id)) {
                return unpacked_page_buffer_pool_[getPageIdKey(page_id)];
            }
            else {
                PackedPage packed_page;
                if(hasPackedPage(page_id)) {
                    // Make space for unpacked page if unpacked buffer pool is full
                    flushUnpackedPageByLRU();

                    // Unpack the page to unpacked buffer pool
                    packed_page = packed_page_buffer_pool_[getPageIdKey(page_id)];
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
                unpacked_page_buffer_pool_[getPageIdKey(unpacked_page.pid_)] = unpacked_page;

                return unpacked_page;
            }
        }

        void flushPagesGivenTableId(int table_id) {
            for(auto it = unpacked_page_buffer_pool_.begin(); it != unpacked_page_buffer_pool_.end();) {
                std::istringstream ss(it->first);
                string table_id_str;
                std::getline(ss, table_id_str, ',');

                int id = std::stoi(table_id_str);

                if(id == table_id) {
                   unpacked_page_buffer_pool_.erase(it++);
                }
                else {
                    ++it;
                }
            }

            for(auto it = packed_page_buffer_pool_.begin(); it != packed_page_buffer_pool_.end();) {
                std::istringstream ss(it->first);
                string table_id_str;
                std::getline(ss, table_id_str, ',');

                int id = std::stoi(table_id_str);

                if(id == table_id) {
                    packed_page_buffer_pool_.erase(it++);
                }
                else {
                    ++it;
                }
            }
        }

        void clonePage(PageId &src_pid, PageId &dst_pid) {
            if(!hasPage(src_pid)) {
                cout << "src pid: " << getPageIdKey(src_pid) << endl;
                throw std::runtime_error("Source page not found.");
            }

            if(hasPage(dst_pid)) {
                throw std::runtime_error("Destination page already exists.");
            }

            if(hasPackedPage(src_pid)) {
                PackedPage src_page = packed_page_buffer_pool_[getPageIdKey(src_pid)];
                PackedPage dst_page;
                dst_page.pid_ = dst_pid;
                dst_page.page_payload_ = src_page.page_payload_;
                dst_page.access_counters_ = src_page.access_counters_;
                packed_page_buffer_pool_[getPageIdKey(dst_pid)] = dst_page;
            }
            else {
                UnpackedPage src_page = unpacked_page_buffer_pool_[getPageIdKey(src_pid)];
                src_page.pined = true;
                unpacked_page_buffer_pool_[getPageIdKey(src_pid)] = src_page;

                UnpackedPage dst_page = getUnpackedPage(dst_pid);
                dst_page.page_payload_ = src_page.page_payload_;
                dst_page.access_counters_ = src_page.access_counters_;
                unpacked_page_buffer_pool_[getPageIdKey(dst_pid)] = dst_page;

                src_page.pined = false;
                unpacked_page_buffer_pool_[getPageIdKey(src_pid)] = src_page;
            }
        }



        EmpManager *emp_manager_ = nullptr;

        int unpacked_page_size_; // in emp::Bits
        int num_unpacked_pages_;
        int packed_page_size_; // in emp::OMPCPackedWire
        int num_packed_pages_;

        int block_n_;

        std::map<string, PageId> page_id_map_;
        std::map<string, UnpackedPage> unpacked_page_buffer_pool_;
        std::map<string, PackedPage> packed_page_buffer_pool_;

    };
}

#endif

#endif
