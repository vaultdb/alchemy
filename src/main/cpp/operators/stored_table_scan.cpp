#include "operators/stored_table_scan.h"
#include "util/system_configuration.h"
#include "query_table/packed_column_table.h"
#include "query_table/buffered_column_table.h"
#include <filesystem>

using namespace vaultdb;

#if  __has_include("emp-sh2pc/emp-sh2pc.h") || __has_include("emp-zk/emp-zk.h")

template<typename B>
QueryTable<B> *StoredTableScan<B>::readStoredTable(string table_name, const vector<int> & col_ordinals, const int & limit) {
    SystemConfiguration & conf = SystemConfiguration::getInstance();
    assert(conf.table_metadata_.find(table_name) != conf.table_metadata_.end());
    TableMetadata md = conf.table_metadata_.at(table_name);

        if(conf.storageModel() == StorageModel::PACKED_COLUMN_STORE) {
            return (QueryTable<B> *) PackedColumnTable::deserialize(md, col_ordinals, limit);
        }
        else {
            return readSecretSharedStoredTable(table_name, col_ordinals, limit);
        }


}

// these are outside of ColumnTable::deserialize because we are using those methods for pilot-related things
template<typename B>
QueryTable<B> *StoredTableScan<B>::readSecretSharedStoredTable(string table_name, const vector<int> & col_ordinals, const int & limit) {
    if(col_ordinals.empty()) {
        return readSecretSharedStoredTable(table_name, limit);
    }

    SystemConfiguration & conf = SystemConfiguration::getInstance();
    assert(conf.table_metadata_.find(table_name) != conf.table_metadata_.end());
    TableMetadata md = conf.table_metadata_.at(table_name);

    // else, first construct dst schema from projection
    auto dst_schema = OperatorUtilities::deriveSchema(md.schema_, col_ordinals);
    auto dst_collation = OperatorUtilities::deriveCollation(md.collation_, col_ordinals);

    QuerySchema secure_schema = QuerySchema::toSecure(md.schema_);
    QuerySchema plain_schema = QuerySchema::toPlain(md.schema_);

    size_t src_tuple_cnt = md.tuple_cnt_;
    size_t tuple_cnt = (limit == -1 || limit > src_tuple_cnt) ? src_tuple_cnt : limit;

    bool *dst_bools;
    size_t dst_bit_cnt = tuple_cnt * dst_schema.size();

    if(!conf.inputParty()) {
        string secret_shares_file = SystemConfiguration::getInstance().stored_db_path_ + "/" + table_name + "." +
                                    std::to_string(SystemConfiguration::getInstance().party_);

        // deduce row count
        size_t src_byte_cnt = std::filesystem::file_size(secret_shares_file);
        size_t src_bit_cnt = src_byte_cnt * 8;


        map<int, int64_t> ordinal_offsets;
        int64_t array_byte_cnt = 0L;
        ordinal_offsets[0] = 0;
        for (int i = 1; i < md.schema_.getFieldCount(); ++i) {
            array_byte_cnt += (md.schema_.getField(i - 1).size() * src_tuple_cnt)/8;
            ordinal_offsets[i] = array_byte_cnt;
        }
        array_byte_cnt += (md.schema_.getField(md.schema_.getFieldCount() - 1).size() * src_tuple_cnt)/8;
        ordinal_offsets[-1] = array_byte_cnt;
        array_byte_cnt += md.schema_.getField(-1).size() * src_tuple_cnt;

        // convert serialized representation from byte-aligned to bit-by-bit for EMP
        dst_bools = new bool[dst_bit_cnt]; // dst_bit_alloc
        bool *dst_cursor = dst_bools;

        assert(src_bit_cnt % plain_schema.size() == 0);
        FILE *fp = fopen(secret_shares_file.c_str(), "rb");

       for (auto src_ordinal: col_ordinals) {
            int read_size_bits = tuple_cnt * secure_schema.getField(src_ordinal).size();
            fseek(fp, ordinal_offsets[src_ordinal], SEEK_SET); // read read_offset bytes from beginning of file
            if (plain_schema.getField(src_ordinal).getType() != FieldType::BOOL) {
                vector<int8_t> tmp_read(read_size_bits / 8);
                fread(tmp_read.data(), 1, read_size_bits / 8, fp);
                emp::to_bool<int8_t>(dst_cursor, tmp_read.data(), read_size_bits, false);
                dst_cursor += read_size_bits;
            } else { // just copy it in
                fread(dst_cursor, 1, read_size_bits, fp);
                for(int j = 0; j < tuple_cnt; ++j) {
                    *dst_cursor = ((*dst_cursor & 1) != 0); // (bool)
                    ++dst_cursor;
                }
            }
        }

        // read dummy tag unconditionally
        fseek(fp, ordinal_offsets[-1], SEEK_SET);
        fread(dst_cursor, 1, tuple_cnt, fp); // 1 byte / row
        for(int j = 0; j < tuple_cnt; ++j) {
            *dst_cursor = ((*dst_cursor & 1) != 0); // (bool)
            ++dst_cursor;
        }

        fclose(fp);
    }


    Integer dst(dst_bit_cnt, 0L, emp::PUBLIC);

    int party = SystemConfiguration::getInstance().party_;
    EmpManager *manager = SystemConfiguration::getInstance().emp_manager_;
    bool *to_send = (party == 1) ? dst_bools : nullptr;
    manager->feed(dst.bits.data(), 1, to_send, dst_bit_cnt);

    for(int i = 2; i <= N; ++i) {
        Integer tmp(dst_bit_cnt, 0L, emp::PUBLIC);
        to_send = (party == i) ? dst_bools : nullptr;
        manager->feed(tmp.bits.data(), i, to_send, dst_bit_cnt);
        dst = dst ^ tmp;
    }

    SecureTable *shared_table = QueryTable<Bit>::deserialize(dst_schema, dst.bits);
    shared_table->order_by_ = md.collation_;

    if(!conf.inputParty()) delete [] dst_bools;
    return (QueryTable<B> *) shared_table;
}

