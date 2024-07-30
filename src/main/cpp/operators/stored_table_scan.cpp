#include "stored_table_scan.h"
#include "util/system_configuration.h"
#include "query_table/packed_column_table.h"
#include <filesystem>

using namespace vaultdb;

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
    size_t tuple_cnt;
    bool *dst_bools;
    size_t dst_bit_cnt;

    if(!conf.inputParty()) {
        string secret_shares_file = SystemConfiguration::getInstance().stored_db_path_ + "/" + table_name + "." +
                                    std::to_string(SystemConfiguration::getInstance().party_);

        // deduce row count
        size_t src_byte_cnt = std::filesystem::file_size(secret_shares_file);
        size_t src_bit_cnt = src_byte_cnt * 8;
        int src_tuple_cnt = src_bit_cnt / plain_schema.size();
        int tuple_cnt = (limit == -1 || limit > src_tuple_cnt) ? src_tuple_cnt : limit;

        map<int, long> ordinal_offsets;
        long array_byte_cnt = 0L;
        ordinal_offsets[0] = 0;
        for (int i = 1; i < md.schema_.getFieldCount(); ++i) {
            array_byte_cnt += md.schema_.getField(i - 1).size() * src_tuple_cnt;
            ordinal_offsets[i] = array_byte_cnt;
        }
        array_byte_cnt += md.schema_.getField(md.schema_.getFieldCount() - 1).size() * src_tuple_cnt;
        ordinal_offsets[-1] = array_byte_cnt;
        array_byte_cnt += md.schema_.getField(-1).size() * src_tuple_cnt;

        // convert serialized representation from byte-aligned to bit-by-bit for EMP
        dst_bit_cnt = tuple_cnt * dst_schema.size();
        dst_bools = new bool[dst_bit_cnt]; // dst_bit_alloc
        bool *dst_cursor = dst_bools;

        assert(src_bit_cnt % plain_schema.size() == 0);
        FILE *fp = fopen(secret_shares_file.c_str(), "rb");

        int dst_ordinal = 0;
        for (auto src_ordinal: col_ordinals) {
            int read_size_bits = tuple_cnt * plain_schema.getField(src_ordinal).size();
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
    else { // TP
        tuple_cnt = md.tuple_cnt_;
        dst_bit_cnt = tuple_cnt * dst_schema.size();
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

    size_t tuple_cnt;
    bool *dst_bools;
    size_t dst_bit_cnt;

    if(!conf.inputParty()) {
        string secret_shares_file = SystemConfiguration::getInstance().stored_db_path_ + "/" + table_name + "." +
                                    std::to_string(SystemConfiguration::getInstance().party_);

        FILE*  fp = fopen(secret_shares_file.c_str(), "rb");

        // deduce row count
        size_t src_byte_cnt = std::filesystem::file_size(secret_shares_file);
        size_t src_bit_cnt = src_byte_cnt * 8;
        int src_tuple_cnt = src_bit_cnt / plain_schema.size();
        tuple_cnt = (limit == -1 || limit > src_tuple_cnt) ? src_tuple_cnt : limit;
        bool truncating = (src_tuple_cnt != tuple_cnt);

        // convert serialized representation from byte-aligned to bit-by-bit
        dst_bit_cnt = tuple_cnt * secure_schema.size();
        //    size_t dst_bit_alloc = dst_bit_cnt + (dst_bit_cnt % 128);  // pad it to 128-bit increments for emp
        dst_bools = new bool[dst_bit_cnt]; // dst_bit_alloc
        bool *dst_cursor = dst_bools;

        assert(src_bit_cnt % plain_schema.size() == 0);

        for (int i = 0; i < plain_schema.getFieldCount(); ++i) {
            auto plain_field = plain_schema.getField(i);
            auto secure_field = secure_schema.getField(i);

            if (plain_field.size() == secure_field.size()) { // 1:1, just serialize it
                int write_size_bits = secure_field.size() * tuple_cnt;
                vector<int8_t> tmp_read(write_size_bits / 8);
                fread(tmp_read.data(), 1, tmp_read.size(), fp);
                emp::to_bool<int8_t>(dst_cursor, tmp_read.data(), write_size_bits, false);
                dst_cursor += write_size_bits;
            } else { // just copy it in
                int size_bits = secure_schema.getField(i).size() * tuple_cnt;
                fread(dst_cursor, 1, size_bits, fp);
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

        // copy dummy tag
        int size_bits = secure_schema.getField(-1).size() * tuple_cnt;
        fread(dst_cursor, 1, size_bits, fp);
        for (int i = 0; i < tuple_cnt; ++i) {
            *dst_cursor = ((*dst_cursor & 1) != 0);
            ++dst_cursor;
        }

    }
    else {
        tuple_cnt = md.tuple_cnt_;
        dst_bit_cnt = tuple_cnt * secure_schema.size();
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

template class vaultdb::StoredTableScan<bool>;
template class vaultdb::StoredTableScan<emp::Bit>;
