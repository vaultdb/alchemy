#include "stored_table_scan.h"
#include "util/system_configuration.h"
#include "query_table/packed_column_table.h"

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
    SystemConfiguration & conf = SystemConfiguration::getInstance();
    assert(conf.table_metadata_.find(table_name) != conf.table_metadata_.end());
    TableMetadata md = conf.table_metadata_.at(table_name);

    QuerySchema secure_schema = QuerySchema::toSecure(md.schema_);
    QuerySchema plain_schema = QuerySchema::toPlain(md.schema_);
    size_t tuple_cnt;
    vector<int8_t> src_data;
    bool *dst_bools;
    size_t dst_bit_cnt;

    if(!conf.inputParty()) {
        string secret_shares_file = SystemConfiguration::getInstance().stored_db_path_ + "/" + table_name + "." +
                                    std::to_string(SystemConfiguration::getInstance().party_);
        src_data = DataUtilities::readFile(secret_shares_file);

        // deduce row count
        size_t src_byte_cnt = src_data.size();
        size_t src_bit_cnt = src_byte_cnt * 8;
        tuple_cnt = src_bit_cnt / plain_schema.size();

        // convert serialized representation from byte-aligned to bit-by-bit
        dst_bit_cnt = tuple_cnt * secure_schema.size();
        //    size_t dst_bit_alloc = dst_bit_cnt + (dst_bit_cnt % 128);  // pad it to 128-bit increments for emp
        dst_bools = new bool[dst_bit_cnt]; // dst_bit_alloc
        bool *dst_cursor = dst_bools;
        int8_t *src_cursor = src_data.data();

        assert(src_bit_cnt % plain_schema.size() == 0);
        // need to "flatten out" bools
        for (int i = 0; i < plain_schema.getFieldCount(); ++i) {
            auto plain_field = plain_schema.getField(i);
            auto secure_field = secure_schema.getField(i);

            if (plain_field.size() == secure_field.size()) { // 1:1, just serialize it
                int write_size = secure_schema.getField(i).size() * tuple_cnt;
                emp::to_bool<int8_t>(dst_cursor, src_cursor, write_size, false);
                dst_cursor += write_size;
                src_cursor += write_size / 8;
            } else {
                assert(plain_field.size() == (secure_field.size() + 7)); // only for bools
                for (int j = 0; j < tuple_cnt; ++j) {
                    *dst_cursor = ((*src_cursor & 1) != 0); // (bool) *src_cursor;
                    ++dst_cursor;
                    ++src_cursor;
                }
            } // end for each tuple
        } // end for all fields

        // copy dummy tag
        for (int i = 0; i < tuple_cnt; ++i) {
            *dst_cursor = ((*src_cursor & 1) != 0);
            ++dst_cursor;
            ++src_cursor;
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