// read all ordinals
template<typename B>
QueryTable<B> *StoredTableScan<B>::readSecretSharedStoredTable(string table_name, const int & limit) {
    SystemConfiguration & conf = SystemConfiguration::getInstance();
    assert(conf.table_metadata_.find(table_name) != conf.table_metadata_.end());
    TableMetadata md = conf.table_metadata_.at(table_name);

    QuerySchema secure_schema = QuerySchema::toSecure(md.schema_);
    QuerySchema plain_schema = QuerySchema::toPlain(md.schema_);

    size_t src_tuple_cnt = md.tuple_cnt_;
    size_t tuple_cnt = (limit == -1 || limit > src_tuple_cnt) ? src_tuple_cnt : limit;

    bool *dst_bools;
    size_t dst_bit_cnt = tuple_cnt * secure_schema.size();


    if(!conf.inputParty()) {
        string secret_shares_file = SystemConfiguration::getInstance().stored_db_path_ + "/" + table_name + "." +
                                    std::to_string(SystemConfiguration::getInstance().party_);

        FILE*  fp = fopen(secret_shares_file.c_str(), "rb");

        // deduce row count
        size_t src_byte_cnt = std::filesystem::file_size(secret_shares_file);
        size_t src_bit_cnt = src_byte_cnt * 8;
        bool truncating = (src_tuple_cnt != tuple_cnt);

        // convert serialized representation from byte-aligned to bit-by-bit
        dst_bit_cnt = tuple_cnt * secure_schema.size();
        dst_bools = new bool[dst_bit_cnt]; // dst_bit_alloc
        bool *dst_cursor = dst_bools;

        assert(src_bit_cnt % plain_schema.size() == 0);

        for (int i = 0; i < plain_schema.getFieldCount(); ++i) {
            auto plain_field = plain_schema.getField(i);
            auto secure_field = secure_schema.getField(i);
            if (plain_field.getType() != FieldType::BOOL) { // 1:1, just serialize it
                int size_bits = secure_field.size() * tuple_cnt;
                vector<int8_t> tmp_read(size_bits / 8);
                fread(tmp_read.data(), 1, tmp_read.size(), fp);
                emp::to_bool<int8_t>(dst_cursor, tmp_read.data(), size_bits, false);
                dst_cursor += size_bits;
            } else { // just copy it in
                fread(dst_cursor, 1, tuple_cnt, fp); // 1 bool per tuple
                for(int j = 0; j < tuple_cnt; ++j) {
                    *dst_cursor = ((*dst_cursor & 1) != 0); // (bool)
                    ++dst_cursor;
                } // end for each tuple
            }  // end else for bool check

            if(truncating) {
                int to_seek =  ((src_tuple_cnt - tuple_cnt) * plain_field.size())/ 8;
                fseek(fp, to_seek, SEEK_CUR);
            }
        } // end for all fields

        fread(dst_cursor, 1, tuple_cnt, fp);
        for (int i = 0; i < tuple_cnt; ++i) {
            *dst_cursor = ((*dst_cursor & 1) != 0);
            ++dst_cursor;
        }

    }

    Integer dst(dst_bit_cnt, 0L, emp::PUBLIC);

    int party = SystemConfiguration::getInstance().party_;
    EmpManager *manager = SystemConfiguration::getInstance().emp_manager_;
    bool *to_send = (party == 1) ? dst_bools : nullptr;
    manager->feed(dst.bits.data(), 1, to_send, dst_bit_cnt);

    for(int i = 2; i <= N; ++i) {
        Integer tmp(dst_bit_cnt, 0L, emp::PUBLIC);
        to_send = (party == i) ? dst_bools : nullptr;
        manager->feed(tmp.bits.data(), i, to_send, dst_bit_cnt);
        dst = dst ^ tmp;
    }

    SecureTable *shared_table = QueryTable<Bit>::deserialize(secure_schema, dst.bits);

    if(!conf.inputParty()) delete [] dst_bools;
    shared_table->order_by_ = md.collation_;
    return (QueryTable<B> *) shared_table;
}

