#ifndef _COMPRESSED_TABLE_
#define _COMPRESSED_TABLE_

#include "query_table/query_table.h"
#include "column_encoding.h"
#include "uncompressed.h"
#include "bit_packed_encoding.h"
#include "dictionary_encoding.h"
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
                    column_encodings_[i] = new Uncompressed<B>(this, i);
            }

            column_encodings_[-1] = new Uncompressed<B>(this, -1); // dummy tag
            Field<B> dummy_tag(TypeUtilities::getBoolType<B>(), B(true));  // set all slots to dummies initially
            column_encodings_[-1]->initializeColumn(dummy_tag);
        }

        CompressedTable(QueryTable<B> *src, const map<int, CompressionScheme> & encodings) : QueryTable<B>(src->tuple_cnt_, src->getSchema(), src->order_by_) {
            SystemConfiguration & s = SystemConfiguration::getInstance();
            assert(s.storageModel() == StorageModel::COMPRESSED_STORE);
            assert(s.emp_manager_->sendingParty() != 0); // allow only one party to secret share at a time

            setSchema(src->getSchema());

            for(int i = 0; i < src->getSchema().getFieldCount(); ++i) {
              auto encoding =  encodings.find(i) == encodings.end() ? CompressionScheme::PLAIN : encodings.at(i);
              column_encodings_[i] = ColumnEncoding<B>::compress(src, i, this, i, encoding);
            }

            // dummy tag
            auto dummy_encoding = (encodings.find(-1) == encodings.end()) ? CompressionScheme::PLAIN : encodings.at(-1);

            column_encodings_[-1] = ColumnEncoding<B>::compress(src, -1, this, -1, dummy_encoding);

        }

        CompressedTable(const CompressedTable<B> &src) : QueryTable<B>(src) {
            assert(src.storageModel() == StorageModel::COMPRESSED_STORE);
            setSchema(src.getSchema());

            for(auto pos : src.column_encodings_) {
                ColumnEncoding<B> *src_encoding = src.column_encodings_.at(pos.first);
                auto dst_encoding = src_encoding->clone(this, pos.first);
                column_encodings_[pos.first] = dst_encoding;
            }
        }

        virtual ~CompressedTable() override {
            // just deletes the ColumnEncodings - leaves this->column_data_ intact in case pinned.
            for(auto &col : column_encodings_) {
                delete col.second;
            }
        }

        void resize(const size_t &tuple_cnt) override {
            if(tuple_cnt == this->tuple_cnt_) return;
            this->tuple_cnt_ = tuple_cnt;
            for(auto pos : column_encodings_) {
                column_encodings_[pos.first]->resize(tuple_cnt);
            }
        }

        QueryTable<B> *decompress()  {
            auto dst = new CompressedTable<B>(this->tuple_cnt_, this->schema_, this->order_by_);
            for(auto col : column_encodings_) {
                col.second->decompress(dst, col.first);
            }
            return dst;
        }

        StorageModel storageModel() const override { return  StorageModel::COMPRESSED_STORE; }

        std::vector<emp::Bit> getPage(const PageId &pid) override {
            return std::vector<emp::Bit>();
        }

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

             if(!this->isEncrypted()) {
                    return (QueryTable<bool> *) this;
                }

            auto dst_schema = QuerySchema::toPlain(this->schema_);

            PlainTable *dst = new CompressedTable<bool>(this->tuple_cnt_, dst_schema, this->order_by_);
            for(auto pos : column_encodings_) {
                pos.second->revealInsecure(dst, pos.first, party);
            }

            return dst;
        }

        void appendColumn(const QueryFieldDesc & desc) override {
            int ordinal = desc.getOrdinal();
            assert(ordinal == this->schema_.getFieldCount());

            this->schema_.putField(desc);
            this->schema_.initializeFieldOffsets();

            auto field_size_bytes =  (std::is_same_v<B, bool>) ? desc.size() / 8 : desc.size() * sizeof(emp::Bit);
            this->tuple_size_bytes_ += field_size_bytes;
            this->field_sizes_bytes_[ordinal] = field_size_bytes;

            if(std::is_same_v<B, Bit> && desc.bitPacked()) {
                column_encodings_[ordinal] = (ColumnEncoding<B> * ) new BitPackedEncoding((QueryTable<Bit> *) this, ordinal);
            }
            else {
                column_encodings_[ordinal] = new Uncompressed<B>(this, ordinal);
            }

        }


        // include dummy tag for this
        void cloneRow(const int & dst_row, const int & dst_col, const QueryTable<B> * src, const int & src_row) override {
            bool copy_compatible = true;
            int col_cnt = src->getSchema().getFieldCount();
//            for(int i = 0; i < col_cnt; ++i) {
//                if(src->getSchema().getField(i).getType() != this->getSchema().getField(dst_col + i).getType()) {
//                    copy_compatible = false;
//                    break;
//                }
//            }

            if(copy_compatible) {
                for(int i = 0; i < col_cnt; ++i) {
                    auto dst_encoding = column_encodings_.at(dst_col + i);
                    dst_encoding->cloneField(dst_row, src, src_row, i);
                }
                auto dst_encoding = column_encodings_.at(-1);
                dst_encoding->cloneField(dst_row, src, src_row, -1);
                return;
            }

        }

        void cloneRowRange(const int & dst_row, const int & dst_col, const QueryTable<B> *src, const int & src_row, const int & copies) override {
            throw std::invalid_argument("NYI!");
        }

        void cloneRow(const B & write, const int & dst_row, const int & dst_col, const QueryTable<B> *src, const int & src_row) override;

        void cloneTable(const int & dst_row, const int & dst_col, QueryTable<B> *s) override {
            assert(s->getSchema() == this->schema_);
            assert((s->tuple_cnt_ + dst_row) <= this->tuple_cnt_);
            assert(s->storageModel() == StorageModel::COMPRESSED_STORE);
            assert((dst_col + s->getSchema().getFieldCount()) <= this->schema_.getFieldCount());

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

        void compareSwap(const B & swap, const int  & lhs_row, const int & rhs_row) override {
            for(auto pos : column_encodings_) {
                pos.second->compareSwap(swap, lhs_row, rhs_row);
            }
        }

        void cloneColumn(const int & dst_col, const int & dst_idx, const QueryTable<B> *s, const int & src_col, const int & src_offset = 0) override {
            assert(s->storageModel() == StorageModel::COMPRESSED_STORE);
            auto src = (CompressedTable<B> *) s;

            assert(src->getSchema().getField(src_col).getType() == this->getSchema().getField(dst_col).getType());

            int src_tuples =   src->tuple_cnt_ - src_offset;
            if(src_tuples > (this->tuple_cnt_ - dst_idx)) {
                src_tuples = this->tuple_cnt_ - dst_idx; // truncate to our available slots
            }

            // clone column encoding
            auto dst_encoding = column_encodings_.at(dst_col);
            dst_encoding->cloneColumn(dst_idx, src, src_col, src_offset);
        }

        void setSchema(const QuerySchema &schema) override {
            // for now just using default settings for this
            this->schema_ = schema;
            this->plain_schema_ = QuerySchema::toPlain(schema);
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

        void deserializeRow(const int & row, vector<int8_t> & src) override {
            int src_size_bytes = src.size() - sizeof(B); // don't handle dummy tag until end
            int cursor = 0; // bytes
            int write_idx = 0; // column indexes

            // does not include dummy tag - handle further down in this method
            // re-pack row
            while(cursor < src_size_bytes && write_idx < this->schema_.getFieldCount()) {
                int bytes_remaining = src_size_bytes - cursor;
                int dst_len = this->field_sizes_bytes_.at(write_idx);
                if(dst_len > bytes_remaining) {
                    vector<int8_t> tmp(dst_len, 0);
                    memcpy(tmp.data(), src.data() + cursor, bytes_remaining);
                    column_encodings_[write_idx]->deserializeField(row, tmp.data());
                    break;
                }
                column_encodings_[write_idx]->deserializeField(row, src.data() + cursor);
                cursor += dst_len;
                ++write_idx;
            }

            B *dummy_tag = (B*) (src.data() + src.size() - sizeof(B));
            setDummyTag(row, *dummy_tag);
        }
    };


}

#endif
