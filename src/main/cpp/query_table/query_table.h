#ifndef QUERY_TABLE_H
#define QUERY_TABLE_H

#include "query_schema.h"
#include "query_tuple.h"
#include <memory>
#include <vaultdb.h>
#include <ostream>
#include "util/utilities.h"
#include <emp-tool/emp-tool.h>




namespace  vaultdb {

    typedef std::pair<std::vector<int8_t>, std::vector<int8_t> > SecretShares;
    template class QueryTuple<BoolField>;
    template class QueryTuple<SecureBoolField>;

    template<typename B> class QueryTable;

    typedef  QueryTable<BoolField> PlainTable;
    typedef  QueryTable<SecureBoolField> SecureTable;

    template <typename B>
    class QueryTable {
        private:


        // tuple order
            SortDefinition orderBy;
            QuerySchema schema_;


    protected:
        std::vector<QueryTuple<B> > tuples_;

    public:
            QueryTable(const size_t &num_tuples, const QuerySchema &schema, const SortDefinition & sortDefinition);

            QueryTable(const size_t &num_tuples, const int &colCount);


            QueryTable(const QueryTable &src);

            ~QueryTable() = default;

            void resize(const size_t & tupleCount);
            bool isEncrypted() const;

            void setSchema(const QuerySchema &schema);

            const QuerySchema &getSchema() const;

            QueryTuple<B> getTuple(int idx) const;

            unsigned int getTupleCount() const;

            std::string toString(const bool &showDummies = false) const;

            void putTuple(const int &idx, const QueryTuple<B> &tuple);


            QueryTuple<B> *getTuplePtr(const int &idx) const;

            void setSortOrder(const SortDefinition &sortOrder);

            SortDefinition getSortOrder() const;


            // retrieves # of tuples that are not dummies
            // only works for unencrypted tables, o.w. returns getTupleCount()
            uint32_t getTrueTupleCount() const;

            std::vector<int8_t> serialize() const;


            std::shared_ptr<QueryTable<SecureBoolField> > secretShare(emp::NetIO *io, const int &party) const;

           // SecretShares generateSecretShares() const; // generate shares for alice and bob - for data sharing (non-computing) node

            [[nodiscard]] std::unique_ptr<QueryTable<BoolField> > reveal(int empParty = emp::PUBLIC) const;

            QueryTable<B> &operator=(const QueryTable<B> &src);

            bool operator==(const QueryTable<B> &other) const;

            bool operator!=(const QueryTable &other) const { return !(*this == other); }

            static std::shared_ptr<QueryTable<B> > deserialize(const QuerySchema & schema, const vector<int8_t> &tableBits);

            // encrypted version of deserialization using emp::Bit
            static std::shared_ptr<QueryTable>
            deserialize(const QuerySchema &schema, vector<Bit> &tableBits);
    };

    template <typename B>
    std::ostream &operator<<(std::ostream &os, const QueryTable<B> &table);



}
#endif // _QUERY_TABLE_H
