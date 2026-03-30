#include <query_table/query_schema.h>
#include "field_utilities.h"
#include "data_utilities.h"
#include <query_table/query_table.h>
#include <query_table/plain_tuple.h>
#include <query_table/secure_tuple.h>
#include <sstream>
#include <operators/operator.h>

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
            return sizeof(emp::Bit) * 32;
        case FieldType::SECURE_LONG:
            return sizeof(emp::Bit) * 64;
        case FieldType::SECURE_STRING:
            return sizeof(emp::Bit) * str_length * 8;
        case FieldType::INVALID:
        default: // unsupported type
            throw;

    }
}

emp::Float FieldUtilities::toFloat(const emp::Integer &input) {
    const Integer zero(32, 0, PUBLIC);
    const Integer one(32, 1, PUBLIC);
    const Integer max_int(32, 1 << 24, PUBLIC); // 2^24
    const Integer min_int = Integer(32, -1 * (1 << 24), PUBLIC); // -2^24
    const Integer twenty_three(32, 23, PUBLIC);

    Float output(0.0, PUBLIC);

    Bit sign_bit = input.bits[31];
    Integer unsigned_input = input.abs();

    Integer first_1_idx = Integer(32, 31, PUBLIC) - unsigned_input.leading_zeros().resize(32);

    Bit left_shift = first_1_idx >= twenty_three;
    Integer shift_offset = emp::If(left_shift, first_1_idx - twenty_three, twenty_three - first_1_idx);
    Integer shifted = emp::If(left_shift, unsigned_input >> shift_offset, unsigned_input << shift_offset);

    // exponent is biased by 127
    Integer exponent = first_1_idx + Integer(32, 127, PUBLIC);
    // move exp to the right place in final output
    exponent = exponent << 23;

    Integer coefficient = shifted;
    // clear leading 1 (bit #23) (it will implicitly be there but not stored)
    coefficient.bits[23] = Bit(false, PUBLIC);


    // bitwise OR the sign bit | exp | coeff
    Integer output_int(32, 0, PUBLIC);
    output_int.bits[31] = sign_bit; // bit 31 is sign bit

    output_int = coefficient | exponent | output_int;
    memcpy(&(output.value[0]), &(output_int.bits[0]), 32 * sizeof(Bit));

    // cover the corner cases
    output = emp::If(input == zero, Float(0.0, PUBLIC), output);
    output = emp::If(input < min_int, Float(INT_MIN, PUBLIC), output);
    output = emp::If(input > max_int, Float(INT_MAX, PUBLIC), output);

    return output;
}


void FieldUtilities::secret_share_send(const PlainTable *src, const int &src_idx, SecureTable *dst, const int &dst_idx,
                                       const int &party) {
    QuerySchema dst_schema = dst->getSchema();
    size_t field_count = dst_schema.getFieldCount();

    for (size_t i = 0; i < field_count; ++i) {
        PlainField src_field = src->getField(src_idx, i);
        QueryFieldDesc dst_field_desc = dst_schema.getField(i);
        SecureField dst_field = SecureField::secret_share_send(src_field, dst_field_desc, party);
        dst->setField(dst_idx, i, dst_field);
    }

    emp::Bit b(src->getDummyTag(src_idx), party);
    dst->setDummyTag(dst_idx, b);

}

void FieldUtilities::secret_share_recv(SecureTable *dst, const int & idx, const int &party) {
    QuerySchema dst_schema = dst->getSchema();
    size_t field_count = dst_schema.getFieldCount();

    for(size_t i = 0;  i < field_count; ++i) {
        SecureField  dst_field = SecureField::secret_share_recv(dst_schema.getField(i), party);
        dst->setField(idx, i, dst_field);
    }

    emp::Bit b(0, party);
    dst->setDummyTag(idx, b);


}






std::string FieldUtilities::printTupleBits(const PlainTuple & p) {
    int bit_size = p.getSchema()->size();
    bool *bits = Utilities::bytesToBool(p.getData(), bit_size/8);
    std::stringstream s;
    for(int i = 0; i < bit_size; ++i) {
        s << bits[i];
        if(i % 8 == 0) s << " ";
    }

    delete [] bits;
    return s.str();
}

std::string FieldUtilities::printTupleBits(const SecureTuple & s) {
    QuerySchema dst_schema = QuerySchema::toPlain(*s.getSchema());
    PlainTuple tmp = s.reveal(&dst_schema, PUBLIC);
    return printTupleBits(tmp);
}

// unioned db name
BitPackingMetadata FieldUtilities::getBitPackingMetadata(const std::string & db_name) {
    PlainTable *p;
    BitPackingMetadata bit_packing;

    try {
        string query = "SELECT table_name, col_name, min, max, domain_size FROM bit_packing";
        p = DataUtilities::getQueryResults(db_name, query, false);
    } catch (std::exception e) {
        return bit_packing; //  skip this step if the table isn't configured
    }

    for(int i = 0; i < p->tuple_cnt_; ++i) {
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

        // TODO: correct this in date handling
        // presently extracting dates to epochs
        // can go much faster if we extract them to dates!
        if(table == "orders" && column == "o_orderdate") {
            bp.min_ *= 86400; // 60 * 60 * 24
            bp.max_ *=  86400;
            bp.domain_size_ *= 86400;;

        }
        bit_packing[c] = bp;
    }

    delete p;
    return bit_packing;
}

string FieldUtilities::printFloat(const float_t &f) {
    stringstream ss;
    ss << std::fixed << std::setprecision(10) << f << ": " << DataUtilities::printBitArray((int8_t *) &f, 4) << ", " << DataUtilities::printByteArray((int8_t *) &f, 4);
    return ss.str();
}

string FieldUtilities::printFloat(const Float &f) {
    stringstream ss;
    double tmp = f.reveal<double>();

    Integer ii(32, 0);
    memcpy(ii.bits.data(), f.value.data(), 32*sizeof(Bit)); // sizeof Bit for in-memory representation instead of serialized one
    int32_t i = ii.reveal<int32_t>();

    bool bools[32];
    ii.revealBools(bools, PUBLIC);

    ss << std::fixed << std::setprecision(10) << tmp << ": " << DataUtilities::printBitArray(bools, 32) << ", " << DataUtilities::printByteArray((int8_t *) &i, 4);

    return ss.str();
}

string FieldUtilities::printInt(const int32_t &i) {
    stringstream ss;
    ss << i << ": " << DataUtilities::printBitArray((int8_t *) &i, 4) << ", " << DataUtilities::printByteArray((int8_t *) &i, 4);
    return ss.str();
}

string FieldUtilities::printInt(const Integer &i) {
    int bit_cnt = i.size();
    int64_t ii = i.reveal<int64_t>(); // ballpark - input might be > 64 bits

    stringstream ss;
    ss << ii << ": " << SystemConfiguration::getInstance().emp_manager_->revealToString(i) << ", " << DataUtilities::printByteArray((int8_t *) &ii, ceil(bit_cnt/8.0));

    return ss.str();

}

string FieldUtilities::printString(const string &s) {
    stringstream ss;
    ss << s << ": " << DataUtilities::printBitArray((int8_t *) s.c_str(), s.size()) << ", " << DataUtilities::printByteArray((int8_t *) s.c_str(), s.size());
    return ss.str();
}






