#include "emp_manager.h"
#include "type_utilities.h"
#include "data_utilities.h"

using namespace vaultdb;

std::shared_ptr<QueryTable> EmpManager:: secretShareTable(const QueryTable *srcTable,  NetIO *netio, int party) {
    size_t aliceSize = srcTable->getTupleCount(); // in tuples
    size_t bobSize = aliceSize;
    int colCount = srcTable->getSchema().getFieldCount();
    QueryTuple dstTuple(colCount, true);

    if (party == emp::ALICE) {
        netio->send_data(&aliceSize, 4);
        netio->flush();
        netio->recv_data(&bobSize, 4);
        netio->flush();
    } else if (party == emp::BOB) {
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
        QueryTuple *srcTuple = (party == emp::ALICE) ? srcTable->getTuplePtr(i) : nullptr;
        dstTuple = secretShareTuple(srcTuple, schema, party, (int) emp::ALICE);
        dstTable->putTuple(i, dstTuple);

    }

    int writeIdx = aliceSize;
    // write bob last --> first to make bitonic sequence
    int readTuple = bobSize; // last tuple

    for (int i = 0; i < bobSize; ++i) {
        --readTuple;
        QueryTuple *srcTuple = (party == emp::BOB) ? srcTable->getTuplePtr(readTuple) : nullptr;
        //if(party == emp::BOB)
        //    std::cout << "Secret sharing: " << *srcTuple << std::endl;
        dstTuple = secretShareTuple(srcTuple, schema,  party, emp::BOB);
        //std::string revealed = dstTuple.reveal(emp::PUBLIC).toString(false);
         //std::cout << "Encrypted: " << dstTuple.reveal(emp::PUBLIC) << std::endl;

        // if(empParty_ == emp::BOB)  assert(revealed == srcTuple->toString());

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


    emp::Bit encryptedDummyTag(dummyTag, dstParty);

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
            emp::Bit eBit(bit, dstParty);
            return types::Value(eBit);
        }
        case vaultdb::types::TypeId::INTEGER32: {
            int32_t value = (myParty == dstParty) ? srcValue.getInt32() : 0;
            emp::Integer intVal(32, value, dstParty);
            //std::cout << "Encrypting int: " << value << " reveals to " << intVal.reveal<int32_t>() << " bits: " << intVal.reveal<std::string>() << std::endl;

            return types::Value(types::TypeId::ENCRYPTED_INTEGER32, intVal);
        }
        case vaultdb::types::TypeId::NUMERIC:
        case vaultdb::types::TypeId::FLOAT32: {
            float value = (myParty == dstParty) ? srcValue.getFloat32() : 0;
            emp::Float floatVal(value, dstParty);
            return types::Value(floatVal);
        }

        case vaultdb::types::TypeId::INTEGER64: {
            int64_t value = (myParty == dstParty) ? srcValue.getInt64() : 0;
            emp::Integer intVal(64, value, dstParty);
            return types::Value(types::TypeId::ENCRYPTED_INTEGER64, intVal);
        }

        case vaultdb::types::TypeId::VARCHAR: {

            std::string valueStr = (myParty == dstParty) ?
                srcValue.getVarchar() :
                std::to_string(0);

            emp::Integer strVal = encryptVarchar(valueStr, length, myParty, dstParty);
            types::Value result(types::TypeId::ENCRYPTED_VARCHAR, strVal);;
            return result;
        }

        default: // unsupported type or it is already encrypted
            throw;
    }


}


emp::Integer EmpManager::encryptVarchar(std::string input, size_t stringBitCount, const int & myParty, const int & dstParty) {


    size_t stringByteCount = stringBitCount / 8;

    assert(input.length() <= stringByteCount);   // while loop will be infinite if the string is already oversized

    while(input.length() != stringByteCount) { // pad it to the right length
        input += " ";
    }

    std::string inputReversed = input;
    std::reverse(inputReversed.begin(), inputReversed.end());
    bool *bools = DataUtilities::bytesToBool((int8_t *) inputReversed.c_str(), stringByteCount);

    emp::Integer result(stringBitCount, 0L, dstParty);
    if(myParty == dstParty) {
        ProtocolExecution::prot_exec->feed((block *)result.bits.data(), dstParty, bools, stringBitCount);
    }
    else {
        ProtocolExecution::prot_exec->feed((block *)result.bits.data(), dstParty, nullptr, stringBitCount);
    }


    delete [] bools;


    return result;

}


