#ifndef _QUERY_TABLE_H_
#define _QUERY_TABLE_H_


#include "query_schema.h"
#include "query_tuple.h"
#include <memory>
#include <ostream>
#include "util/utilities.h"
#include "plain_tuple.h"
#include "util/data_utilities.h"

namespace  vaultdb {



    template<typename B> class QueryTable;
    typedef QueryTable<bool> PlainTable;
    typedef QueryTable<emp::Bit> SecureTable;

    template<typename B>
    class QueryTable {

        public:
            size_t tuple_size_bytes_;
            size_t tuple_cnt_;
            std::map<int, int> field_sizes_bytes_;
            bool pinned_ = false; // if set to true, do not delete table automatically.  For use in binary operators with CTEs
            SortDefinition order_by_;
            std::map<int, std::vector<int8_t> > column_data_;

            QueryTable<B>(const size_t &tuple_cnt, const QuerySchema &schema, const SortDefinition & sort_def) : order_by_(sort_def), tuple_cnt_(tuple_cnt) {
              // all other logic needs to be done in child classes because setSchema is class-dependant
            }

            QueryTable<B>(const QueryTable<B> &src) : order_by_(src.order_by_), tuple_cnt_(src.tuple_cnt_) {
                pinned_ = false;

                for(auto col_entry : src.column_data_) {
                    column_data_[col_entry.first] = col_entry.second;
                }
            }

            virtual ~QueryTable() = default;
            inline  bool isEncrypted() const {     return std::is_same_v<B, emp::Bit>; }

            virtual void resize(const size_t &tuple_cnt) = 0;
            virtual StorageModel storageModel() const = 0;
            void putTuple(const int &idx, const QueryTuple<B> &tuple) {
                assert(*tuple.getSchema() == schema_);

                int8_t *write_ptr;
                int write_size;
                int8_t *read_ptr = (int8_t *) tuple.getData();

                for(int i = 0; i < schema_.getFieldCount(); ++i) {
                    write_ptr = column_data_[i].data() + idx * field_sizes_bytes_[i];
                    write_size = field_sizes_bytes_[i];
                    memcpy(write_ptr, read_ptr, write_size);
                    read_ptr += write_size;
                }
                write_size =  field_sizes_bytes_[-1];
                write_ptr = column_data_[-1].data() +  write_size * idx;
                memcpy(write_ptr, read_ptr, write_size);

            }

        inline const  QuerySchema & getSchema() const  { return schema_; }


        virtual Field<B> getField(const int  & row, const int & col)  const = 0;
        virtual void setField(const int  & row, const int & col, const Field<B> & f)  = 0;


        virtual B getDummyTag(const int & row)  const = 0;
        virtual void setDummyTag(const int & row, const B & val) = 0;
        virtual void appendColumn(const QueryFieldDesc & desc) = 0;

        vector<int8_t> serialize() const;


        virtual SecureTable *secretShare() = 0;


        SecretShares generateSecretShares() const; // generate shares for alice and bob - for data sharing (non-computing) node

        QueryTable<B> &operator=(const QueryTable<B> &src);


        static QueryTable<B> *deserialize(const QuerySchema &schema, const vector<int8_t> &table_bytes);
        static QueryTable<B> *deserialize(const QuerySchema &schema, const vector<Bit> &table_bits);

        int getTrueTupleCount() const {
            assert(!isEncrypted());
            size_t count = 0;

            for(size_t i = 0; i < tuple_cnt_; ++i) {
                B dummy_tag = getDummyTag(i); // always bool because !isEncrypted
                bool d = *((bool *) &dummy_tag);
                if(!d) {
                    ++count;
                }
            }

            return count;

        }

        PlainTable *reveal(const int & party = emp::PUBLIC);

        // holds onto dummy rows
        virtual PlainTable *revealInsecure(const int & party = emp::PUBLIC);