// read secret shares from disk for BufferedColumnTable
template<typename B>
QueryTable<B> *StoredTableScan<B>::readSecretSharesForBufferedColumnTable(const TableMetadata & md, const vector<int> &col_ordinals, const int &limit) {
    return QueryTable<B>::getTable(0, QuerySchema(), SortDefinition());
}

#else
#include "util/emp_manager/outsourced_mpc_manager.h"

template<typename B>
QueryTable<B> *StoredTableScan<B>::readStoredTable(string table_name, const vector<int> & col_ordinals, const int & limit) {
    SystemConfiguration & conf = SystemConfiguration::getInstance();
    assert(conf.table_metadata_.find(table_name) != conf.table_metadata_.end());
    TableMetadata md = conf.table_metadata_.at(table_name);

    if(conf.storageModel() == StorageModel::PACKED_COLUMN_STORE) {
        return (QueryTable<B> *) PackedColumnTable::deserialize(md, col_ordinals, limit);
    }
    else if(conf.storageModel() == StorageModel::COLUMN_STORE && conf.bp_enabled_) {
        return readSecretSharesForBufferedColumnTable(md, col_ordinals, limit);
    }
    else {
        return readSecretSharedStoredTable(table_name, col_ordinals, limit);
    }


}

// these are outside of ColumnTable::deserialize because we are using those methods for pilot-related things
template<typename B>
QueryTable<B> *StoredTableScan<B>::readSecretSharedStoredTable(string table_name, const vector<int> & col_ordinals, const int & limit) {
    if(col_ordinals.empty()) {
        return readSecretSharedStoredTable(table_name, limit);
    }

    SystemConfiguration & conf = SystemConfiguration::getInstance();
    assert(conf.table_metadata_.find(table_name) != conf.table_metadata_.end());
    TableMetadata md = conf.table_metadata_.at(table_name);

    // else, first construct dst schema from projection
    QuerySchema dst_schema = OperatorUtilities::deriveSchema(md.schema_, col_ordinals);
    SortDefinition dst_collation = OperatorUtilities::deriveCollation(md.collation_, col_ordinals);

    QuerySchema secure_schema = QuerySchema::toSecure(md.schema_);
    QuerySchema plain_schema = QuerySchema::toPlain(md.schema_);

    int src_tuple_cnt = md.tuple_cnt_;
    int tuple_cnt = (limit == -1 || limit > src_tuple_cnt) ? src_tuple_cnt : limit;

    int dst_bit_cnt = tuple_cnt * dst_schema.size();
    Integer dst_int(dst_bit_cnt, 0L, emp::PUBLIC);

    int currernt_emp_bit_size_on_disk = (conf.party_ == 1 ? empBitSizesInPhysicalBytes::evaluator_disk_size_ : (conf.party_ == 10086 ? 1 : empBitSizesInPhysicalBytes::garbler_disk_size_));

    map<int, int64_t> ordinal_offsets;
    int64_t col_bytes_cnt = 0L;
    ordinal_offsets[0] = 0;
    for (int i = 1; i < md.schema_.getFieldCount(); ++i) {
        col_bytes_cnt += md.schema_.getField(i - 1).size() * src_tuple_cnt * currernt_emp_bit_size_on_disk;
        ordinal_offsets[i] = col_bytes_cnt;
    }
    col_bytes_cnt += md.schema_.getField(md.schema_.getFieldCount() - 1).size() * src_tuple_cnt * currernt_emp_bit_size_on_disk;
    ordinal_offsets[-1] = col_bytes_cnt;

    auto *auth_shares = new AuthShare<emp::N>[dst_bit_cnt];
    bool *masked_values = new bool[dst_bit_cnt]();

    string secret_shares_file = SystemConfiguration::getInstance().stored_db_path_ + "/" + table_name + "." + std::to_string(conf.party_);

    FILE*  fp = fopen(secret_shares_file.c_str(), "rb");
    int bit_cursor = 0;

    for(auto src_ordinal : col_ordinals) {
        auto secure_field = secure_schema.getField(src_ordinal);
        int read_size_bits = tuple_cnt * secure_field.size();

        fseek(fp, ordinal_offsets[src_ordinal], SEEK_SET);

        for(int j = 0; j < read_size_bits; ++j) {
            AuthShare<emp::N> cur_auth_share;

            if(!conf.inputParty()) {
                fread((int8_t *) &cur_auth_share.mac, 1, emp::N * sizeof(emp::block), fp);
                fread((int8_t *) &cur_auth_share.key, 1, emp::N * sizeof(emp::block), fp);
            }

            int8_t cur_lambda = 0;
            fread((int8_t *) &cur_lambda, 1, 1, fp);
            cur_auth_share.lambda = ((cur_lambda & 1) != 0);

            auth_shares[bit_cursor] = cur_auth_share;

            if(conf.party_ == 1) {
                int8_t cur_masked_value = 0;
                fread((int8_t *) &cur_masked_value, 1, 1, fp);
                masked_values[bit_cursor] = ((cur_masked_value & 1) != 0);
            }

            ++bit_cursor;
        }
    }

    // read dummy tag
    fseek(fp, ordinal_offsets[-1], SEEK_SET);

    for(int i = 0; i < tuple_cnt; ++i) {
        AuthShare<emp::N> cur_auth_share;

        if(!conf.inputParty()) {
            fread((int8_t *) &cur_auth_share.mac, 1, emp::N * sizeof(emp::block), fp);
            fread((int8_t *) &cur_auth_share.key, 1, emp::N * sizeof(emp::block), fp);
        }

        int8_t cur_lambda = 0;
        fread((int8_t *) &cur_lambda, 1, 1, fp);
        cur_auth_share.lambda = ((cur_lambda & 1) != 0);

        auth_shares[dst_bit_cnt - tuple_cnt + i] = cur_auth_share;

        if(conf.party_ == 1) {
            int8_t cur_masked_value = 0;
            fread((int8_t *) &cur_masked_value, 1, 1, fp);
            masked_values[dst_bit_cnt - tuple_cnt + i] = ((cur_masked_value & 1) != 0);
        }
    }

    fclose(fp);

    ((OutsourcedMpcManager *) conf.emp_manager_)->protocol_->regen_label(dst_int.bits.data(), masked_values, auth_shares, dst_bit_cnt);

    delete [] auth_shares;
    delete [] masked_values;

    SecureTable *shared_table = QueryTable<Bit>::deserialize(dst_schema, dst_int.bits);
    shared_table->order_by_ = dst_collation;

    return (QueryTable<B> *) shared_table;
}

