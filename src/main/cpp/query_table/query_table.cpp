#include <memory>
#include <utility>
#include <util/data_utilities.h>
#include <util/field_utilities.h>
#include <util/logger.h>
#include "plain_tuple.h"
#include <operators/sort.h>
#include <util/system_configuration.h>
#include "secure_tuple.h"
#include "column_table.h"
#include "packed_column_table.h"
#include "compression/compressed_table.h"
#include "input_party_packed_column_table.h"
#include "computing_party_packed_column_table.h"
#include "util/operator_utilities.h"


using namespace vaultdb;

// iterate over all tuples and produce one long bit array for encrypting/decrypting in emp
// only tested in PUBLIC or XOR mode
template <typename B>
vector<int8_t> QueryTable<B>::serialize() {

    int dst_size = tuple_size_bytes_ * tuple_cnt_;
    vector<int8_t> dst(dst_size);
    if(dst_size == 0) return dst;

    int8_t *write_ptr = dst.data();
    int write_size;
    for(int i = 0; i < schema_.getFieldCount(); ++i) {
        write_size = field_sizes_bytes_.at(i) * tuple_cnt_;
        memcpy(write_ptr, column_data_.at(i).data(), write_size);
        write_ptr += write_size;
    }

    // dummy tag
    write_size =  field_sizes_bytes_.at(-1) * tuple_cnt_;
    memcpy(write_ptr, column_data_.at(-1).data(), write_size);
    return dst;
}




template <typename B>
QueryTable<B> & QueryTable<B>::operator=(const QueryTable<B> & s) {
    auto src = (QueryTable<B> *) &s;

    if(&s == this)
        return *this;

    setSchema(src->getSchema());

    column_data_.clear();

    for(auto pos : src->column_data_) {
        column_data_[pos.first] = pos.second;
    }

    return *this;
}

// use this for acting as a data sharing party in the PDF
// generates alice and bob's shares and returns the pair
template<typename B>
SecretShares QueryTable<B>::generateSecretShares()  {
    assert(!isEncrypted());

    vector<int8_t> serialized = serialize();
    int8_t *secrets = serialized.data();
    size_t share_size = serialized.size();

    vector<int8_t> alice_shares, bob_shares;
    alice_shares.resize(share_size);
    bob_shares.resize(share_size);
    int8_t *alice = alice_shares.data();
    int8_t *bob = bob_shares.data();

    emp::PRG prg; // initializes with a random seed
    prg.random_data(alice, share_size);

    for(size_t i = 0; i < share_size; ++i) {
        bob[i] = alice[i] ^ secrets[i];
    }

    return SecretShares(alice_shares, bob_shares);
}


template<typename B>
vector<vector<int8_t> > QueryTable<B>::generateSecretShares(const int & party_count)  {
    assert(!isEncrypted());
    emp::PRG prg; // initializes with a random seed
    vector<int8_t> serialized = serialize();
    int8_t *to_xor = serialized.data();
    size_t share_size = serialized.size();


    vector<vector<int8_t> > shares;
    shares.resize(party_count);

    for(int i = 1; i < party_count; ++i) {
        shares[i].resize(share_size);

        prg.random_data(shares[i].data(), share_size);

        for(int j = 0; j < share_size; ++j) {
            to_xor[j] ^= shares[i][j];
        }
    }

    shares[0] = serialized;
    return shares;
}



// plaintext version
template <typename B>
QueryTable<B> *QueryTable<B>::deserialize(const QuerySchema &schema, const vector<int8_t> & table_bytes, const int & limit) {
    const int8_t *read_ptr = table_bytes.data();
    bool is_plain = std::is_same_v<B, bool>;
    assert(is_plain);


    uint32_t row_size =  schema.size() / 8;
    uint32_t src_tuple_cnt =  table_bytes.size() /  row_size;
    int tuple_cnt = (limit < src_tuple_cnt || limit == -1) ? limit : src_tuple_cnt;

    auto result = QueryTable<B>::getTable(tuple_cnt, schema);
    int write_size;

    for(int i = 0; i < schema.getFieldCount(); ++i) {
        write_size = result->field_sizes_bytes_[i] * tuple_cnt;
//        result->column_data_[i].resize(write_size);
        memcpy(result->column_data_[i].data(), read_ptr, write_size);
        read_ptr += result->field_sizes_bytes_[i] * src_tuple_cnt;
    }

    // dummy tag
    write_size = result->field_sizes_bytes_[-1] * tuple_cnt;
//    result->column_data_.at(-1).resize(write_size);
    memcpy(result->column_data_.at(-1).data(), read_ptr, write_size);

    return result;

}

