#ifndef VAULTDB_EMP_SECRET_SHARE_AND_PACK_TPCH_DATA_FROM_QUERY_H
#define VAULTDB_EMP_SECRET_SHARE_AND_PACK_TPCH_DATA_FROM_QUERY_H

#include "operators/secure_sql_input.h"
#include "query_table/packed_column_table.h"
#include "util/buffer_pool/buffer_pool_manager.h"

using namespace vaultdb;

class SecretShareAndPackTpchDataFromQuery {
public:
    SecretShareAndPackTpchDataFromQuery(std::string db_name, std::string sql, std::string table_name) : db_name_(db_name), sql_(sql), table_(
            nullptr), table_name_(table_name) {}

    void set_db_name(std::string db_name) {
        db_name_ = db_name;
    }

    void set_sql(std::string sql) {
        sql_ = sql;
    }

    void set_table_name(std::string table_name) {
        table_name_ = table_name;
    }

    PackedColumnTable *getTable() {
        // secret share
        SecureSqlInput *secure_input = new SecureSqlInput(db_name_, sql_, false);
        table_ = (PackedColumnTable *) secure_input->getOutput();

        // pack everything in unpacked buffer pool to packed buffer pool
        std::map<BufferPoolManager::PageId, int> unpacked_page_slots = bpm_->unpacked_page_slots_;
        for(auto &unpacked_page : unpacked_page_slots) {
            BufferPoolManager::PageId pid = unpacked_page.first;
            int slot = unpacked_page.second;

            emp::Bit *current_slot_ptr = bpm_->unpacked_buffer_pool_.data() + slot * bpm_->unpacked_page_size_;

            emp::OMPCPackedWire packed_wire(bpm_->block_n_);
            emp_manager_->pack(current_slot_ptr, (Bit*) &packed_wire, bpm_->unpacked_page_size_);
            emp::OMPCPackedWire *packed_wires_ptr = bpm_->packed_buffer_pool_[pid.table_id_][pid.col_id_]->data() + pid.page_idx_;
            *packed_wires_ptr = packed_wire;
            (*(bpm_->wire_status_[pid.table_id_][pid.col_id_]))[pid.page_idx_] = true;

            bpm_->removeUnpackedPage(pid);
        }

        return table_;
    }

    void save_table_to_disk(std::string path, int party) {
        // save tuple cnt
        DataUtilities::writeFile(path + "tuple.cnt", to_string(this->table_->tuple_cnt_));
        // save schema
        DataUtilities::writeFile(path + this->table_name_ + ".schema", this->table_->getSchema().prettyPrint());

        // save packed buffer pool
        for(int i = -1; i < this->table_->getSchema().getFieldCount(); ++i) {
            for(int j = 0; j < this->table_->packed_buffer_pool_[i].size(); ++j) {
                std::string file_name = path + "packed_" + this->table_name_ + "_col_" + std::to_string(i) + "_page_" + std::to_string(j) + "_" + std::to_string(party) + ".page";

                vector<int8_t> serialized_wire = this->table_->serializePackedWire(i, j);
                DataUtilities::writeFile(file_name, serialized_wire);
            }
        }
    }

    PackedColumnTable *load_table_from_disk(std::string path, int party) {
        // load tuple cnt
        int tuple_cnt = std::stoi(DataUtilities::readTextFileToString(path + "tuple.cnt"));

        // load schema
        QuerySchema schema(DataUtilities::readTextFileToString(path + this->table_name_ + ".schema"));

        // load packed buffer pool
        PackedColumnTable *loaded_table = (PackedColumnTable *) QueryTable<Bit>::getTable(tuple_cnt, schema);

        for(int i = -1; i < schema.getFieldCount(); ++i) {
            for(int j = 0; j < loaded_table->packed_buffer_pool_[i].size(); ++j) {
                std::string file_name = path + "packed_" + this->table_name_ + "_col_" + std::to_string(i) + "_page_" + std::to_string(j) + "_" + std::to_string(party) + ".page";

                std::vector<int8_t> serialized_wire = DataUtilities::readFile(file_name);
                loaded_table->packed_buffer_pool_[i][j] = loaded_table->deserializePackedWire(serialized_wire);
                loaded_table->wire_status_[i][j] = true;
            }
        }

        return loaded_table;
    }

    void verify_loaded_table(std::string path, int party) {
        PackedColumnTable *loaded_table = load_table_from_disk(path, party);

        PackedColumnTable *expected_customer_table = getTable();
        PlainTable *expected_customer_table_revealed = expected_customer_table->revealInsecure();
        PlainTable *loaded_table_revealed = loaded_table->revealInsecure();
        ASSERT_EQ(*expected_customer_table_revealed, *loaded_table_revealed);
    }

private:
    std::string db_name_;
    std::string sql_;
    PackedColumnTable *table_;
    std::string table_name_;

    BufferPoolManager *bpm_ = SystemConfiguration::getInstance().bpm_;
    EmpManager *emp_manager_ = SystemConfiguration::getInstance().emp_manager_;
};

#endif //VAULTDB_EMP_SECRET_SHARE_AND_PACK_TPCH_DATA_FROM_QUERY_H
