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
        SystemConfiguration &conf_ = SystemConfiguration::getInstance();
        int table_id_ = conf_.num_tables_++;
        std::map<int, int> fields_per_page_;
        std::map<int, int> pages_per_field_;

        BufferedColumnTable(const size_t &tuple_cnt, const QuerySchema &schema, const SortDefinition &sort_def = SortDefinition()) : QueryTable<Bit>(tuple_cnt, schema, sort_def) {
            assert(this->conf_.storageModel() == this->storageModel() && this->conf_.bp_enabled_);
            setSchema(schema);

            if(this->tuple_cnt_ == 0) {
                return;
            }

            this->conf_.bpm_.registerTable(this->table_id_, (QueryTable<Bit> *) this);
        }

        BufferedColumnTable(const BufferedColumnTable &src) : QueryTable<Bit>(src) {
            assert(this->conf_.storageModel() == this->storageModel() && this->conf_.bp_enabled_);
            setSchema(src.schema_);

            if(src.tuple_cnt_ == 0) {
                return;
            }

            this->conf_.bpm_.registerTable(this->table_id_, (QueryTable<Bit> *) this);
        }

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

        void setSchema(const QuerySchema &schema) override {
            this->schema_ = schema;
            this->plain_schema_ = QuerySchema::toPlain(schema);

            this->tuple_size_bytes_ = 0;

            for(int i = -1; i < this->schema_.getFieldCount(); ++i) {
                QueryFieldDesc desc = this->schema_.getField(i);
                int field_size_bytes = desc.size() * sizeof(emp::Bit);

                this->tuple_size_bytes_ += field_size_bytes;
                this->field_sizes_bytes_[i] = field_size_bytes;

                int bits_per_page = this->conf_.bpm_.unpacked_page_size_bits_;

                if(desc.size() / bits_per_page > 0) {
                    this->fields_per_page_[i] = 1;
                    this->pages_per_field_[i] = desc.size() / bits_per_page + (desc.size() % bits_per_page != 0);
                }
                else {
                    this->fields_per_page_[i] = bits_per_page / desc.size();
                    this->pages_per_field_[i] = 1;
                }
            }
        }

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
