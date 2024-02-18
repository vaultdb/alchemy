#ifndef _BUFFER_POOL_MANAGER_H_
#define _BUFFER_POOL_MANAGER_H_

#include "emp-tool/emp-tool.h"
#include "emp-rescu/emp-rescu.h"
#include "common/defs.h"
#include "util/emp_manager/emp_manager.h"

using namespace std;

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
            time_point<high_resolution_clock> timestamp_; // for LRU
        } UnpackedPage;

        typedef struct packd_page_ {
            PageId pid_;
            emp::OMPCPackedWire page_payload_;
            time_point<high_resolution_clock> timestamp_; // for LRU
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

        bool isUnpackedBufferPoolFull() {
            return unpacked_page_buffer_pool_.size() >= num_unpacked_pages_;
        }

        bool isPackedBufferPoolFull() {
            return packed_page_buffer_pool_.size() >= num_packed_pages_;
        }

        void flushUnpackedPageByLRU() {
            if(isUnpackedBufferPoolFull()) {
                time_point<high_resolution_clock> oldest_time = high_resolution_clock::now();
                string oldest_key;
                for (auto &[key, page]: unpacked_page_buffer_pool_) {
                    if (page.timestamp_ < oldest_time) {
                        oldest_time = page.timestamp_;
                        oldest_key = key;
                    }
                }
                UnpackedPage oldest_unpacked_page = unpacked_page_buffer_pool_[oldest_key];

                // TODO: flush to disk if necessary
                if(isPackedBufferPoolFull()) {
                    // NYI
                }

                PackedPage packed_page;
                packed_page.pid_ = oldest_unpacked_page.pid_;

                emp::OMPCPackedWire pack_wire(block_n_);
                emp_manager_->pack(oldest_unpacked_page.page_payload_.data(), (Bit *) &pack_wire, unpacked_page_size_);
                packed_page.page_payload_ = pack_wire;
                packed_page.timestamp_ = high_resolution_clock::now();

                packed_page_buffer_pool_[oldest_key] = packed_page;

                unpacked_page_buffer_pool_.erase(oldest_key);

                //cout << "Oldest page - " << getPageIdKey(oldest_unpacked_page.pid_) << " - flushed to packed buffer pool" << endl;
            }
        }

        UnpackedPage getUnpackedPage(PageId &page_id) {
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
                }

                UnpackedPage unpacked_page;
                unpacked_page.pid_ = page_id;
                unpacked_page.page_payload_ = std::vector<emp::Bit>(unpacked_page_size_, emp::Bit(0));
                emp_manager_->unpack((Bit *) &packed_page.page_payload_, unpacked_page.page_payload_.data(), unpacked_page_size_);
                unpacked_page.timestamp_ = high_resolution_clock::now();

                unpacked_page_buffer_pool_[getPageIdKey(page_id)] = unpacked_page;

                return unpacked_page;
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
