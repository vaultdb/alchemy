#ifndef VAULTDB_EMP_SECRET_SHARE_AND_PACK_DATA_FROM_QUERY_H
#define VAULTDB_EMP_SECRET_SHARE_AND_PACK_DATA_FROM_QUERY_H

#include "query_table/packed_column_table.h"
#include "util/buffer_pool/buffer_pool_manager.h"

using namespace vaultdb;

class SecretShareAndPackDataFromQuery {
public:
    SecretShareAndPackDataFromQuery(std::string db_name, std::string sql, std::string table_name) : db_name_(db_name), sql_(sql), table_(
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
        // TODO: try a simpler way to pack the table without buffer pool.
        // secret share
        PlainTable *plain_input = DataUtilities::getQueryResults(db_name_, sql_, false);
        table_ = (PackedColumnTable *) plain_input->secretShare();

        // pack everything in unpacked buffer pool to packed buffer pool
        // TODO: omit buffer pool from secret sharing process - we don't need it here
        for(auto pos = bpm_->position_map_.begin(); pos != bpm_->position_map_.end(); ++pos) {
            PageId pid = pos->first;
            int slot = pos->second.slot_id_;

            emp::Bit *current_slot_ptr = bpm_->unpacked_buffer_pool_.data() + slot * bpm_->unpacked_page_size_bits_;

            emp::OMPCPackedWire packed_wire(bpm_->block_n_);
            emp_manager_->pack(current_slot_ptr, (Bit*) &packed_wire, bpm_->unpacked_page_size_bits_);
            emp::OMPCPackedWire *packed_wires_ptr = bpm_->packed_buffer_pool_[pid.table_id_][pid.col_id_] + pid.page_idx_;
            *packed_wires_ptr = packed_wire;

            bpm_->evictPage(pid);
        }

        return table_;
    }

    void save_backend_parameters(std::string path, int party) {
        if(party == TP) {
            vector<int8_t> serialized_parameters(sizeof(block), 0);
            memcpy(serialized_parameters.data(), (int8_t *) &protocol_->multi_pack_delta, sizeof(block));
            DataUtilities::writeFile(path + "multi_pack_delta.data", serialized_parameters);
        }
    }

    block load_backend_parameters(std::string path, int party) {
        if(party == TP) {
            vector<int8_t> serialized_parameters = DataUtilities::readFile(path + "multi_pack_delta.data");
            block multi_pack_delta = zero_block;
            memcpy((int8_t *) &multi_pack_delta, serialized_parameters.data(), sizeof(block));
            return multi_pack_delta;
        }
        return block();
    }

    void save_table_to_disk(std::string path, int party) {
        // save tuple cnt
        DataUtilities::writeFile(path + "tuple.cnt", to_string(this->table_->tuple_cnt_));
        // save schema
        DataUtilities::writeFile(path + this->table_name_ + ".schema", this->table_->getSchema().prettyPrint());

        // save packed buffer pool
        for(int i = -1; i < this->table_->getSchema().getFieldCount(); ++i) {
            std::string file_name = path + "packed_" + this->table_name_ + "_col_" + std::to_string(i) + "_" + std::to_string(party) + ".page";
            vector<int8_t> serialized_wires_for_col;

            for(int j = 0; j < this->table_->packed_buffer_pool_[i].size(); ++j) {
                vector<int8_t> serialized_wire = this->table_->serializePackedWire(i, j);
                serialized_wires_for_col.insert(serialized_wires_for_col.end(), serialized_wire.begin(), serialized_wire.end());
            }
            DataUtilities::writeFile(file_name, serialized_wires_for_col);
        }
    }

    QuerySchema load_schema_from_disk(std::string path) {
        return QuerySchema(DataUtilities::readTextFileToString(path + this->table_name_ + ".schema"));
    }

    PackedColumnTable *load_table_from_disk(std::string path, int party) {
        // load tuple cnt
        int tuple_cnt = std::stoi(DataUtilities::readTextFileToString(path + "tuple.cnt"));

        // load schema
        QuerySchema schema = load_schema_from_disk(path);

        // load packed buffer pool
        PackedColumnTable *loaded_table = (PackedColumnTable *) QueryTable<Bit>::getTable(tuple_cnt, schema);

        int packed_page_size_bytes = (2 * bpm_->block_n_ + 1) * sizeof(block);

        for(int i = -1; i < schema.getFieldCount(); ++i) {
            int fields_per_wire = loaded_table->fields_per_wire_[i];
            std::string file_name = path + "packed_" + this->table_name_ + "_col_" + std::to_string(i) + "_" + std::to_string(party) + ".page";
            cout << "Reading file: " << file_name << endl;
            int expected_pages =  ((tuple_cnt / fields_per_wire)  + (tuple_cnt % fields_per_wire > 0));
            cout << "Tuple count: " << tuple_cnt << " tuples per page: " << fields_per_wire << " expected pages: " << expected_pages << " expected bytes based on schema: " << expected_pages * packed_page_size_bytes << endl;
            vector<int8_t> serialized_wires_for_col = DataUtilities::readFile(file_name);
            cout << "Read in " << serialized_wires_for_col.size() << " bytes" << endl;
            cout << "Expecting " << loaded_table->packed_buffer_pool_[i].size() * packed_page_size_bytes << " bytes" << endl;
            int8_t *cursor =  serialized_wires_for_col.data();

            assert(loaded_table->packed_buffer_pool_[i].size() == (serialized_wires_for_col.size() / packed_page_size_bytes));

            for(int j = 0; j < loaded_table->packed_buffer_pool_[i].size(); ++j) {

                //std::vector<int8_t> serialized_wire = std::vector<int8_t>(serialized_wires_for_col.begin() + j * packed_page_size_bytes,
//                                                                          serialized_wires_for_col.begin() + (j + 1) * packed_page_size_bytes);
                loaded_table->packed_buffer_pool_[i][j] = loaded_table->deserializePackedWire(cursor);
                cursor += packed_page_size_bytes;
            }
        }

        return loaded_table;
    }

    void verify_loaded_table(std::string path, int party) {
        PackedColumnTable *loaded_table = load_table_from_disk(path, party);

        PackedColumnTable *expected_customer_table = getTable();
        PlainTable *expected_customer_table_revealed = expected_customer_table->revealInsecure();
        PlainTable *loaded_table_revealed = loaded_table->revealInsecure();
        assert(*expected_customer_table_revealed == *loaded_table_revealed);
    }

private:
    std::string db_name_;
    std::string sql_;
    PackedColumnTable *table_;
    std::string table_name_;

    BufferPoolManager *bpm_ = SystemConfiguration::getInstance().bpm_;
    EmpManager *emp_manager_ = SystemConfiguration::getInstance().emp_manager_;
    OMPCBackend<N> *protocol_ = (OMPCBackend<N> *) emp::backend;
};

#endif //VAULTDB_EMP_SECRET_SHARE_AND_PACK_DATA_FROM_QUERY_H