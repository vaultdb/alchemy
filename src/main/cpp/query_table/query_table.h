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
        // size of each tuple in bytes
        size_t tuple_size_bytes_;
        size_t tuple_cnt_;
        std::map<int, int> field_sizes_bytes_;
        std::map<int, int> field_offsets_bytes_;
        bool pinned_ = false; // if set to true, do not delete table automatically.  For use in binary operators with CTEs


        std::map<int, std::vector<int8_t> > column_data_;

        // empty sort definition for default case
        QueryTable(const size_t &tuple_cnt, const QuerySchema &schema,
                 const SortDefinition &sort_def = SortDefinition());

        // deep copy
        explicit QueryTable(const QueryTable<B> &src);

        virtual ~QueryTable() {}
        inline  bool isEncrypted() const {     return std::is_same_v<B, emp::Bit>; }

        void resize(const size_t &tuple_cnt);



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

        inline const  QuerySchema getSchema() const  { return schema_; }
        inline SortDefinition getSortOrder() const { return order_by_; }
        inline void setSortOrder(const SortDefinition &order_by) { order_by_ = order_by; }

        inline size_t getTupleCount() const { return tuple_cnt_; }
        Field<B> getField(const int  & row, const int & col)  const {
            int8_t *read_ptr = getFieldPtr(row, col);
            return Field<B>::deserialize(schema_.fields_.at(col), read_ptr );
        }


        void setField(const int  & row, const int & col, const Field<B> & f)  {
            int8_t *write_ptr = getFieldPtr(row, col);
            f.serialize(write_ptr, schema_.fields_.at(col));

        }


        B getDummyTag(const int & row)  const {
            int8_t *dummy_tag =  getFieldPtr(row, -1);
            Field<B> res = Field<B>::deserialize(schema_.getField(-1), dummy_tag);
            return res.template getValue<B>();
        }

        void setDummyTag(const int & row, const B & val) {
            int8_t *write_ptr = getFieldPtr(row, -1);
            QueryFieldDesc desc = schema_.fields_.at(-1);
            Field<B> f(desc.getType(), val);
            f.serialize(write_ptr, desc);
        }


        vector<int8_t> serialize() const;

        inline int8_t *getFieldPtr(const int & row, const int & col) const {
            return const_cast<int8_t *>(column_data_.at(col).data() + row * field_sizes_bytes_.at(col));
        }

        SecureTable *secretShare();


        SecretShares
        generateSecretShares() const; // generate shares for alice and bob - for data sharing (non-computing) node



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
        PlainTable *revealInsecure(const int & party = emp::PUBLIC) const;

        inline PlainTuple revealRow(const int & row,  QuerySchema & dst_schema, const int & party = PUBLIC) const  {
            if(std::is_same_v<B, bool>) {
                return getPlainTuple(row);
            }

            PlainTuple plain(&dst_schema);
            int field_cnt = schema_.getFieldCount();
            for(int i = 0; i < field_cnt; i++) {
                PlainField p = getField(row, i).reveal(schema_.getField(i), party);
                plain.setField(i, p);
            }

            PlainField dummy_tag = getField(row, -1).reveal(schema_.getField(-1), party);
            plain.setDummyTag(dummy_tag);

            return plain;
        }



        inline bool empty() const { return tuple_cnt_ == 0; }


        QueryTuple<bool> getPlainTuple(size_t idx) const;

        // memcpy a field from one table to another
        void assignField(const int & dst_row, const int & dst_col,const  QueryTable<B> *src, const int & src_row, const int & src_col);
        // conditional write
        void assignField(const emp::Bit & write, const int & dst_row, const int & dst_col,const  QueryTable<B> *src, const int & src_row, const int & src_col);

        void cloneFields(const int &dst_row, const int &dst_start_col, const QueryTable<B> *src, const int &src_row, const int & src_start_col,
                         const int &col_cnt);

        void cloneRow(const int & dst_row, const int & dst_col, const QueryTable<B> * src, const int & src_row);
        void cloneRow(const bool & write, const int & dst_row, const int & dst_col, const QueryTable<B> * src, const int & src_row);
        void cloneRow(const Bit & write, const int & dst_row, const int & dst_col, const QueryTable<B> * src, const int & src_row);
        void cloneColumn(const int & dst_col, const QueryTable<B> *src, const int & src_col);
        void cloneTable(const int & dst_row, QueryTable<B> *src);

        // for serializing a row from its wire packed form
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

            Bit *cursor = dst.bits.data();
            for(int i = 0; i < col_cnt; ++i) {
                SecureField f = src->getField(row, i);
                QueryFieldDesc desc = schema_.fields_.at(i);

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
            return dst;
        }



        vector<int8_t> unpackRowBytes(const int & row, const int & col_cnt) const {
             int read_len = 0;
             for(int i = 0; i < col_cnt; ++i) {
                 read_len += field_sizes_bytes_.at(i);
             }
             vector<int8_t> dst(read_len);
             int8_t *cursor = dst.data();

             for(int i = 0; i < col_cnt; ++i) {
                 int write_len = field_sizes_bytes_.at(i);
                 memcpy(cursor, getFieldPtr(row, i), write_len);
                 cursor += write_len;
             }

             return dst;

        }

        // unpack entire row
        vector<int8_t> unpackRowBytes(const int & row) const {
            vector<int8_t> dst(tuple_size_bytes_);
            int col_cnt = schema_.getFieldCount();
            int8_t *cursor = dst.data();

            for(int i = 0; i < col_cnt; ++i) {
                int write_len = field_sizes_bytes_.at(i);
                memcpy(cursor, getFieldPtr(row, i), write_len);
                cursor += write_len;
            }

            // copy dummy tag to last slot
            memcpy(cursor, getFieldPtr(row, -1), field_sizes_bytes_.at(-1));
            return dst;
        }

         void packRow(const int & row, Integer src) {
           assert(isEncrypted());
           auto dst = (SecureTable *) this;
            Bit *cursor = src.bits.data();

            for(int i = 0; i < schema_.getFieldCount(); ++i) {
                QueryFieldDesc desc = schema_.getField(i);
                switch(desc.getType()) {
                    case FieldType::SECURE_BOOL: {
                        Bit b = *cursor;
                        SecureField f(desc.getType(), b);
                        dst->setField(row, i, f);
                        break;
                    }
                    case FieldType::SECURE_INT:
                    case FieldType::SECURE_LONG:
                    case FieldType::SECURE_STRING: {
                        Integer i_field(desc.size() + desc.bitPacked(), 0, PUBLIC);
                        memcpy(i_field.bits.data(), cursor, desc.size() * sizeof(Bit));
                        SecureField f(desc.getType(), i_field, desc.getStringLength());
                        dst->setField(row, i, f);
                        break;
                    }
                    case FieldType::SECURE_FLOAT: {
                        Float f_field;
                        memcpy(f_field.value.data(), cursor, desc.size() * sizeof(Bit));
                        SecureField f(desc.getType(), f_field);
                        dst->setField(row, i, f);
                        break;
                    }
                    default:
                        throw;

                }
                cursor += desc.size();

            }

            // do dummy tag last
            Bit dummy_tag = *cursor;
            dst->setDummyTag(row, dummy_tag);
        }

        QueryTuple<B> getRow(const int & idx);
        void setRow(const int & idx, const QueryTuple<B> &tuple);

        QueryTable<B> *clone() {
            return new QueryTable<B>(*this);
        }

        void compareSwap(const bool & swap, const int  & lhs_row, const int & rhs_row);
        void compareSwap(const Bit & swap, const int  & lhs_row, const int & rhs_row);

        inline virtual StorageModel storageModel() const { return StorageModel::COLUMN_STORE; }

        bool operator==(const QueryTable<B> &other) const;
        string toString(const bool &show_dummies = false) const;
        string toString(const size_t &limit, const bool &show_dummies = false) const;




        void setSchema(const QuerySchema &schema) {
            schema_ = schema;

            if(isEncrypted()) {
                // covers dummy tag as -1
                tuple_size_bytes_ = 0;
                QueryFieldDesc desc;
                bool packed_wires = SystemConfiguration::getInstance().wire_packing_enabled_ && (std::is_same_v<B, Bit>);
                int field_size_bytes;

                for(auto pos : schema_.offsets_) {
                    desc = schema_.getField(pos.first);
                    field_size_bytes = (packed_wires) ? desc.packedWires()  : desc.size();
                    field_size_bytes *=  (packed_wires) ? sizeof(emp::OMPCPackedWire) : sizeof(emp::Bit);
                    tuple_size_bytes_ += field_size_bytes;
                    // offset units are packed wires for OMPC (Bits o.w.)
                    field_offsets_bytes_[pos.first] = pos.second * sizeof(emp::Bit);
                    field_sizes_bytes_[pos.first] = field_size_bytes;
                }
                return;
            }


            // plaintext case
            tuple_size_bytes_ = schema_.size() / 8; // bytes for plaintext

            for(auto pos : schema_.offsets_) {
                field_offsets_bytes_[pos.first] = pos.second / 8;
                field_sizes_bytes_[pos.first] = schema_.getField(pos.first).size() / 8;
            }


        }

    protected:
        SortDefinition order_by_;
        QuerySchema schema_;





    };



    static std::ostream &operator<<(std::ostream &os, const QueryTable<bool> &table)   {
        os << table.getSchema() << " isEncrypted? false, order by: " << DataUtilities::printSortDefinition(table.getSortOrder()) << std::endl;

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
