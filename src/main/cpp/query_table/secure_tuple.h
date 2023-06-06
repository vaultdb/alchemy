#ifndef _SECURE_TUPLE_H
#define _SECURE_TUPLE_H

#include "query_tuple.h"
#include "plain_tuple.h"

namespace vaultdb {

    typedef QueryTuple<emp::Bit> SecureTuple;

    // secure specialization
    template<>
    class QueryTuple<emp::Bit> {

    public:

        emp::Bit *fields_; // has dummy tag at end, serialized representation, points to an offset in parent QueryTable
        QuerySchema *schema_; // pointer to enclosing table
        emp::Bit *managed_data_ = nullptr;

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

        emp::Bit *getData() const  { return fields_; }

        inline bool  isEncrypted() const { return true; }

        inline bool hasManagedStorage() const { return managed_data_ != nullptr; }

        inline SecureField getField(const int &ordinal) {
            size_t field_offset = schema_->getFieldOffset(ordinal);
            const emp::Bit *read_ptr = fields_ + field_offset;

            return Field<emp::Bit>::deserialize(schema_->getField(ordinal),
                                                (int8_t *) read_ptr);

        }
        const inline SecureField getField(const int & ordinal) const {
            size_t field_offset = schema_->getFieldOffset(ordinal);
            const emp::Bit *read_ptr = fields_ + field_offset;

            return Field<emp::Bit>::deserialize(schema_->getField(ordinal),
                                                (int8_t *) read_ptr);

        }


        inline void setField(const int &idx, const SecureField &f) {
            size_t field_offset = schema_->getFieldOffset(idx);
            int8_t *write_pos = (int8_t *) (fields_ + field_offset);

            f.serialize(write_pos, schema_->getField(idx));

        }

        inline void setDummyTag(const emp::Bit & d) {
            const emp::Bit *dst = fields_ + schema_->getFieldOffset(-1);
            std::memcpy((int8_t *) dst, (int8_t *) &(d.bit), TypeUtilities::getEmpBitSize());

        }

        inline void setDummyTag(const bool & b) {  setDummyTag(Bit(b)); }

        inline void setDummyTag(const Field<emp::Bit> & d) {
            setDummyTag(d.getValue<emp::Bit>());
        }

        inline void setSchema(QuerySchema *q) { schema_ = q; }


        inline emp::Bit getDummyTag() const {
            const emp::Bit *src = fields_ + schema_->getFieldOffset(-1);
            return emp::Bit(*((emp::Bit *) src));
        }

        QuerySchema *getSchema() const { return schema_; }

        QueryTuple<bool> reveal(const int &party, QuerySchema *dst_schema, int8_t *dst) const;
        // self-managed storage
        QueryTuple<bool> reveal(QuerySchema *dst_schema, const int &party = emp::PUBLIC) const;

        string toString(const bool &showDummies = false) const;

        void serialize(int8_t *dst) {     memcpy((emp::Bit *) dst, fields_, schema_->size()); }

        inline size_t getFieldCount() const {
            return schema_->getFieldCount();
        }


        QueryTuple& operator=(const SecureTuple& other);

        emp::Bit operator==(const SecureTuple & other) const;
        emp::Bit operator!=(const SecureTuple & other) const { return  !(*this == other); }

        inline SecureField operator[](const int32_t & idx) { return getField(idx); }
        const inline  SecureField operator[](const int32_t & idx) const { return getField(idx); }

        static void compareSwap(const emp::Bit & cmp, QueryTuple<emp::Bit> & lhs, QueryTuple<emp::Bit> & rhs);


        static SecureTuple
        deserialize(emp::Bit *dst_tuple_bits, QuerySchema *schema, const emp::Bit *src_tuple_bits);

        static void writeSubset(const SecureTuple & src_tuple, const SecureTuple & dst_tuple, uint32_t src_start_idx, uint32_t src_attr_cnt, uint32_t dst_start_idx);

        static SecureTuple If(const emp::Bit & cond, const SecureTuple & lhs, const SecureTuple & rhs);

    };



    std::ostream &operator<<(std::ostream &os, const SecureTuple &tuple);




}

#endif // _SECURE_TUPLE_H