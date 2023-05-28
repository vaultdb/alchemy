#ifndef QUERY_TABLE_H
#define QUERY_TABLE_H


#include "query_schema.h"
#include "query_tuple.h"
#include <memory>
#include <ostream>
#include "util/utilities.h"
#include "plain_tuple.h"
#include <emp-tool/emp-tool.h>
#include <emp-zk/emp-zk.h>


namespace  vaultdb {
    template<typename B>
    class QueryTableIterator;

    typedef std::pair<std::vector<int8_t>, std::vector<int8_t> > SecretShares;

    template<typename B> class QueryTable;

    typedef QueryTable<bool> PlainTable;
    typedef QueryTable<emp::Bit> SecureTable;


    template<typename B>
    class QueryTable {

        friend class QueryTableIterator<B>;


    private:
        SortDefinition order_by_;
        std::shared_ptr<QuerySchema> schema_;

    public:
        using iterator = QueryTableIterator<B>;

        std::vector<int8_t> tuple_data_;
        // size of each tuple in bytes
        size_t tuple_size_;
        size_t tuple_cnt_;

        // empty sort definition for default case
        QueryTable(const size_t &num_tuples, const QuerySchema &schema,
                   const SortDefinition &sortDefinition = SortDefinition());

        // deep copy
        QueryTable(const QueryTable &src);

        ~QueryTable() {}

        void resize(const size_t &tupleCount);

        bool isEncrypted() const;

        inline void setSchema(const QuerySchema &schema) {
            schema_ = std::make_shared<QuerySchema>(schema);
            tuple_size_ = schema_->size()/8; // bytes for plaintext

            if(std::is_same_v<emp::Bit, B>) {
                size_t tuple_bits = schema_->size();
                tuple_size_ = tuple_bits * TypeUtilities::getEmpBitSize(); // bits, one block per bit
            }

        }

        inline const std::shared_ptr<QuerySchema> getSchema() const { return schema_; }

        QueryTuple<B> getTuple(int idx);

        const QueryTuple<B> getImmutableTuple(int idx) const;

        inline size_t getTupleCount() const { return tuple_cnt_; }

        std::string toString(const bool &showDummies = false) const;

        std::string toString(const size_t &limit, const bool &showDummies = false) const;

        void putTuple(const int &idx, const QueryTuple<B> &tuple);


        void setSortOrder(const SortDefinition &sortOrder);

        SortDefinition getSortOrder() const;


        std::vector<int8_t> serialize() const;

        Field<B> getField(const int  & row, const int & col) const;


        static std::shared_ptr<SecureTable> secretShare(const PlainTable &input, emp::NetIO *io, const int &party);

        static std::shared_ptr<SecureTable>
        secretShare(const PlainTable &input, emp::BoolIO<NetIO> *ios[], const size_t &thread_count, const int &party);

        static std::shared_ptr<SecureTable>
        secret_share_send_table(const std::shared_ptr<PlainTable> &input, emp::NetIO *io, const int &sharing_party);

        static std::shared_ptr<SecureTable>
        secret_share_recv_table(const QuerySchema &src_schema,
                                const SortDefinition &sortDefinition, emp::NetIO *io, const int &sharing_party);


        SecretShares
        generateSecretShares() const; // generate shares for alice and bob - for data sharing (non-computing) node

        std::unique_ptr<QueryTable<bool> > reveal(int empParty = emp::PUBLIC) const;

        QueryTable<B> &operator=(const QueryTable<B> &src);

        bool operator==(const QueryTable<B> &other) const;

        bool operator!=(const QueryTable &other) const { return !(*this == other); }

        QueryTuple<B> operator[](const int &idx);

        const QueryTuple<B> operator[](const int &idx) const;

        static std::shared_ptr<PlainTable> deserialize(const QuerySchema &schema, const vector<int8_t> &tableBits);

        // encrypted version of deserialization using emp::Bit
        static std::shared_ptr<SecureTable> deserialize(const QuerySchema &schema, vector<Bit> &table_bits);

        size_t getTrueTupleCount() const;

        bool empty() const { return tuple_data_.empty(); }

        QueryTuple<bool> getPlainTuple(size_t idx) const;

        inline QueryTableIterator<B> begin() {   return QueryTableIterator<B>(*this); }
        inline QueryTableIterator<B> end() { return QueryTableIterator<B>( *this, tuple_cnt_); }



    private:
        static std::unique_ptr<PlainTable> revealTable(const SecureTable &table, const int &party);

        static std::unique_ptr<PlainTable> revealTable(const PlainTable &table, const int &party);

        static void secret_share_send(const int &party, const PlainTable &src_table, SecureTable &dst_table,
                                      const int &write_offset,
                                      const bool &reverse_read_order);

        static void secret_share_recv(const size_t &tuple_count, const int &dst_party,
                                      SecureTable &dst_table, const size_t &write_offset,
                                      const bool &reverse_read_order);

    };


    std::ostream &operator<<(std::ostream &os, const PlainTable &table);

    std::ostream &operator<<(std::ostream &os, const SecureTable &table);


    template<typename B>
    class QueryTableIterator {
        using iterator_category = std::forward_iterator_tag;
        using difference_type   = std::ptrdiff_t;
        using value_type        = QueryTuple<B>;
        using pointer           = QueryTuple<B>*;
        using reference         = QueryTuple<B>&;

    public:
        QueryTableIterator(QueryTable<B> &t) : table_(t), wrapper_(table_.getSchema(), table_.tuple_data_.data()),
            tuple_size_((std::is_same_v<B, bool>) ? table_.schema_->size()/8 : table_.schema_->size()) { }

        QueryTableIterator(QueryTable<B> &t, const int & offset)
           : table_(t),
             tuple_size_((std::is_same_v<B, bool>) ? table_.schema_->size()/8 : table_.schema_->size()),
             wrapper_(table_.getSchema(), table_.tuple_data_.data() + table_.tuple_size_ * offset) {
            assert(offset <= t.getTupleCount());

        }



        friend inline bool operator== (const QueryTableIterator<B> & a, const QueryTableIterator<B> & b)
            { return a.wrapper_.fields_ == b.wrapper_.fields_; };

        friend inline bool operator!= (const QueryTableIterator<B> & a, const QueryTableIterator<B> & b) {
            return a.wrapper_.fields_ != b.wrapper_.fields_;
            };

        QueryTuple<B> & operator*() { return wrapper_; }
        QueryTuple<B>* operator->() { return &wrapper_; }

        QueryTableIterator & operator++() {
            wrapper_.fields_ = wrapper_.fields_ + tuple_size_;
            return *this;
        }

        QueryTableIterator operator++ ( int )
        {
            QueryTableIterator<B> clone( *this );
            ++(*this);
            return clone;
        }


    private:
        QueryTable<B> &table_;
        QueryTuple<B> wrapper_;
        int tuple_size_;

    };

}
#endif // _QUERY_TABLE_H
