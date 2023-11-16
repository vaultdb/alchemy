#ifndef _COMPRESSED_TABLE_
#define _COMPRESSED_TABLE_

#include "query_table/query_table.h"
#include "column_encoding.h"
#include "plain_encoding.h"
#include "bit_packed_encoding.h"
#include "query_table/secure_tuple.h"
#include "query_table/plain_tuple.h"


namespace vaultdb {

    //  need to support plain and MPC modes to facilitate compressing from plaintext data
    template <typename B>
    class CompressedTable : public QueryTable<B> {

        public:
        map<int, ColumnEncoding<B> *> column_encodings_; // algos for accessing compressed data in-situ

        CompressedTable(const size_t &tuple_cnt, const QuerySchema &schema, const SortDefinition &sort_def = SortDefinition()) : QueryTable<B>(tuple_cnt, schema, sort_def) {
            SystemConfiguration & s = SystemConfiguration::getInstance();
            assert(s.storageModel() == StorageModel::COMPRESSED_STORE);
            assert(s.emp_manager_ == nullptr || s.emp_manager_->sendingParty() != 0); // allow only one party to secret share at a time
            setSchema(schema);

            // each CompressedColumn has a pointer to column_data_ in parent table
            // column_data_ will be malloc'd according to compressed size
            for(int i = 0; i < schema.getFieldCount(); ++i) {
                if(schema.getField(i).bitPacked() && std::is_same_v<B, Bit>)
                    column_encodings_[i] = (ColumnEncoding<B> * ) new BitPackedEncoding((QueryTable<Bit> *) this, i);
                else
                    column_encodings_[i] = new PlainEncoding<B>(this, i);
            }
            column_encodings_[-1] = new PlainEncoding<B>(this, -1); // dummy tag
        }

        CompressedTable(QueryTable<B> *src, const map<int, ColumnEncodingModel> & encodings) : QueryTable<B>(src->tuple_cnt_, src->getSchema(), src->order_by_) {
            assert(src->getSchema().getFieldCount() == encodings.size());
            SystemConfiguration & s = SystemConfiguration::getInstance();
            assert(s.storageModel() == StorageModel::COMPRESSED_STORE);
            assert(s.emp_manager_->sendingParty() != 0); // allow only one party to secret share at a time

            setSchema(src->getSchema());

            for(int i = 0; i < src->getSchema().getFieldCount(); ++i) {
              column_encodings_[i] = ColumnEncoding<B>::compress(src, i, this, i, encodings.at(i));
            }

            column_encodings_[-1] = ColumnEncoding<B>::compress(src, -1, this, -1, encodings.at(-1));

        }

        explicit CompressedTable(const CompressedTable<B> &src) : QueryTable<B>(src) {
            assert(src.storageModel() == StorageModel::COMPRESSED_STORE);

            setSchema(src.getSchema());
            auto src_table = (CompressedTable<B> *) &src;

            for(auto pos : src_table->column_encodings_) {
                column_encodings_[pos.first] = src_table->column_encodings_.at(pos.first)->clone(this, pos.first);
            }
        }

        ~CompressedTable() override {
            // just deletes the ColumnEncodings - leaves this->column_data_ intact in case pinned.
            for(auto &col : column_encodings_) {
                delete col.second;
            }
        }
        void resize(const size_t &tuple_cnt) override {
            this->tuple_cnt_ = tuple_cnt;
            for(auto pos : column_encodings_) {
                column_encodings_[pos.first]->resize(tuple_cnt);
            }
        }


        StorageModel storageModel() const override { return  StorageModel::COMPRESSED_STORE; }
        Field<B> getField(const int  & row, const int & col)  const override {
            return column_encodings_.at(col)->getField(row);
        }

        void setField(const int  & row, const int & col, const Field<B> & f) override {
            column_encodings_.at(col)->setField(row, f);
        }

        B getDummyTag(const int & row)  const override {
            return column_encodings_.at(-1)->getField(row).template getValue<B>(); }

        void setDummyTag(const int & row, const B & val) override {
            Field<B> f(TypeUtilities::getBoolType<B>(), val);
            column_encodings_.at(-1)->setField(row, f);
        }

