#include "emp_manager.h"
#include "type_utilities.h"
#include "data_utilities.h"

using namespace vaultdb;
using namespace emp;

std::shared_ptr<QueryTable> EmpManager:: secretShareTable(const QueryTable *srcTable,  NetIO *netio, int party) {
    size_t aliceSize = srcTable->getTupleCount(); // in tuples
    size_t bobSize = aliceSize;
    int colCount = srcTable->getSchema().getFieldCount();
    QueryTuple dstTuple(colCount, true);

    if (party == ALICE) {
        netio->send_data(&aliceSize, 4);
        netio->flush();
        netio->recv_data(&bobSize, 4);
        netio->flush();
    } else if (party == BOB) {
        netio->recv_data(&aliceSize, 4);
        netio->flush();
        netio->send_data(&bobSize, 4);
        netio->flush();
    }


    std::shared_ptr<QueryTable> dstTable(new QueryTable(aliceSize + bobSize, colCount));

    dstTable->setSchema(QuerySchema::toSecure(srcTable->getSchema()));
    dstTable->setSortOrder(srcTable->getSortOrder());

    const QuerySchema *schema = &srcTable->getSchema();




    // read alice in order
    for (int i = 0; i < aliceSize; ++i) {
        QueryTuple *srcTuple = (party == ALICE) ? srcTable->getTuplePtr(i) : nullptr;
        dstTuple = secretShareTuple(srcTuple, schema, party, (int) ALICE);
        dstTable->putTuple(i, dstTuple);

    }

    int writeIdx = aliceSize;
    // write bob last --> first to make bitonic sequence
    int readTuple = bobSize; // last tuple

    for (int i = 0; i < bobSize; ++i) {
        --readTuple;
        QueryTuple *srcTuple = (party == BOB) ? srcTable->getTuplePtr(readTuple) : nullptr;
        //if(party == BOB)
        //    std::cout << "Secret sharing: " << *srcTuple << std::endl;
        dstTuple = secretShareTuple(srcTuple, schema,  party, BOB);
        //std::string revealed = dstTuple.reveal(PUBLIC).toString(false);
         //std::cout << "Encrypted: " << dstTuple.reveal(PUBLIC) << std::endl;

        // if(empParty_ == BOB)  assert(revealed == srcTuple->toString());

        dstTable->putTuple(writeIdx, dstTuple);
        ++writeIdx;
    }

    netio->flush();

    return dstTable;

}



// srcTuple == nullptr if we are collecting secret shares of the other party's private data
QueryTuple EmpManager::secretShareTuple(QueryTuple *srcTuple, const QuerySchema *schema, const int & myParty, const int & dstParty) {
    int fieldCount = schema->getFieldCount();
    QueryTuple dstTuple(fieldCount, true);
    types::TypeId aType;
    size_t aSize;


    for(int i = 0; i < fieldCount; ++i) {

        const QueryField *srcField = (myParty == dstParty) ? srcTuple->getFieldPtr(i) : nullptr;
        aSize = schema->getField(i).size();
        aType = schema->getField(i).getType();
        QueryField dstField = secretShareField(srcField, aType, i, aSize, myParty, dstParty);
        dstTuple.putField(dstField);

    }

    bool dummyTag = 0;
    if(srcTuple != nullptr)
         dummyTag = srcTuple->getDummyTag().getBool();


    Bit encryptedDummyTag(dummyTag, dstParty);

    types::Value valueBit(encryptedDummyTag);
    dstTuple.setDummyTag(valueBit);



    return dstTuple;
}

// slap on the ordinal for Value
QueryField
EmpManager::secretShareField(const QueryField *srcField, const types::TypeId &type, const int32_t &ordinal,
                             size_t length, const int &myParty, const int &dstParty) {

    types::Value srcValue = (myParty == dstParty) ? srcField->getValue() : types::Value((int64_t) 0);


    types::Value dstValue = secretShareValue(srcValue, type, length, myParty, dstParty);
    QueryField dstField(ordinal, dstValue);


    return dstField;
}

