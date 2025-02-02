#ifndef VAULTDB_EMP_SECRET_SHARE_AND_PACK_DATA_FROM_QUERY_H
#define VAULTDB_EMP_SECRET_SHARE_AND_PACK_DATA_FROM_QUERY_H

#include <vector>
#include <string>
#if __has_include("emp-rescu/emp-rescu.h")

#include "query_table/packed_column_table.h"
#include "util/buffer_pool/buffer_pool_manager.h"

namespace vaultdb {
class SecretShareAndPackDataFromQuery {
public:
    SecretShareAndPackDataFromQuery(std::string db_name, std::string sql, std::string table_name) : db_name_(
            db_name), sql_(sql), table_(
            nullptr), table_name_(table_name) {}

    void setDbName(std::string db_name) {
        db_name_ = db_name;
    }

    void setSql(std::string sql) {
        sql_ = sql;
    }

    void setTableName(std::string table_name) {
        table_name_ = table_name;
    }

    PackedColumnTable *getTable() {
        // NYI: try a simpler way to pack the table without buffer pool.
        // secret share
        PlainTable *plain_input = DataUtilities::getQueryResults(db_name_, sql_, false);
        table_ = reinterpret_cast<PackedColumnTable *>(plain_input->secretShare());

        // pack everything in unpacked buffer pool to packed buffer pool
        // NYI: omit buffer pool from secret sharing process - we don't need it here
        for (auto pos = bpm_.position_map_.begin(); pos != bpm_.position_map_.end(); ++pos) {
            PageId pid = pos->first;
            int slot = pos->second.slot_id_;

            emp::Bit *current_slot_ptr = bpm_.unpacked_buffer_pool_.data() + slot * bpm_.unpacked_page_size_bits_;

            emp::OMPCPackedWire packed_wire(bpm_.block_n_);
            emp_manager_->pack(current_slot_ptr, reinterpret_cast<Bit *>(&packed_wire), bpm_.unpacked_page_size_bits_);
            table_->writePackedWire(pid, packed_wire);
        }

        return table_;
    }

    void saveBackendParameters(std::string path, int party) {
        if (party == TP) {
            vector<int8_t> serialized_parameters(sizeof(block), 0);
            memcpy(serialized_parameters.data(), reinterpret_cast<int8_t *>(&protocol_->multi_pack_delta), sizeof(block));
            DataUtilities::writeFile(path + "multi_pack_delta.data", serialized_parameters);
        }
    }

    block loadBackendParameters(std::string path, int party) {
        if (party == TP) {
            vector<int8_t> serialized_parameters = DataUtilities::readFile(path + "multi_pack_delta.data");
            block multi_pack_delta = zero_block;
            memcpy(reinterpret_cast<int8_t *>(&multi_pack_delta), serialized_parameters.data(), sizeof(block));
            return multi_pack_delta;
        }
        return block();
    }

    void saveTableToDisk(std::string path, int party) {
        // save tuple cnt
        DataUtilities::writeFile(path + "tuple.cnt", to_string(this->table_->tuple_cnt_));
        // save schema
        DataUtilities::writeFile(path + this->table_name_ + ".schema", this->table_->getSchema().prettyPrint());

        // save packed buffer pool
        for (int i = -1; i < this->table_->getSchema().getFieldCount(); ++i) {
            std::string file_name = path + "packed_" + this->table_name_ + "_col_" + std::to_string(i) + "_" +
                                    std::to_string(party) + ".page";
            vector<int8_t> serialized_wires_for_col;

            for (int j = 0; j < this->table_->packed_pages_[i].size(); ++j) {
                vector<int8_t> serialized_wire = this->table_->serializePackedWire(i, j);
                serialized_wires_for_col.insert(serialized_wires_for_col.end(), serialized_wire.begin(),
                                                serialized_wire.end());
            }
            DataUtilities::writeFile(file_name, serialized_wires_for_col);
        }
    }

    QuerySchema loadSchemaFromDisk(std::string path) {
        return QuerySchema(DataUtilities::readTextFileToString(path + this->table_name_ + ".schema"));
    }

    PackedColumnTable *loadTableFromDisk(std::string path, int party) {
        // load tuple cnt
        int tuple_cnt = std::stoi(DataUtilities::readTextFileToString(path + "tuple.cnt"));

        // load schema
        QuerySchema schema = loadSchemaFromDisk(path);

        // load packed buffer pool
        auto loaded_table = reinterpret_cast<PackedColumnTable *>(QueryTable<Bit>::getTable(tuple_cnt, schema));
        // no shares to load for TP
        if (party == SystemConfiguration::getInstance().input_party_) return loaded_table;

        for (int i = -1; i < schema.getFieldCount(); ++i) {
            std::string file_name = path + "packed_" + this->table_name_ + "_col_" + std::to_string(i) + "_" +
                                    std::to_string(party) + ".page";
            vector<int8_t> serialized_wires_for_col = DataUtilities::readFile(file_name);
            assert(loaded_table->packed_pages_[i].size() == serialized_wires_for_col.size());
            memcpy(loaded_table->packed_pages_[i].data(), serialized_wires_for_col.data(),
                   serialized_wires_for_col.size());
        }

        return loaded_table;
    }

    void verifyLoadedTable(std::string path, int party) {
        PackedColumnTable *loaded_table = loadTableFromDisk(path, party);

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

    BufferPoolManager bpm_ = SystemConfiguration::getInstance().bpm_;
    EmpManager *emp_manager_ = SystemConfiguration::getInstance().emp_manager_;
    OMPCBackend<N> *protocol_ = (OMPCBackend<N> *) emp::backend;
};
} // namespace vaultdb
#else
int main(int argc, char **argv) {
    std::cout << "emp-rescu backend not found!" << std::endl;
}
#endif
#endif //VAULTDB_EMP_SECRET_SHARE_AND_PACK_DATA_FROM_QUERY_H