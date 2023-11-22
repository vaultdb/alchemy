#ifndef _SECURE_TUPLE_H
#define _SECURE_TUPLE_H

#include "query_tuple.h"
#include "plain_tuple.h"

namespace vaultdb {

    typedef QueryTuple<emp::Bit> SecureTuple;

    // MPC specialization
    template<>
    class QueryTuple<emp::Bit> {

    public:

        emp::Bit *fields_; // has dummy tag at end, serialized representation, usually points to an offset in QueryTable
        QuerySchema *schema_; // pointer to enclosing table's schema
        emp::Bit *managed_data_ = nullptr;
        // storing byte offsets because OMPC will store this in packed wires - not actually a Bit * array
        map<int, int> field_offset_bytes_;
        int tuple_size_bytes_ = 0;

        QueryTuple() {};
        ~QueryTuple()  { if(managed_data_ != nullptr) delete [] managed_data_; }
        QueryTuple(const QueryTuple & src);

        inline QueryTuple(QuerySchema *query_schema, int8_t *src) : fields_((emp::Bit *) src), schema_(query_schema) {
            assert(fields_ != nullptr);

        }

        inline QueryTuple(QuerySchema *query_schema, emp::Bit *src) : fields_(src), schema_(query_schema) {
            assert(fields_ != nullptr);

        }
        // constructor for fake immutable tuple
        QueryTuple(QuerySchema *query_schema, const int8_t *src);
        explicit QueryTuple(QuerySchema *schema); // self-managed memory

        int8_t *getData() const { return (int8_t *) fields_; }

        inline bool  isEncrypted() const { return true; }

        inline bool hasManagedStorage() const { return managed_data_ != nullptr; }


        const inline SecureField getField(const int & ordinal) const {

            auto cursor = ((int8_t *) fields_) + field_offset_bytes_.at(ordinal);
            auto field_desc = schema_->getField(ordinal);
            auto field_size_bits = field_desc.size();
            auto f_type = field_desc.getType();

            switch(f_type) {
                case FieldType::SECURE_BOOL: {
                    Bit b = *cursor;
                    return SecureField(FieldType::SECURE_BOOL, b);
                }
                case FieldType::SECURE_INT:
                case FieldType::SECURE_LONG:
                case FieldType::SECURE_STRING: {
                    Integer i(field_size_bits + field_desc.bitPacked(), 0L);
                    memcpy(i.bits.data(), cursor, field_size_bits * sizeof(emp::Bit));
                    return SecureField(f_type, i);
                }
                case FieldType::SECURE_FLOAT: {
                    Float f;
                    memcpy(f.value.data(), cursor, field_size_bits * sizeof(emp::Bit));
                    return SecureField(f_type, f);
                }
                default:
                    throw;
            }

        }



        inline void setField(const int &ordinal, const SecureField &f) {
            auto cursor = ((int8_t *) fields_) + field_offset_bytes_[ordinal];
            auto write_size_bytes = schema_->getField(ordinal).size() * sizeof(emp::Bit);

            switch(f.getType()) {
                case FieldType::SECURE_BOOL: {
                    Bit b = f.template getValue<emp::Bit>();
                    memcpy(cursor, &b, write_size_bytes);
                    break;
                }
                case FieldType::SECURE_INT:
                case FieldType::SECURE_LONG:
                case FieldType::SECURE_STRING: {
                    Integer i = f.getInt();
                    memcpy(cursor, i.bits.data(), write_size_bytes);
                    break;
                }
                case FieldType::SECURE_FLOAT: {
                    Float f_tmp = f.getFloat();
                    memcpy(cursor, f_tmp.value.data(), write_size_bytes);
                    break;
                }
                default:
                    throw;
            }

        }


        inline void setDummyTag(const emp::Bit & d) {
            SecureField f(FieldType::SECURE_BOOL, d);
            setDummyTag(f);
        }

        inline void setDummyTag(const bool & b) {  setDummyTag(Bit(b)); }

        inline void setDummyTag(const Field<emp::Bit> & d) {
            setField(-1, d);
        }

        inline void setSchema(QuerySchema *q) { schema_ = q; }


        inline emp::Bit getDummyTag() const {
            auto pos =  (emp::Bit *) (((int8_t *) fields_) + field_offset_bytes_.at(-1));
            return *pos;
        }

        QuerySchema *getSchema() const { return schema_; }

        QueryTuple<bool> reveal(const int &party, QuerySchema *dst_schema, int8_t *dst) const;
        // self-managed storage
        QueryTuple<bool> reveal(QuerySchema *dst_schema, const int &party = emp::PUBLIC) const;
        QueryTuple<bool> revealInsecure(QuerySchema *dst_schema, const int &party = emp::PUBLIC) const;

        string toString(const bool &show_dummies = false) const;

        void serialize(int8_t *dst) {
            memcpy((emp::Bit *) dst, fields_, tuple_size_bytes_); }

        inline size_t getFieldCount() const {
            return schema_->getFieldCount();
        }


        QueryTuple& operator=(const SecureTuple& other);

        emp::Bit operator==(const SecureTuple & other) const;
        emp::Bit operator!=(const SecureTuple & other) const { return  !(*this == other); }

        inline SecureField operator[](const int32_t & idx) { return getField(idx); }
        const inline  SecureField operator[](const int32_t & idx) const { return getField(idx); }

        static void compareSwap(const emp::Bit & cmp, QueryTuple<emp::Bit> & lhs, QueryTuple<emp::Bit> & rhs);

        static SecureTuple deserialize(emp::Bit *dst_tuple_bits, QuerySchema *schema, const emp::Bit *src_tuple_bits);

        static SecureTuple If(const emp::Bit & cond, const SecureTuple & lhs, const SecureTuple & rhs);

    private:
        inline void initializeFieldOffsets() {

            // based on QueryTable::setSchema
            for(auto pos : schema_->offsets_) {
                auto desc = schema_->getField(pos.first);
                auto field_size_bytes = desc.size() * sizeof(emp::Bit);
                tuple_size_bytes_ += field_size_bytes;
                // offset units are packed wires for OMPC (Bits o.w.)
                field_offset_bytes_[pos.first] = pos.second * sizeof(emp::Bit);
            }
        }

    };



    std::ostream &operator<<(std::ostream &os, const SecureTuple &tuple);




}

#endif // _SECURE_TUPLE_H