#include <query_table/query_schema.h>
#include "field_utilities.h"
#include "data_utilities.h"
#include <query_table/query_table.h>
#include <query_table/plain_tuple.h>
#include <query_table/secure_tuple.h>
#include <sstream>


using namespace vaultdb;
using namespace  emp;

size_t FieldUtilities::getPhysicalSize(const FieldType &id, const size_t &str_length) {
    switch (id) {
        case FieldType::BOOL:
            return sizeof(bool); // stored size when we serialize it
        case FieldType::INT:
            return sizeof(int32_t);
        case FieldType::LONG:
            return sizeof(int64_t);
        case FieldType::FLOAT:
            return sizeof(float_t);
        case FieldType::STRING:
            return str_length;
        case FieldType::SECURE_BOOL:
            return sizeof(emp::block);
        case FieldType::SECURE_INT:
        case FieldType::SECURE_FLOAT:
            return sizeof(emp::block) * 32;
        case FieldType::SECURE_LONG:
            return sizeof(emp::block) * 64;

        case FieldType::SECURE_STRING:
            return sizeof(emp::block) * str_length * 8;
        case FieldType::INVALID:
        default: // unsupported type
            throw;

    }
}

emp::Float FieldUtilities::toFloat(const emp::Integer &input) {
    const Integer zero(32, 0, PUBLIC);
    const Integer one(32, 1, PUBLIC);
    const Integer maxInt(32, 1 << 24, PUBLIC); // 2^24
    const Integer minInt = Integer(32, -1 * (1 << 24), PUBLIC); // -2^24
    const Integer twentyThree(32, 23, PUBLIC);

    Float output(0.0, PUBLIC);

    Bit signBit = input.bits[31];
    Integer unsignedInput = input.abs();

    Integer firstOneIdx = Integer(32, 31, PUBLIC) - unsignedInput.leading_zeros().resize(32);

    Bit leftShift = firstOneIdx >= twentyThree;
    Integer shiftOffset = emp::If(leftShift, firstOneIdx - twentyThree, twentyThree - firstOneIdx);
    Integer shifted = emp::If(leftShift, unsignedInput >> shiftOffset, unsignedInput << shiftOffset);

    // exponent is biased by 127
    Integer exponent = firstOneIdx + Integer(32, 127, PUBLIC);
    // move exp to the right place in final output
    exponent = exponent << 23;

    Integer coefficient = shifted;
    // clear leading 1 (bit #23) (it will implicitly be there but not stored)
    coefficient.bits[23] = Bit(false, PUBLIC);


    // bitwise OR the sign bit | exp | coeff
    Integer outputInt(32, 0, PUBLIC);
    outputInt.bits[31] = signBit; // bit 31 is sign bit

    outputInt =  coefficient | exponent | outputInt;
    memcpy(&(output.value[0]), &(outputInt.bits[0]), 32 * sizeof(Bit));

    // cover the corner cases
    output = emp::If(input == zero, Float(0.0, PUBLIC), output);
    output = emp::If(input < minInt, Float(INT_MIN, PUBLIC), output);
    output = emp::If(input > maxInt, Float(INT_MAX, PUBLIC), output);

    return output;
}


//void FieldUtilities::secret_share_send(const PlainTuple & src_tuple, SecureTuple & dst_tuple, const int & party) {
//    size_t field_count = dst_tuple.getSchema()->getFieldCount();
//    for (size_t i = 0; i < field_count; ++i) {
//        PlainField src_field = src_tuple.getField(i);
//        SecureField dst_field = SecureField::secret_share_send(src_field, dst_tuple.getSchema()->getField(i), party);
//        dst_tuple.setField(i, dst_field);
//    }
//
//    emp::Bit b(src_tuple.getDummyTag(), party);
//    dst_tuple.setDummyTag(b);
//
//
//}
//
//void FieldUtilities::secret_share_recv(SecureTuple &dst_tuple, const int &dst_party) {
//    size_t field_count = dst_tuple.getSchema()->getFieldCount();
//
//    for(size_t i = 0;  i < field_count; ++i) {
//        SecureField  dst_field = SecureField::secret_share_recv(dst_tuple.getSchema()->getField(i), dst_party);
//        dst_tuple.setField(i, dst_field);
//    }
//
//    emp::Bit b(0, dst_party);
//    dst_tuple.setDummyTag(b);
//
//}