// read all ordinals
template<typename B>
QueryTable<B> *StoredTableScan<B>::readSecretSharedStoredTable(string table_name, const int & limit) {
    SystemConfiguration & conf = SystemConfiguration::getInstance();
    assert(conf.table_metadata_.find(table_name) != conf.table_metadata_.end());
    TableMetadata md = conf.table_metadata_.at(table_name);

    QuerySchema secure_schema = QuerySchema::toSecure(md.schema_);
    QuerySchema plain_schema = QuerySchema::toPlain(md.schema_);

    int src_tuple_cnt = (int) md.tuple_cnt_;
    int tuple_cnt = (limit == -1 || limit > src_tuple_cnt) ? src_tuple_cnt : limit;

    int dst_bit_cnt = tuple_cnt * secure_schema.size();
    Integer dst_int(dst_bit_cnt, 0L, emp::PUBLIC);

    auto *auth_shares = new AuthShare<emp::N>[dst_bit_cnt];
    bool *masked_values = new bool[dst_bit_cnt]();

    string secret_shares_file = SystemConfiguration::getInstance().stored_db_path_ + "/" + table_name + "." +
                                std::to_string(conf.party_);

    FILE*  fp = fopen(secret_shares_file.c_str(), "rb");
    int file_offset = 0;
    int currernt_emp_bit_size_on_disk = (conf.party_ == 1 ? empBitSizesInPhysicalBytes::evaluator_disk_size_ : (conf.party_ == 10086 ? 1 : empBitSizesInPhysicalBytes::garbler_disk_size_));

    int bit_cursor = 0;
    // read col by col
    for(int i = 0; i < secure_schema.getFieldCount(); ++i) {
        auto secure_field = secure_schema.getField(i);
        int read_size_bits = tuple_cnt * secure_field.size();

        // set to beginning of each column
        fseek(fp, file_offset, SEEK_SET);

        // read given size for each column
        for(int j = 0; j < read_size_bits; ++j) {
            AuthShare<emp::N> cur_auth_share;

            if(!conf.inputParty()) {
                fread((int8_t *) &cur_auth_share.mac, 1, emp::N * sizeof(emp::block), fp);
                fread((int8_t *) &cur_auth_share.key, 1, emp::N * sizeof(emp::block), fp);
            }

            int8_t cur_lambda = 0;
            fread((int8_t *) &cur_lambda, 1, 1, fp);
            cur_auth_share.lambda = ((cur_lambda & 1) != 0);

            auth_shares[bit_cursor] = cur_auth_share;

            if(conf.party_ == 1) {
                int8_t cur_masked_value = 0;
                fread((int8_t *) &cur_masked_value, 1, 1, fp);
                masked_values[bit_cursor] = ((cur_masked_value & 1) != 0);
            }

            ++bit_cursor;
        }

        file_offset += src_tuple_cnt * secure_field.size() * currernt_emp_bit_size_on_disk;
    }

    // read dummy tag
    fseek(fp, file_offset, SEEK_SET);
    for(int i = 0; i < tuple_cnt; ++i) {
        AuthShare<emp::N> cur_auth_share;

        if(!conf.inputParty()) {
            fread((int8_t *) &cur_auth_share.mac, 1, emp::N * sizeof(emp::block), fp);
            fread((int8_t *) &cur_auth_share.key, 1, emp::N * sizeof(emp::block), fp);
        }

        int8_t cur_lambda = 0;
        fread((int8_t *) &cur_lambda, 1, 1, fp);
        cur_auth_share.lambda = ((cur_lambda & 1) != 0);

        auth_shares[dst_bit_cnt - tuple_cnt + i] = cur_auth_share;

        if(conf.party_ == 1) {
            int8_t cur_masked_value = 0;
            fread((int8_t *) &cur_masked_value, 1, 1, fp);
            masked_values[dst_bit_cnt - tuple_cnt + i] = ((cur_masked_value & 1) != 0);
        }
    }

    fclose(fp);

    ((OutsourcedMpcManager *) conf.emp_manager_)->protocol_->regen_label(dst_int.bits.data(), masked_values, auth_shares, dst_bit_cnt);

    delete [] auth_shares;
    delete [] masked_values;

    SecureTable *shared_table = QueryTable<Bit>::deserialize(secure_schema, dst_int.bits);
    shared_table->order_by_ = md.collation_;

    return (QueryTable<B> *) shared_table;
}