        inline PlainTuple revealRow(const int &row, const int &party = PUBLIC)   {
            if(std::is_same_v<B, bool>) {
                return getPlainTuple(row);
            }

            PlainTuple plain(&plain_schema_);
            int field_cnt = schema_.getFieldCount();

            for(int i = 0; i < field_cnt; i++) {
                auto s = getField(row, i);
                auto p = s.reveal(schema_.getField(i), party);
                plain.setField(i, p);
            }

            PlainField dummy_tag = getField(row, -1).reveal(schema_.getField(-1), party);
            plain.setDummyTag(dummy_tag);

            return plain;
        }



        inline bool empty() const { return tuple_cnt_ == 0; }


        QueryTuple<bool> getPlainTuple(size_t idx) const;

        // includes dummy tag for cloneRow
        virtual void cloneRow(const int & dst_row, const int & dst_col, const QueryTable<B> * src, const int & src_row) = 0;

        // conditional writes
        virtual void cloneRow(const B & write, const int & dst_row, const int & dst_col, const QueryTable<B> *src, const int & src_row) = 0;

        void cloneColumn(const int & dst_col, const QueryTable<B> *src, const int & src_col) {
            assert(src->tuple_cnt_ == this->tuple_cnt_);
            assert(src->getSchema().getField(src_col).size() == getSchema().getField(dst_col).size());

            column_data_[dst_col] = src->column_data_.at(src_col);
        }

        // copy all src_col entries from src_offset until end (or until we run out of slots)
        // to self at dst_col starting at dst_idx.
        virtual void cloneColumn(const int & dst_col, const int & dst_idx, const QueryTable<B> *src, const int & src_col, const int & src_offset = 0) = 0;

        virtual void cloneTable(const int & dst_row, QueryTable<B> *src) = 0;

        // for serializing a row
       Integer unpackRow(const int & row, const int & col_cnt=-1) const {
           assert(isEncrypted());
           auto src = (SecureTable *) this;
            int to_unpack = (col_cnt < 1) ? schema_.getFieldCount() : col_cnt;

            Integer dst(schema_.size(), 0, PUBLIC);
            Bit *cursor = dst.bits.data();

            for(int i = 0; i < to_unpack; ++i) {
                SecureField f = src->getField(row, i);
                QueryFieldDesc desc = schema_.getField(i);

                switch(f.getType()) {
                    case FieldType::SECURE_BOOL: {
                        Bit b = f.getValue<emp::Bit>();
                        *cursor = b;
                        break;
                    }
                    case FieldType::SECURE_INT:
                    case FieldType::SECURE_LONG:
                    case FieldType::SECURE_STRING: {
                        Integer i_field = f.getValue<Integer>();
                        memcpy(cursor, i_field.bits.data(), desc.size() * sizeof(Bit));
                        break;
                    }
                    case FieldType::SECURE_FLOAT: {
                        Float f_field = f.getValue<Float>();
                        memcpy(cursor, f_field.value.data(), desc.size() * sizeof(Bit));
                        break;
                    }
                    default:
                        throw;

                }
                cursor += desc.size();
            }

            // do dummy tag last
            SecureField b_field = src->getDummyTag(row);
            Bit b = b_field.getValue<emp::Bit>();
            *cursor = b;
            return dst.bits;
        }

        // does not include dummy tag
        Integer unpackRow(const int & row, const int & col_cnt, const int & selection_length_bits) const {
            assert(isEncrypted());
            auto src = (SecureTable *) this;
            Integer dst(selection_length_bits, 0, PUBLIC);
            Bit *write_cursor = dst.bits.data();

            for(int i = 0; i < col_cnt; ++i) {
                SecureField f = src->getField(row, i);
                QueryFieldDesc desc = schema_.fields_.at(i);

                switch(f.getType()) {
                    case FieldType::SECURE_BOOL: {
                        Bit b = f.getValue<emp::Bit>();
                        *write_cursor = b;
                        break;
                    }
                    case FieldType::SECURE_INT:
                    case FieldType::SECURE_LONG:
                    case FieldType::SECURE_STRING: {
                        Integer i_field = f.getValue<Integer>();
                        memcpy(write_cursor, i_field.bits.data(), desc.size() * sizeof(Bit));
                        break;
                    }
                    case FieldType::SECURE_FLOAT: {
                        Float f_field = f.getValue<Float>();
                        memcpy(write_cursor, f_field.value.data(), desc.size() * sizeof(Bit));
                        break;
                    }
                    default:
                        throw;

                }
                write_cursor += desc.size();
                if((write_cursor - dst.bits.data()) >= selection_length_bits)
                    break;
            }
            return dst;
        }