void FieldUtilities::secret_share_send(const PlainTable *src, const int &src_idx, SecureTable *dst, const int &dst_idx,
                                       const int &party) {
    size_t field_count = dst->getSchema()->getFieldCount();

    for (size_t i = 0; i < field_count; ++i) {
        PlainField src_field = src->getField(src_idx, i);
        SecureField dst_field = SecureField::secret_share_send(src_field, dst->getSchema()->getField(i), party);
        dst->setField(dst_idx, i, dst_field);
    }

    emp::Bit b(src->getDummyTag(src_idx), party);
    dst->setDummyTag(dst_idx, b);

}

void FieldUtilities::secret_share_recv(SecureTable *dst, const int & idx, const int &party) {

    size_t field_count = dst->getSchema()->getFieldCount();

    for(size_t i = 0;  i < field_count; ++i) {
        SecureField  dst_field = SecureField::secret_share_recv(dst->getSchema()->getField(i), party);
        dst->setField(idx, i, dst_field);
    }

    emp::Bit b(0, party);
    dst->setDummyTag(idx, b);


}


// to compute sort
bool FieldUtilities::select(const bool &choice, const bool &lhs, const bool &rhs) {
    return choice ? lhs : rhs;
}

emp::Bit FieldUtilities::select(const Bit &choice, const Bit &lhs, const Bit &rhs) {
    return emp::If(choice, lhs, rhs);
}


PlainTuple FieldUtilities::revealTuple(const SecureTuple & s) {
    std::shared_ptr<QuerySchema> secure_schema = s.getSchema();
    std::shared_ptr<QuerySchema> plain_schema =  std::make_shared<QuerySchema>(QuerySchema::toPlain(*secure_schema));
    return s.reveal(plain_schema, emp::PUBLIC);
}


std::string FieldUtilities::printTupleBits(const PlainTuple & p) {
    int bit_size = p.getSchema()->size();
    bool *bits = Utilities::bytesToBool(p.getData(), bit_size/8);
    std::stringstream s;
    for(int i = 0; i < bit_size; ++i) {
        s << bits[i];
    }

    delete [] bits;
    return s.str();
}

std::string FieldUtilities::printTupleBits(const SecureTuple & s) {
    int bit_size = s.getSchema()->size();
    Integer i(bit_size, 0);
    memcpy(i.bits.data(), s.getData(), bit_size);
    return  i.reveal<std::string>();
}

// unioned db name
BitPackingMetadata FieldUtilities::getBitPackingMetadata(const std::string & db_name) {
    std::shared_ptr<PlainTable> p;
    BitPackingMetadata bit_packing;

    try {
        string query = "SELECT table_name, col_name, min, max, domain_size FROM bit_packing";
        p =  DataUtilities::getQueryResults(db_name, query, false);
    } catch (std::exception e) {
        return bit_packing; //  skip this step if the table isn't configured
    }

    for(int i = 0; i < p->getTupleCount(); ++i) {
        //PlainTuple t = p->getTuple(i);

        BitPackingDefinition bp;
        string table = p->getField(i, 0).toString();
        string column = p->getField(i, 1).toString();

        table.erase(table.find_last_not_of(" \t\n\r\f\v") + 1); // delete trailing spaces
        column.erase(column.find_last_not_of(" \t\n\r\f\v") + 1);

        ColumnReference c(table, column);

        bp.min_ = p->getField(i, 2).getValue<int>();
        bp.max_ = p->getField(i, 3).getValue<int>();
        bp.domain_size_ =  p->getField(i, 4).getValue<int>();

        bit_packing[c] = bp;
    }

    return bit_packing;
}