// read secret shares from disk for BufferedColumnTable
template<typename B>
QueryTable<B> *StoredTableScan<B>::readSecretSharesForBufferedColumnTable(const TableMetadata  & md, const vector<int> &col_ordinals, const int &limit) {
    SystemConfiguration & conf = SystemConfiguration::getInstance();

    bool not_limit = (limit == -1 || limit > md.tuple_cnt_);
    int tuple_cnt = not_limit ? md.tuple_cnt_ : limit;
    QuerySchema secure_schema = col_ordinals.empty() ? QuerySchema::toSecure(md.schema_) : QuerySchema::toSecure(OperatorUtilities::deriveSchema(md.schema_, col_ordinals));
    BufferedColumnTable *buffered_table = new BufferedColumnTable(tuple_cnt, secure_schema, md.collation_);

    string src_data_path = conf.stored_db_path_ + "/" + md.name_ + "." + std::to_string(conf.party_);
    if(!col_ordinals.empty() || !not_limit) {
        for(int i = 0; col_ordinals.empty() ? i < md.schema_.getFieldCount() : i < col_ordinals.size(); ++i) {
            int max_page = tuple_cnt / buffered_table->fields_per_page_[i] + (tuple_cnt % buffered_table->fields_per_page_[i] != 0);

            for(int j = 0; j < max_page; ++j) {
                PageId pid(buffered_table->table_id_, i, j);
                std::vector<emp::Bit> secret_shares = buffered_table->readSecretSharedPageFromDisk(pid, md.tuple_cnt_, md.schema_, col_ordinals.empty() ? i : col_ordinals[i], src_data_path);
                std::vector<int8_t> write_buffer = buffered_table->convertEMPBitToWriteBuffer(secret_shares);
                DataUtilities::appendFile(buffered_table->secret_shares_path_, write_buffer);
            }
        }

        int max_dummy_page = tuple_cnt / buffered_table->fields_per_page_.at(-1) + ((tuple_cnt % buffered_table->fields_per_page_.at(-1)) != 0);

        for(int i = 0; i < max_dummy_page; ++i) {
            PageId pid(buffered_table->table_id_, -1, i);
            std::vector<emp::Bit> secret_shares = buffered_table->readSecretSharedPageFromDisk(pid, md.tuple_cnt_, md.schema_, -1, src_data_path);

            std::vector<int8_t> write_buffer = buffered_table->convertEMPBitToWriteBuffer(secret_shares);

            DataUtilities::appendFile(buffered_table->secret_shares_path_, write_buffer);
        }
    }
    else {
        std::filesystem::copy_file(src_data_path, buffered_table->secret_shares_path_, std::filesystem::copy_options::overwrite_existing);
    }

    return (QueryTable<B> *) buffered_table;
}


#endif

template class vaultdb::StoredTableScan<bool>;
template class vaultdb::StoredTableScan<emp::Bit>;