         inline void packRow(const int & row, Integer src) {
           assert(isEncrypted());
           auto dst = (SecureTable *) this;
            Bit *cursor = src.bits.data();
            Bit *end = src.bits.data() + src.size() - 1; // -1 for dummy tag, do this separately
            int dst_idx = 0;
            while(cursor < end) {
                QueryFieldDesc desc = schema_.getField(dst_idx);
                switch(desc.getType()) {
                    case FieldType::SECURE_BOOL: {
                        Bit b = *cursor;
                        SecureField f(FieldType::SECURE_BOOL, b);
                        dst->setField(row, dst_idx, f);
                        break;
                    }
                    case FieldType::SECURE_INT:
                    case FieldType::SECURE_LONG:
                    case FieldType::SECURE_STRING: {
                        Integer i_field(desc.size() + desc.bitPacked(), 0, PUBLIC);
                        memcpy(i_field.bits.data(), cursor, desc.size() * sizeof(Bit));
                        SecureField f(desc.getType(), i_field, desc.getStringLength());
                        dst->setField(row, dst_idx, f);
                        break;
                    }
                    case FieldType::SECURE_FLOAT: {
                        Float f_field;
                        memcpy(f_field.value.data(), cursor, desc.size() * sizeof(Bit));
                        SecureField f(FieldType::SECURE_FLOAT, f_field);
                        dst->setField(row, dst_idx, f);
                        break;
                    }
                    default:
                        throw;

                }
                cursor += desc.size();
                ++dst_idx;
            }

            // do dummy tag last
            Bit dummy_tag = *end;
            dst->setDummyTag(row, dummy_tag);
        }

        virtual QueryTuple<B> getRow(const int & idx) = 0;
        virtual void setRow(const int & idx, const QueryTuple<B> &tuple) = 0;

        virtual QueryTable<B> *clone() = 0;
        virtual void compareSwap(const B & swap, const int  & lhs_row, const int & rhs_row) = 0;

        bool operator==(const QueryTable<B> &other) const;
        string toString(const bool &show_dummies = false) const;
        string toString(const size_t &limit, const bool &show_dummies = false) const;




       virtual void setSchema(const QuerySchema &schema) = 0;

       static QueryTable<B> *getTable(const size_t &tuple_cnt, const QuerySchema &schema, const SortDefinition &sort_def = SortDefinition());

        // includes dummy tag
        vector<int8_t> serializeRow(const int & row) const  {
           vector<int8_t> dst(tuple_size_bytes_);
           int8_t *write_ptr = dst.data();

           for (int i = 0; i < this->schema_.getFieldCount(); ++i) {
               Field<B> to_write = getField(row, i);
               Field<B>::writeField(write_ptr, to_write, this->schema_.getField(i));
               write_ptr += field_sizes_bytes_.at(i);
           }
           // dummy tag
           B dummy_tag = getField(row, -1).template getValue<B>();
           *((B *) write_ptr) = dummy_tag;
           return dst;
        }



    protected:
        QuerySchema schema_;
        QuerySchema plain_schema_;





    };


    static std::ostream &operator<<(std::ostream &os, const QueryTable<bool> &table)   {
        os << table.getSchema() << " isEncrypted? false, order by: " << DataUtilities::printSortDefinition(table.order_by_) << std::endl;

        for(int i = 0; i < table.tuple_cnt_; ++i) {
            os <<  table.getPlainTuple(i);

            if(!table.getDummyTag(i))
                os << std::endl;
        }

        return os;
    }

    static std::ostream &operator<<(std::ostream &os, const QueryTable<Bit> &table)   {
        throw std::runtime_error("secret shared table!");
    }


}

#endif //  _QUERY_TABLE_H_