        // take a plain table (that may or may not be compressed) and secret share it
        SecureTable *secretShare() override  {
            if(this->isEncrypted()) return (SecureTable *) this->clone();
            auto dst_schema = QuerySchema::toSecure(this->schema_);
            SystemConfiguration &s = SystemConfiguration::getInstance();
            EmpManager *manager = s.emp_manager_;
            auto row_cnt = manager->getTableCardinality(this->tuple_cnt_);

            SecureTable *dst_table = new CompressedTable<Bit>(row_cnt, dst_schema, this->order_by_);
            for(auto pos : column_encodings_) {
                pos.second->secretShare(dst_table, pos.first);
            }
            return dst_table;
        }

        PlainTable *revealInsecure(const int & party = emp::PUBLIC) const override {
            auto dst_schema = QuerySchema::toPlain(this->schema_);

            PlainTable *dst = new CompressedTable<bool>(this->tuple_cnt_, dst_schema, this->order_by_);
            for(auto pos : column_encodings_) {
                pos.second->revealInsecure(dst, pos.first, party);
            }

            return dst;
        }

        void appendColumn(const QueryFieldDesc & desc) override {
            throw std::invalid_argument("NYI!");
        }


        // include dummy tag for this
        void cloneRow(const int & dst_row, const int & dst_col, const QueryTable<B> * src, const int & src_row) override {
            for(int i = 0; i < src->getSchema().getFieldCount(); ++i) {
                Field<B> field = src->getField(src_row, i);
                column_encodings_.at(dst_col+i)->setField(dst_row, field);
            }
            Field<B> dummy_tag = src->getField(src_row, -1);
            column_encodings_.at(-1)->setField(dst_row, dummy_tag);
        }

        void cloneRow(const B & write, const int & dst_row, const int & dst_col, const QueryTable<B> *src, const int & src_row) override;
        void cloneTable(const int & dst_row, QueryTable<B> *src) override {
            throw std::invalid_argument("NYI!");

        }
        QueryTuple<B> getRow(const int & idx) override {

            QueryTuple<B>  row(&this->schema_);

            for(int i = 0; i < this->schema_.getFieldCount(); ++i) {
                Field<B> field = column_encodings_.at(i)->getField(idx);
                row.setField(i, field);
            }
            Field<B> dummy_tag = column_encodings_.at(-1)->getField(idx);
            row.setField(-1, dummy_tag);
            return row;
        }

        void setRow(const int & idx, const QueryTuple<B> & tuple) override {
            assert(*tuple.schema_ == this->schema_);
            for(int i = 0; i < this->schema_.getFieldCount(); ++i) {
                Field<B> field = tuple.getField(i);
                column_encodings_.at(i)->setField(idx, field);
            }
            Field<B> dummy_tag = tuple.getField(-1);
            column_encodings_.at(-1)->setField(idx, dummy_tag);
        }

        QueryTable<B> *clone() override { return new CompressedTable<B>(*this); }
        void compareSwap(const B & swap, const int  & lhs_row, const int & rhs_row) override;

        void cloneColumn(const int & dst_col, const int & dst_idx, const QueryTable<B> *src, const int & src_col, const int & src_offset = 0) override {
            assert(dst_idx == 0); // offsets NYI
            assert(src_offset == 0);
            assert(src->storageModel() == StorageModel::COMPRESSED_STORE);

            column_encodings_[dst_col] = ((CompressedTable<B> *) src)->column_encodings_.at(src_col)->clone(this, dst_col);
        }

        void setSchema(const QuerySchema &schema) override {
            // for now just using default settings for this
            this->schema_ = schema;
            int running_count = 0;
            int field_size_bytes;

            if(this->isEncrypted()) {
                // covers dummy tag as -1
                this->tuple_size_bytes_ = 0;
                QueryFieldDesc desc;

                for(auto pos : this->schema_.offsets_) {
                    desc = this->schema_.getField(pos.first);
                    field_size_bytes =  desc.size() * sizeof(emp::Bit);
                    this->tuple_size_bytes_ += field_size_bytes;
                    this->field_sizes_bytes_[pos.first] = field_size_bytes;
                    running_count += field_size_bytes;
                }

                return;
            }

            // plaintext case
            this->tuple_size_bytes_ = this->schema_.size() / 8; // bytes for plaintext
            for(auto pos : this->schema_.offsets_) {
                field_size_bytes = this->schema_.getField(pos.first).size() / 8;
                this->field_sizes_bytes_[pos.first] = field_size_bytes;
                running_count += field_size_bytes;
            }

        }


    };


}

#endif