// only works with no wire packing
template <typename B>
QueryTable<B> *QueryTable<B>::deserialize(const QuerySchema &schema, const vector<Bit> & table_bits, const int & limit) {
    const Bit *read_ptr = table_bits.data();
    assert(SystemConfiguration::getInstance().storageModel() != StorageModel::PACKED_COLUMN_STORE);
    bool is_plain = std::is_same_v<B, bool>;
    assert(!is_plain);

    uint32_t src_tuple_cnt =  table_bits.size() /  schema.size();
    int tuple_cnt = (limit < src_tuple_cnt || limit == -1) ? limit : src_tuple_cnt;
    auto result = QueryTable<B>::getTable(tuple_cnt, schema);
    int write_size;

    for(int i = 0; i < schema.getFieldCount(); ++i) {
        write_size = result->field_sizes_bytes_[i] * tuple_cnt;
        memcpy(result->column_data_[i].data(), read_ptr, write_size);
        read_ptr += ((result->field_sizes_bytes_[i] * src_tuple_cnt) / sizeof(emp::Bit));
    }

    // dummy tag
    write_size = result->field_sizes_bytes_[-1] * tuple_cnt;
    memcpy(result->column_data_.at(-1).data(), read_ptr, write_size);
    return result;
}


// read all cols
// designed to only read bytes from file that are within limit
template<typename B>
QueryTable<B> *QueryTable<B>::deserialize(const TableMetadata  & md, const int &limit) {
    int src_tuple_cnt = md.tuple_cnt_;
    int tuple_cnt = (limit < md.tuple_cnt_ && limit > -1) ? limit : src_tuple_cnt;
    auto filename = Utilities::getFilenameForTable(md.name_);
    auto dst = QueryTable<B>::getTable(tuple_cnt, md.schema_, md.collation_);
    bool truncating = (src_tuple_cnt != tuple_cnt);

    FILE*  fp = fopen(filename.c_str(), "rb");
    for(int i = 0; i < md.schema_.getFieldCount(); ++i) {
        fread(dst->column_data_[i].data(), 1, dst->column_data_[i].size(), fp);
        if(truncating) {
            int to_seek =  dst->field_sizes_bytes_[i] * src_tuple_cnt - dst->column_data_[i].size();
            fseek(fp, to_seek, SEEK_CUR);
        }
    }

    // dummy tag
    fread(dst->column_data_[-1].data(), 1, dst->column_data_[-1].size(), fp);
    fclose(fp);

    return dst;
}



// only read columns listed in ordinals
template<typename B>
QueryTable<B> *QueryTable<B>::deserialize(const TableMetadata & md, const vector<int> & ordinals, const int & limit) {
    int src_tuple_cnt = md.tuple_cnt_;
    int tuple_cnt = (limit == -1 || limit > src_tuple_cnt) ? src_tuple_cnt : limit;
    SystemConfiguration & conf = SystemConfiguration::getInstance();

    // if no projection
    if(ordinals.empty()) {
       return deserialize(md, limit);
    }


    // else, first construct dst schema from projection
    auto dst_schema = OperatorUtilities::deriveSchema(md.schema_, ordinals);
    auto dst_collation = OperatorUtilities::deriveCollation(md.collation_, ordinals);

    auto dst = QueryTable<B>::getTable(tuple_cnt, dst_schema, dst_collation);
    // baseline for RESCU-SQL
    if(conf.emp_mode_ == EmpMode::OUTSOURCED && conf.inputParty()) {
        return dst;
    }

    // else
    // find the offsets to read in serialized file for each column
    map<int, long> ordinal_offsets;
    long array_byte_cnt = 0L;
    ordinal_offsets[0] = 0;
    for(int i = 1; i < md.schema_.getFieldCount(); ++i) {
        array_byte_cnt += dst->field_sizes_bytes_[i-1] * src_tuple_cnt;
        ordinal_offsets[i] = array_byte_cnt;
    }
    array_byte_cnt += dst->field_sizes_bytes_[md.schema_.getFieldCount()-1] * src_tuple_cnt;
    ordinal_offsets[-1] = array_byte_cnt;
    array_byte_cnt += dst->field_sizes_bytes_[-1] * src_tuple_cnt;

    auto filename = Utilities::getFilenameForTable(md.name_);
    FILE*  fp = fopen(filename.c_str(), "rb");

    int dst_ordinal = 0;
    for(auto src_ordinal : ordinals) {
        fseek(fp, ordinal_offsets[src_ordinal], SEEK_SET); // read read_offset bytes from beginning of file
        fread(dst->column_data_[dst_ordinal].data(), 1, dst->column_data_[dst_ordinal].size(), fp);
        ++dst_ordinal;
    }

    // read dummy tag unconditionally
    fseek(fp, ordinal_offsets[-1], SEEK_SET);
    fread(dst->column_data_[-1].data(),  1, dst->column_data_[-1].size(), fp);

    return dst;
}

