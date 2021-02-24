#ifndef _QUERY_TABLE_H
#define _QUERY_TABLE_H

#include "emp-tool/emp-tool.h"
#include "query_schema.h"
#include "query_tuple.h"
#include <memory>
#include <vaultdb.h>
#include <ostream>


namespace  vaultdb {

    typedef std::pair<std::vector<int8_t>, std::vector<int8_t> > SecretShares;
    class QueryTuple;

    class QueryTable {
        private:


        // tuple order
            SortDefinition orderBy;
            QuerySchema schema_;


    protected:
        std::vector<QueryTuple> tuples_;

    public:
            QueryTable(const int &num_tuples, const QuerySchema &schema, SortDefinition sortDefinition);

            QueryTable(const int &num_tuples, const int &colCount);

            QueryTable(const QueryTable &src);

            ~QueryTable() {}


            void resize(const size_t & tupleCount);
            bool isEncrypted() const;

            void setSchema(const QuerySchema &schema);

            const QuerySchema &getSchema() const;

            QueryTuple getTuple(int idx) const;

            unsigned int getTupleCount() const;

            std::string toString(const bool &showDummies = false) const;

            void putTuple(const int &idx, const QueryTuple &tuple);

            void setTupleDummyTag(const int &tupleIdx, const types::Value &dummyTag);

            QueryTuple *getTuplePtr(const int &idx) const;

            void setSortOrder(const SortDefinition &sortOrder);

            SortDefinition getSortOrder() const;


            // retrieves # of tuples that are not dummies
            // only works for unencrypted tables, o.w. returns getTupleCount()
            uint32_t getTrueTupleCount() const;

            std::vector<int8_t> serialize() const;


            std::shared_ptr<QueryTable> secretShare(emp::NetIO *io, const int &party) const; // shared_ptr so we can pass it among Operator instances

            SecretShares generateSecretShares() const; // generate shares for alice and bob - for data sharing (non-computing) node

            [[nodiscard]] std::unique_ptr<QueryTable> reveal(int empParty = emp::PUBLIC) const;

            QueryTable &operator=(const QueryTable &src);

            bool operator==(const QueryTable &other) const;

            bool operator!=(const QueryTable &other) const { return !(*this == other); }

            static std::shared_ptr<QueryTable> deserialize(const QuerySchema & schema, const vector<int8_t> &tableBits);

            // encrypted version of deserialization using emp::Bit
            static std::shared_ptr<QueryTable>
            deserialize(const QuerySchema &schema, vector<Bit> &tableBits);
    };
    std::ostream &operator<<(std::ostream &os, const QueryTable &table);



}
#endif // _QUERY_TABLE_H
