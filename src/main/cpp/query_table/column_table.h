#ifndef _COLUMN_TABLE_H_
#define _COLUMN_TABLE_H_
#include "query_table.h"

namespace vaultdb {

    template<typename B>
    class ColumnTable : public QueryTable<B> {
    public:
        // empty sort definition for default case
        ColumnTable<B>(const size_t &tuple_cnt, const QuerySchema &schema, const SortDefinition &sort_def = SortDefinition())
                : QueryTable<B>(tuple_cnt, schema, sort_def) {
            setSchema(schema);

            if(tuple_cnt == 0)
                return;

            int write_size;
            for(auto col_entry : this->field_sizes_bytes_) {
                vector<int8_t> fields;
                write_size = tuple_cnt * col_entry.second;
                fields.resize(write_size);
                memset(fields.data(), 0, write_size);
                this->column_data_[col_entry.first] = fields;
            }

            // initialize dummy tags to true
            B d(true);
            B *dummy_col = (B *) this->column_data_[-1].data();

            for(int i = 0; i < tuple_cnt; ++i) {
                dummy_col[i] = d;
            }
        }

        explicit ColumnTable<B>(const ColumnTable<B> &src) : QueryTable<B>(src) {
            setSchema(src.schema_);
        }

        Field<B> getField(const int  & row, const int & col)  const override {
            int8_t *src = getFieldPtr(row, col);
            QueryFieldDesc desc = this->schema_.getField(col);
            return Field<B>::deserialize(desc, src);
        }


        void setField(const int  & row, const int & col, const Field<B> & f)  override {
            int8_t *dst = getFieldPtr(row, col);
            Field<B>::writeField(dst, f, this->schema_.getField(col));
        }


        inline int8_t *getFieldPtr(const int & row, const int & col) const  {
            return const_cast<int8_t *>(this->column_data_.at(col).data() + row * this->field_sizes_bytes_.at(col));
        }

        QueryTable<B> *clone()  override {
            return new ColumnTable<B>(*this);
        }