template<typename B>
QueryTable<B> *QueryTable<B>::deserialize(const TableMetadata & md, const string & col_names_csv, const int & limit) {
    if(col_names_csv.empty()) return deserialize(md, limit);
    auto ordinals = OperatorUtilities::getOrdinalsFromColNames(md.schema_, col_names_csv);
    return QueryTable<B>::deserialize(md, ordinals, limit);
}



template<typename B>
PlainTuple QueryTable<B>::getPlainTuple(size_t idx) const {
    assert(!isEncrypted());

    PlainTuple p(const_cast<QuerySchema *>(&schema_));
    PlainTable *t = (PlainTable *) this;
    for(auto pos : field_sizes_bytes_) {
        Field f = t->getField(idx, pos.first);
        p.setField(pos.first, f);
    }
    return p;
}



template <typename B>
bool QueryTable<B>::operator==(const QueryTable<B> &other) const {

    assert(!isEncrypted()); // reveal this for tables in the clear
    assert(!other.isEncrypted());

    if(schema_ != other.getSchema()) {

        std::cout << "Failed to match on schema: \n" << schema_ << "\n  == \n" << other.getSchema() << std::endl;
        return false;
    }

    if(order_by_ != other.order_by_) {
        std::cout << "Failed to match on sort order expected="  << DataUtilities::printSortDefinition(order_by_)
                  << "observed=" << DataUtilities::printSortDefinition(other.order_by_) <<  std::endl;
        return false;
    }

    if(tuple_cnt_ != other.tuple_cnt_) {
        std::cout  << "Failed to match on tuple count " << tuple_cnt_ << " vs " << other.tuple_cnt_ << std::endl;
        return false;
    }


    auto *lhs =  (QueryTable<bool> *) this;
    auto *rhs = (QueryTable<bool> *) &other;



    for(uint32_t i = 0; i < tuple_cnt_; ++i) {
        bool lhs_dummy_tag = lhs->getDummyTag(i);
        bool rhs_dummy_tag = rhs->getDummyTag(i);

        if(lhs_dummy_tag != rhs_dummy_tag) {
            std::cout  << "Comparing on idx " << i << " failed to match on dummy tag!" << std::endl;
            return false;
        }
        else if(!lhs->getDummyTag(i)) { // they match
                for(int j = 0; j < schema_.getFieldCount(); ++j) {
                    if(lhs->getField(i, j) != rhs->getField(i, j)) {
                        PlainTuple this_tuple = getPlainTuple(i);
                        PlainTuple other_tuple = other.getPlainTuple(i);
                        std::cout << "Comparing on idx " << i << " with " << this_tuple.toString(true)
                                  << "\n          !=            " << other_tuple.toString(true) << endl;
                        std::cout << "    Failed to match!" << std::endl;
                        return false;

                    }
                }
            }
        }


    return true;

}



template <typename B>
string QueryTable<B>::toString(const bool & show_dummies) const {

    assert(!isEncrypted());
    std::ostringstream os;
    os << schema_ << " isEncrypted? " << isEncrypted() <<  " order by: " << DataUtilities::printSortDefinition(order_by_) << std::endl;

    PlainTable *plain_table = (PlainTable *) this;
    for(uint32_t i = 0; i < tuple_cnt_; ++i) {
            os <<  getPlainTuple(i).toString(show_dummies);

            if(!plain_table->getDummyTag(i)  || show_dummies)
                os << std::endl;
    }

    return os.str();

}


