#ifndef _BUFFERED_COLUMN_DATA_H_
#define _BUFFERED_COLUMN_DATA_H_
#include "query_table/query_table.h"

#if  __has_include("emp-sh2pc/emp-sh2pc.h") || __has_include("emp-zk/emp-zk.h")

namespace vaultdb {
    class BufferedColumnTable : public QueryTable<Bit> {
        public:
            BufferedColumnTable(const size_t &tuple_cnt, const QuerySchema &schema, const SortDefinition &sort_def = SortDefinition()) : QueryTable<Bit>(tuple_cnt, schema, sort_def) {}

            BufferedColumnTable(const BufferedColumnTable &src) : QueryTable<Bit>(src) {}

            Field<Bit> getField(const int &row, const int &col) const override {
                return Field<Bit>();
            }

            inline void setField(const int &row, const int &col, const Field<Bit> &f) override {}

            SecureTable *secretShare() override  {
                assert(this->isEncrypted());
                throw; // can't secret share already encrypted table
            }

            void appendColumn(const QueryFieldDesc & desc) override {}

            void resize(const size_t &tuple_cnt) override {}

            Bit getDummyTag(const int & row)  const override {
                return emp::Bit(0);
            }

            void setDummyTag(const int & row, const Bit & val) override {}

            QueryTable<Bit> *clone() override {
                return new BufferedColumnTable(*this);
            }

            void setSchema(const QuerySchema &schema) override {}

            QueryTuple<Bit> getRow(const int & idx) override {
                return QueryTuple<Bit>();
            }

            void setRow(const int & idx, const QueryTuple<Bit> &tuple) override {}

            void compareSwap(const Bit & swap, const int  & lhs_row, const int & rhs_row) override {}

            void cloneTable(const int & dst_row, const int & dst_col, QueryTable<Bit> *src) override {}

            void cloneRow(const int & dst_row, const int & dst_col, const QueryTable<Bit> * src, const int & src_row) override {}

            void cloneRow(const Bit & write, const int & dst_row, const int & dst_col, const QueryTable<Bit> *src, const int & src_row) override {}

            void cloneRowRange(const int & dst_row, const int & dst_col, const QueryTable<Bit> *src, const int & src_row, const int & copies) override {}

            void cloneColumn(const int & dst_col, const int & dst_row, const QueryTable<Bit> *src, const int & src_col, const int & src_row = 0) override {}

            StorageModel storageModel() const override { return StorageModel::COLUMN_STORE; }

            void deserializeRow(const int & row, vector<int8_t> & src) override {}

            ~BufferedColumnTable() {}
    };
}

#else
namespace vaultdb {
    class BufferedColumnTable : public QueryTable<Bit> {
    public:
        BufferedColumnTable(const size_t &tuple_cnt, const QuerySchema &schema, const SortDefinition &sort_def = SortDefinition()) : QueryTable<Bit>(tuple_cnt, schema, sort_def) {}

        BufferedColumnTable(const BufferedColumnTable &src) : QueryTable<Bit>(src) {}

        Field<Bit> getField(const int &row, const int &col) const override {
            return Field<Bit>();
        }

        inline void setField(const int &row, const int &col, const Field<Bit> &f) override {}

        SecureTable *secretShare() override  {
            assert(this->isEncrypted());
            throw; // can't secret share already encrypted table
        }

        void appendColumn(const QueryFieldDesc & desc) override {}

        void resize(const size_t &tuple_cnt) override {}

        Bit getDummyTag(const int & row)  const override {
            return emp::Bit(0);
        }

        void setDummyTag(const int & row, const Bit & val) override {}

        QueryTable<Bit> *clone() override {
            return new BufferedColumnTable(*this);
        }

        void setSchema(const QuerySchema &schema) override {}

        QueryTuple<Bit> getRow(const int & idx) override {
            return QueryTuple<Bit>();
        }

        void setRow(const int & idx, const QueryTuple<Bit> &tuple) override {}

        void compareSwap(const Bit & swap, const int  & lhs_row, const int & rhs_row) override {}

        void cloneTable(const int & dst_row, const int & dst_col, QueryTable<Bit> *src) override {}

        void cloneRow(const int & dst_row, const int & dst_col, const QueryTable<Bit> * src, const int & src_row) override {}

        void cloneRow(const Bit & write, const int & dst_row, const int & dst_col, const QueryTable<Bit> *src, const int & src_row) override {}

        void cloneRowRange(const int & dst_row, const int & dst_col, const QueryTable<Bit> *src, const int & src_row, const int & copies) override {}

        void cloneColumn(const int & dst_col, const int & dst_row, const QueryTable<Bit> *src, const int & src_col, const int & src_row = 0) override {}

        StorageModel storageModel() const override { return StorageModel::COLUMN_STORE; }

        void deserializeRow(const int & row, vector<int8_t> & src) override {}

        ~BufferedColumnTable() {}
    };
}

#endif

#endif