         void setSchema(const QuerySchema &schema) override {
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

         void resize(const size_t &tuple_cnt) override {
            int prev_size = this->tuple_cnt_;
            this->tuple_cnt_ = tuple_cnt;


             for(auto col_entry : this->column_data_) {
                 this->column_data_[col_entry.first].resize(tuple_cnt * this->field_sizes_bytes_[col_entry.first]);
             }

             if(tuple_cnt > prev_size) {
                 // initialize dummy tags to true
                 B d(true);
                 B *dummy_col = (B *) this->column_data_[-1].data();

                 for(int i = prev_size; i < tuple_cnt; ++i) {
                     dummy_col[i] = d;
                 }
             }
        }

        QueryTuple<B> getRow(const int & idx) override {
            QueryTuple<B>  row(&this->schema_);
            int8_t *write_pos = row.getData();

            for(int i = 0; i < this->schema_.getFieldCount(); ++i) {
                auto read_pos = getFieldPtr(idx, i);
                memcpy(write_pos, read_pos, this->field_sizes_bytes_[i]);
                write_pos += this->field_sizes_bytes_[i];
            }
            memcpy(write_pos, getFieldPtr(idx, -1), this->field_sizes_bytes_[-1]);

            return row;
        }
        void setRow(const int & idx, const QueryTuple<B> &tuple) override;

        void compareSwap(const B & swap, const int  & lhs_row, const int & rhs_row) override;

        void appendColumn(const QueryFieldDesc & desc) override {
            int ordinal = desc.getOrdinal();
            assert(ordinal == this->schema_.getFieldCount());

            this->schema_.putField(desc);
            this->schema_.initializeFieldOffsets();

            auto field_size_bytes =  (std::is_same_v<B, bool>) ? desc.size() / 8 : desc.size() * sizeof(emp::Bit);
            this->tuple_size_bytes_ += field_size_bytes;
            this->field_sizes_bytes_[ordinal] = field_size_bytes;

            this->column_data_[ordinal] = std::vector<int8_t>(this->tuple_cnt_ * field_size_bytes);
        }
// up to two-way secret share - both Alice and Bob providing private inputs
        SecureTable *secretShare() override  {
            assert(!this->isEncrypted());
            SystemConfiguration & conf = SystemConfiguration::getInstance();
            return conf.emp_manager_->secretShare((PlainTable *) this);
        }

        B getDummyTag(const int & row)  const override {
            auto dummy_tags = (B*) this->column_data_.at(-1).data();
            return dummy_tags[row];
        }

        void setDummyTag(const int & row, const B & val)  override {
            auto dummy_tags = (B*) this->column_data_[-1].data();
            dummy_tags[row] = val;
        }


        // copy all columns from src to dst
        void cloneTable(const int & dst_row, QueryTable<B> *s) override {

            assert(s->getSchema() == this->schema_);
            assert((s->tuple_cnt_ + dst_row) <= this->tuple_cnt_);
            assert(s->storageModel() == StorageModel::COLUMN_STORE);

            auto src = (ColumnTable<B> *) s;
            int8_t *write_pos, *read_pos;

            // copy out entire cols
            for(auto pos : this->field_sizes_bytes_) {
                int col_id = pos.first;
                int field_size = pos.second;
                write_pos = getFieldPtr(dst_row, col_id);
                read_pos = src->column_data_.at(col_id).data();
                memcpy(write_pos, read_pos, field_size * src->tuple_cnt_);
            }
        }

        // dummy tag included
        void cloneRow(const int & dst_row, const int & dst_col, const QueryTable<B> * s, const int & src_row) override{
            assert(s->storageModel() == StorageModel::COLUMN_STORE);
            auto src = (ColumnTable<B> *) s;
            int src_size = src->tuple_size_bytes_ - src->field_sizes_bytes_.at(-1);

            int cursor = 0; // bytes
            int write_idx = dst_col; // field indexes

            // serialize src vals into a temp row
            // does not include dummy tag - handle further down in this method
            vector<int8_t> row = src->unpackRowBytes(src_row, src->schema_.getFieldCount());

            // re-pack row
            while(cursor < src_size) {
                int8_t *write_pos = getFieldPtr(dst_row, write_idx);
                int bytes_remaining = src_size - cursor;
                int dst_len = this->field_sizes_bytes_.at(write_idx);
                int to_read = (dst_len < bytes_remaining) ? dst_len : bytes_remaining;
                memcpy(write_pos, row.data() + cursor, to_read);
                cursor += to_read;
                ++write_idx;
            }

            B *dummy_tag = (B*) src->getFieldPtr(src_row, -1);
            *((B *) getFieldPtr(dst_row, -1)) = *dummy_tag;
        }

        void cloneRow(const B & write, const int & dst_row, const int & dst_col, const QueryTable<B> *src, const int & src_row) override;

        void cloneColumn(const int & dst_col, const int & dst_idx, const QueryTable<B> *s, const int & src_col, const int & src_offset = 0) override {

            assert(s->storageModel() == StorageModel::COLUMN_STORE);
            assert(s->getSchema().getField(src_col).getType() == this->getSchema().getField(dst_col).getType());

            auto src = (ColumnTable<B> *) s;

            int8_t *write_ptr = getFieldPtr(dst_idx, dst_col);
            int8_t *read_ptr = src->getFieldPtr(src_offset, src_col);
            int src_tuples =   src->tuple_cnt_ - src_offset;
            if(src_tuples > (this->tuple_cnt_ - dst_idx)) {
                src_tuples = this->tuple_cnt_ - dst_idx; // truncate to our available slots
            }

            memcpy(write_ptr, read_ptr, this->field_sizes_bytes_.at(dst_col) * src_tuples);
        }

        StorageModel storageModel() const override { return StorageModel::COLUMN_STORE; }
    private:
        // need static context to avoid needlessly invoking default constructors for Bits

        vector<int8_t> unpackRowBytes(const int & row, const int & col_cnt) const {
            int read_len = 0;
            for(int i = 0; i < col_cnt; ++i) {
                read_len += this->field_sizes_bytes_.at(i);
            }

            vector<int8_t> dst(read_len);
            int8_t *cursor = dst.data();

            for(int i = 0; i < col_cnt; ++i) {
                int write_len = this->field_sizes_bytes_.at(i);
                memcpy(cursor, getFieldPtr(row, i), write_len);
                cursor += write_len;
            }

            return dst;

        }

        // unpack entire row
        vector<int8_t> unpackRowBytes(const int & row) const {
            vector<int8_t> dst(this->tuple_size_bytes_);
            int col_cnt = this->schema_.getFieldCount();
            int8_t *cursor = dst.data();

            for(int i = 0; i < col_cnt; ++i) {
                int write_len = this->field_sizes_bytes_.at(i);
                memcpy(cursor, getFieldPtr(row, i), write_len);
                cursor += write_len;
            }

            // copy dummy tag to last slot
            memcpy(cursor, getFieldPtr(row, -1), this->field_sizes_bytes_.at(-1));
            return dst;
        }


    };
}


#endif