types::Value
EmpManager::secretShareValue(const types::Value &srcValue, const types::TypeId &type, size_t length, const int &myParty,
                             const int &dstParty) {

    switch (type) {
        case vaultdb::types::TypeId::BOOLEAN: {
            bool bit = (myParty == dstParty) ? srcValue.getBool() : 0;
            Bit eBit(bit, dstParty);
            return types::Value(eBit);
        }
        case vaultdb::types::TypeId::INTEGER32: {
            int32_t value = (myParty == dstParty) ? srcValue.getInt32() : 0;
            Integer intVal(32, value, dstParty);
            //std::cout << "Encrypting int: " << value << " reveals to " << intVal.reveal<int32_t>() << " bits: " << intVal.reveal<std::string>() << std::endl;

            return types::Value(types::TypeId::ENCRYPTED_INTEGER32, intVal);
        }
        case vaultdb::types::TypeId::NUMERIC:
        case vaultdb::types::TypeId::FLOAT32: {
            float value = (myParty == dstParty) ? srcValue.getFloat32() : 0;
            Float floatVal(value, dstParty);
            return types::Value(floatVal);
        }

        case vaultdb::types::TypeId::INTEGER64: {
            int64_t value = (myParty == dstParty) ? srcValue.getInt64() : 0;
            Integer intVal(64, value, dstParty);
            return types::Value(types::TypeId::ENCRYPTED_INTEGER64, intVal);
        }

        case vaultdb::types::TypeId::VARCHAR: {

            std::string valueStr = (myParty == dstParty) ?
                srcValue.getVarchar() :
                std::to_string(0);

            Integer strVal = encryptVarchar(valueStr, length, myParty, dstParty);
            types::Value result(types::TypeId::ENCRYPTED_VARCHAR, strVal);;
            return result;
        }

        default: // unsupported type or it is already encrypted
            throw;
    }


}


Integer EmpManager::encryptVarchar(std::string input, size_t stringBitCount, const int & myParty, const int & dstParty) {


    size_t stringByteCount = stringBitCount / 8;

    assert(input.length() <= stringByteCount);   // while loop will be infinite if the string is already oversized

    while(input.length() != stringByteCount) { // pad it to the right length
        input += " ";
    }

    std::string inputReversed = input;
    std::reverse(inputReversed.begin(), inputReversed.end());
    bool *bools = DataUtilities::bytesToBool((int8_t *) inputReversed.c_str(), stringByteCount);

    Integer result(stringBitCount, 0L, dstParty);
    if(myParty == dstParty) {
        ProtocolExecution::prot_exec->feed((block *)result.bits.data(), dstParty, bools, stringBitCount);
    }
    else {
        ProtocolExecution::prot_exec->feed((block *)result.bits.data(), dstParty, nullptr, stringBitCount);
    }


    delete [] bools;


    return result;

}

// https://stackoverflow.com/questions/20302904/converting-int-to-float-or-float-to-int-using-bitwise-operations-software-float
Float EmpManager::castIntToFloat(const Integer &input) {
    Float output(0.0, PUBLIC);
    Integer zero(0, PUBLIC);
    Integer one(1, PUBLIC);
    Integer twentyThree(23, PUBLIC);

    Bit signBit = If(input < zero, Bit(true, PUBLIC), Bit(false, PUBLIC));
    Integer unsignedInput = If(signBit, Integer(-1, PUBLIC) * input, input);

    // find leading 1
    Integer bitNum(31, PUBLIC);
    Integer shiftCount = zero;
    int i;
    Bit oneFound(false, PUBLIC);
    Bit predicate = oneFound;
    Integer firstOneIdx(31, PUBLIC); // bit number of first 1 in the mantissa

    for(i = 31; i >= 23; --i) {
        bitNum = Integer(i, PUBLIC);
        predicate = (unsignedInput & (one << i)) != zero;
        predicate = predicate & (!oneFound);

        // need to shift right
        int shiftCount = 23 - i;
        unsignedInput = If(predicate, unsignedInput >> shiftCount, unsignedInput);
        // if this is our first 1, record it
        firstOneIdx = If(!oneFound & predicate, bitNum, firstOneIdx);
        // update the flag for recording first instance of 1 bit
        oneFound = oneFound | predicate;
    }

    for(; i > 0; --i) {
        bitNum = Integer(i, PUBLIC);
        predicate = (unsignedInput & (one << i)) != zero;
        predicate = predicate & (!oneFound);

        // need to shift left
        int shiftCount = 23 - i;
        unsignedInput = If(predicate, unsignedInput << shiftCount, unsignedInput);

        // if this is our first 1, record it
        firstOneIdx = If(!oneFound & predicate, bitNum, firstOneIdx);
        // update the flag for recording first instance of 1 bit
        oneFound = oneFound | predicate;

    }

    // exponent is biased by 127
    Integer exponent = firstOneIdx + Integer(127, PUBLIC);
    // move exp to the right place
    exponent = exponent << 23;

    // clear leading 1 (bit #23) (it will implicitly be there but not stored)
    Integer coefficient = unsignedInput;
    coefficient.bits[22] = Bit(false, PUBLIC);

    // bitwise OR the sign bit | exp | coeff
    Integer outputInt(32, 0, PUBLIC);
    outputInt.bits[FLOAT_LEN-1] = signBit; // bit 31 is sign bit
    outputInt = outputInt | exponent | coefficient;

    // cover the corner cases
    output = If(input == zero, Float(0.0, PUBLIC), output);
    output = If(input == Integer(INT_MIN, PUBLIC), Float((float) INT_MIN, PUBLIC), output);

    return output;

}