template<typename B>
std::string QueryTable<B>::toString(const size_t &limit, const bool &show_dummies) const {
    std::ostringstream os;
    size_t tuples_printed = 0;
    size_t cursor = 0;

    assert(!isEncrypted());

    os << schema_ <<  " order by: " << DataUtilities::printSortDefinition(order_by_) << std::endl;
    while((cursor < tuple_cnt_) && (tuples_printed < limit)) {
        PlainTuple tuple = getPlainTuple(cursor);
        if(show_dummies  // print unconditionally
           || !tuple.getDummyTag()) {
            os << cursor << ": " << tuple.toString(show_dummies) << std::endl;
            ++tuples_printed;
        }
        ++cursor;
    }

    return os.str();

}






template<typename B>
PlainTable *QueryTable<B>::reveal(const int &party) {
    {
        if(!isEncrypted()) { return (QueryTable<bool> *) this;  }
        auto table = (QueryTable<Bit> *) this;
        QuerySchema dst_schema = QuerySchema::toPlain(schema_);
        auto collation = table->order_by_;

        // if it is not sorted so that the dummies are at the end, sort it now.
        if(collation.empty() || collation[0].first != -1) {
            SortDefinition tmp{ColumnSort(-1, SortDirection::ASCENDING)};
            for(int i = 0; i < collation.size(); ++i) {
                tmp.emplace_back(collation[i]);
            }
            auto to_sort = ((QueryTable<Bit> *) this)->clone();
            Sort sort(to_sort, tmp);
            sort.setOperatorId(-2);

            table = sort.getOutput()->clone();
        }

        // count # of real (not dummy) rows
        int row_cnt = 0;
        while((row_cnt < table->tuple_cnt_) &&
              !table->getDummyTag(row_cnt).reveal()) {
            ++row_cnt;
        }

        auto dst_table = new ColumnTable<bool>(row_cnt, dst_schema, collation);

        for(int i = 0; i < row_cnt; ++i)  {
            PlainTuple dst_tuple = table->revealRow(i, party);
            dst_table->putTuple(i, dst_tuple);
        }

        if(table != (QueryTable<Bit> *) this) // extra sort
            delete table;

        return dst_table;
    }
}




template<typename B>
PlainTable *QueryTable<B>::revealInsecure(const int &party)  const {

    if(!isEncrypted()) {
        return (QueryTable<bool> *) this;
    }

    QuerySchema dst_schema = QuerySchema::toPlain(schema_);

    auto dst_table = PlainTable::getTable(tuple_cnt_, dst_schema, order_by_);

    for(int i = -1; i < schema_.getFieldCount(); ++i) {
        QueryFieldDesc field_desc = schema_.getField(i);

        for(int j = 0; j < tuple_cnt_; ++j) {
            auto f = getField(j, i);
            PlainField plain = f.reveal(field_desc, party);
            dst_table->setField(j, i, plain);
        }
    }

    return dst_table;

}



template<typename B>
QueryTable<B> *QueryTable<B>::getTable(const size_t &tuple_cnt, const QuerySchema &schema, const SortDefinition &sort_def) {
    SystemConfiguration & conf = SystemConfiguration::getInstance();
    StorageModel s = conf.storageModel();
    // SystemConfiguration::initialize mirrors this logic for determining whether to allocate the buffer pool
    // if we change this, we need to change that too
    if(s == StorageModel::PACKED_COLUMN_STORE && std::is_same_v<B, emp::Bit>) {
       if(conf.party_ == conf.input_party_)
            return (QueryTable<B> *) new InputPartyPackedColumnTable(tuple_cnt, schema, sort_def);
        else
            return (QueryTable<B> *)  new ComputingPartyPackedColumnTable(tuple_cnt, schema, sort_def);
    }
    if(s == StorageModel::COMPRESSED_STORE) {
        return new CompressedTable<B>(tuple_cnt, schema, sort_def);
    }
    return new ColumnTable<B>(tuple_cnt, schema, sort_def);
}




template class vaultdb::QueryTable<bool>;
template class vaultdb::QueryTable<emp::Bit>;
