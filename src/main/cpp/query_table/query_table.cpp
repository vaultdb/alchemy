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


using namespace vaultdb;

// iterate over all tuples and produce one long bit array for encrypting/decrypting in emp
// only tested in PUBLIC or XOR mode
template <typename B>
vector<int8_t> QueryTable<B>::serialize() const {

    int dst_size = tuple_size_bytes_ * tuple_cnt_;
    vector<int8_t> dst(dst_size);

    int8_t *write_ptr = dst.data();
    int write_size;
    cout << "Schema: " << schema_ << " size: " << schema_.size() << endl;
    cout << "Serialize writing:\n";
    for(int i = 0; i < schema_.getFieldCount(); ++i) {
        write_size = field_sizes_bytes_.at(i) * tuple_cnt_;
        cout << "  " << i << ": " << write_size << " bytes to " << write_ptr - dst.data() << "\n";
        memcpy(write_ptr, column_data_.at(i).data(), write_size);
        write_ptr += write_size;
    }

    // dummy tag
    cout << " -1: " << field_sizes_bytes_.at(-1) * tuple_cnt_ << " bytes to " << write_ptr - dst.data() << "\n";
    write_size =  field_sizes_bytes_.at(-1) * tuple_cnt_;
    memcpy(write_ptr, column_data_.at(-1).data(), write_size);
    cout << "Wrote " << dst_size << " secret-sharable bytes e2e --> bits: " << dst_size * 8 << endl;

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
SecretShares QueryTable<B>::generateSecretShares() const {
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




template <typename B>
QueryTable<B> *QueryTable<B>::deserialize(const QuerySchema &schema, const vector<int8_t> & table_bytes) {
    const int8_t *read_ptr = table_bytes.data();

    uint32_t row_size = (std::is_same_v<B, bool>) ? schema.size() / 8 : schema.size() * sizeof(emp::Bit);
    uint32_t tuple_cnt =  table_bytes.size() /  row_size;
    auto result = QueryTable<B>::getTable(tuple_cnt, schema);
    int write_size;

    for(int i = 0; i < schema.getFieldCount(); ++i) {
        write_size = result->field_sizes_bytes_[i] * tuple_cnt;
        result->column_data_[i].resize(write_size);
        memcpy(result->column_data_[i].data(), read_ptr, write_size);
        read_ptr += write_size;
    }

    // dummy tag
    write_size = result->field_sizes_bytes_[-1] * tuple_cnt;
    result->column_data_.at(-1).resize(write_size);
    memcpy(result->column_data_.at(-1).data(), read_ptr, write_size);

    return result;

}

// only works with no wire packing
template <typename B>
QueryTable<B> *QueryTable<B>::deserialize(const QuerySchema &schema, const vector<Bit> & table_bits) {
    const Bit *read_ptr = table_bits.data();
    assert(!(SystemConfiguration::getInstance().storageModel() == StorageModel::PACKED_COLUMN_STORE));
    bool is_plain = std::is_same_v<B, bool>;
    assert(!is_plain);

    uint32_t tuple_cnt =  table_bits.size() /  schema.size();
    auto result = QueryTable<B>::getTable(tuple_cnt, schema);
    int write_size;

    for(int i = 0; i < schema.getFieldCount(); ++i) {
        write_size = result->field_sizes_bytes_[i] * tuple_cnt;
        memcpy(result->column_data_[i].data(), read_ptr, write_size);
        read_ptr += (write_size / sizeof(emp::Bit));
    }

    // dummy tag
    write_size = result->field_sizes_bytes_[-1] * tuple_cnt;
    memcpy(result->column_data_.at(-1).data(), read_ptr, write_size);
    return result;
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
PlainTable *QueryTable<B>::revealInsecure(const int &party)  {

    if(!isEncrypted()) {
        return (QueryTable<bool> *) this;
    }

    QuerySchema dst_schema = QuerySchema::toPlain(schema_);

    auto dst_table = PlainTable::getTable(tuple_cnt_, dst_schema, order_by_);

    for(uint32_t i = 0; i < tuple_cnt_; ++i)  {
        PlainTuple t = revealRow(i, party);
        dst_table->putTuple(i, t);
    }
    return dst_table;

}



template<typename B>
QueryTable<B> *QueryTable<B>::getTable(const size_t &tuple_cnt, const QuerySchema &schema, const SortDefinition &sort_def) {

    StorageModel s = SystemConfiguration::getInstance().storageModel();
    // SystemConfiguration::initialize mirrors this logic for determining whether to allocate the buffer pool
    // if we change this, we need to change that too
    if(s == StorageModel::PACKED_COLUMN_STORE && std::is_same_v<B, emp::Bit>) {
            return (QueryTable<B> *)  new PackedColumnTable(tuple_cnt, schema, sort_def);
    }
    if(s == StorageModel::COMPRESSED_STORE) {
        return new CompressedTable<B>(tuple_cnt, schema, sort_def);
    }
    return new ColumnTable<B>(tuple_cnt, schema, sort_def);
}




template class vaultdb::QueryTable<bool>;
template class vaultdb::QueryTable<emp::Bit>;
