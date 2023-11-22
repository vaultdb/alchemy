#include "field.h"
#include "util/type_utilities.h"

#if __has_include("emp-rescu/emp-rescu.h")

template<typename B>
void Field<B>::serialize(int8_t *dst, const QueryFieldDesc &schema) const {
    assert(dst != nullptr);
    OMPCBackend<N>* protocol = (OMPCBackend<N> *) emp::backend;


    switch (schema.getType()) {
        case FieldType::BOOL:
            *((bool *) dst) = getValue<bool>();
            break;
        case FieldType::INT:
            *((int32_t *) dst) = getValue<int32_t>();
            break;
        case FieldType::LONG:
            *((int64_t *) dst) = getValue<int64_t>();
            break;
        case FieldType::FLOAT:
            *((float_t *) dst) = getValue<float_t>();
            break;

        case FieldType::STRING: {
            string s = getValue<string>();
            std::reverse(s.begin(), s.end()); // reverse it so we can more easily conver to EMP format
            memcpy(dst, (int8_t *) s.c_str(), s.size()); // null termination chopped
            break;
        }
        case FieldType::SECURE_BOOL: {
            Bit b = boost::get<Bit>(payload_);
            protocol->pack(&b, (OMPCPackedWire *) dst, 1);
            break;
        }
        case FieldType::SECURE_INT:
        case FieldType::SECURE_LONG: {
            Integer i = boost::get<Integer>(payload_);
            protocol->pack(i.bits.data(), (OMPCPackedWire *) dst, i.size());
            break;
        }
        case FieldType::SECURE_STRING:
            Field<B>::serializeSecureString(*this, dst);
            break;
        case FieldType::SECURE_FLOAT: {
            emp::Float f = boost::get<emp::Float>(payload_);
            ((OMPCBackend<N> *) emp::backend)->pack(f.value.data(), (OMPCPackedWire *) dst, 32);
            break;
        }
        default:
            throw;
    }
}

template<typename B>
void Field<B>::serializeSecureString(const Field<B> &src, int8_t *dst) {
    assert(src.type_ == FieldType::SECURE_STRING);

    emp::Integer si = boost::get<emp::Integer>(src.payload_);
    int bit_cnt = si.size();
    int wire_cnt =  TypeUtilities::packedWireCount(bit_cnt);

    OMPCPackedWire *write_cursor = (OMPCPackedWire *) dst;
    emp::Bit *read_cursor =  si.bits.data();
    int to_write = bit_cnt;
    int pack_size;

    for(int i = 0; i < wire_cnt; ++i) {
        pack_size = (to_write >= 128)  ? 128 : to_write;
        ((OMPCBackend<N> *) emp::backend)->pack(read_cursor, write_cursor, pack_size);

        ++write_cursor;
        read_cursor += 128;
        to_write -= 128;
    }


}


template<typename B>
Field<B> Field<B>::deserializeSecureString(OMPCPackedWire *src, const int & str_length) {
    OMPCBackend<N> *protocol = (OMPCBackend<N> *) emp::backend;
    size_t bit_cnt = str_length * 8;
    int wire_cnt =  TypeUtilities::packedWireCount(bit_cnt);

    emp::Integer v(bit_cnt, 0, PUBLIC);

    OMPCPackedWire *read_cursor =  src;
    emp::Bit *write_cursor = v.bits.data();
    int to_write = bit_cnt;
    int pack_size;

    for(int i = 0; i < wire_cnt; ++i) {
        pack_size = (to_write >= 128)  ? 128 : to_write;
        protocol->unpack(write_cursor, read_cursor, pack_size);

        ++read_cursor;
        write_cursor += 128;
        to_write -= 128;
    }

    return  Field<B>(FieldType::SECURE_STRING, v, str_length);

}
#endif