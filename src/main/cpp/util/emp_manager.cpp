//
// Created by Jennie Rogers on 7/25/20.
//

#include "emp_manager.h"
#include "type_utilities.h"
#include "data_utilities.h"

EmpManager * EmpManager::instance_ = nullptr;


std::shared_ptr<QueryTable> EmpManager::secretShareTable(const QueryTable *srcTable) {
    size_t aliceSize = srcTable->getTupleCount(); // in tuples
    size_t bobSize = aliceSize;
    int colCount = srcTable->getSchema().getFieldCount();
    QueryTuple dstTuple(colCount, true);

    if (empParty_ == emp::ALICE) {
        netio_->send_data(&aliceSize, 4);
        netio_->flush();
        netio_->recv_data(&bobSize, 4);
        netio_->flush();
    } else if (empParty_ == emp::BOB) {
        netio_->recv_data(&aliceSize, 4);
        netio_->flush();
        netio_->send_data(&bobSize, 4);
        netio_->flush();
    }


    std::shared_ptr<QueryTable> dstTable(new QueryTable(aliceSize + bobSize, colCount, true));

    dstTable->setSchema(srcTable->getSchema());



    // read alice in order
    for (int i = 0; i < aliceSize; ++i) {
        QueryTuple *srcTuple = (empParty_ == emp::ALICE) ? srcTable->getTuplePtr(i) : nullptr;
        dstTuple = secretShareTuple(srcTuple, &srcTable->getSchema(), (int) emp::ALICE);
        dstTable->putTuple(i, dstTuple);

    }

    netio_->flush();

    int writeIdx = aliceSize;
    // write bob last --> first to make bitonic sequence
    int readTuple = bobSize; // last tuple

    for (int i = 0; i < bobSize; ++i) {
        --readTuple;
        QueryTuple *srcTuple = (empParty_ == emp::BOB) ? srcTable->getTuplePtr(readTuple) : nullptr;
        if(empParty_ == emp::BOB)
            std::cout << "Secret sharing: " << *srcTuple << std::endl;
        dstTuple = secretShareTuple(srcTuple, &srcTable->getSchema(),  emp::BOB);
        std::string revealed = dstTuple.reveal(emp::PUBLIC).toString(false);
         std::cout << "Encrypted: " << dstTuple.reveal(emp::PUBLIC) << std::endl;

        // if(empParty_ == emp::BOB)  assert(revealed == srcTuple->toString());

        dstTable->putTuple(writeIdx, dstTuple);
        ++writeIdx;
    }

    netio_->flush();

    return dstTable;

}



// srcTuple == nullptr if we are collecting secret shares of the other party's private data
QueryTuple EmpManager::secretShareTuple(QueryTuple *srcTuple, const QuerySchema *schema, int party) {
    int fieldCount = schema->getFieldCount();
    QueryTuple dstTuple(fieldCount, true);

    for(int i = 0; i < fieldCount; ++i) {

        const QueryField *srcField = (empParty_ == party) ? srcTuple->getFieldPtr(i) : nullptr;
        QueryField dstField = secretShareField(srcField, i, schema->getField(i).getType(), schema->getField(i).size(), party);
        dstTuple.putField(dstField);

    }

    bool dummyTag = 0;
    if(srcTuple != nullptr)
         dummyTag = srcTuple->getDummyTag().getBool();


    emp::Bit encryptedDummyTag(dummyTag, party);

    types::Value valueBit(encryptedDummyTag);
    dstTuple.setDummyTag(valueBit);



    return dstTuple;
}

// slap on the ordinal for Value
QueryField
EmpManager::secretShareField(const QueryField *srcField, int ordinal, types::TypeId type, size_t length, int party) {

    types::Value srcValue = ((int) empParty_ == party) ? srcField->getValue() : types::Value(0);


    types::Value dstValue = secretShareValue(srcValue, type, length, party);
    QueryField dstField(ordinal, dstValue);


    return dstField;
}

types::Value EmpManager::secretShareValue(const types::Value &srcValue, types::TypeId type, size_t length, int party) {


    switch (type) {
        case vaultdb::types::TypeId::BOOLEAN: {
            bool bit = ((int) empParty_ == party) ? srcValue.getBool() : 0;
            emp::Bit eBit(bit, party);
            return types::Value(eBit);
        }
        case vaultdb::types::TypeId::INTEGER32: {
            int32_t value = ((int) empParty_ == party) ? srcValue.getInt32() : 0;
            emp::Integer intVal(32, value, party);
            return types::Value(types::TypeId::ENCRYPTED_INTEGER32, intVal);
        }
        case vaultdb::types::TypeId::NUMERIC:
        case vaultdb::types::TypeId::FLOAT32: {
            float value = ((int) empParty_ == party) ? srcValue.getFloat32() : 0;
            emp::Float floatVal(value, party);
            return types::Value(floatVal);
        }

        case vaultdb::types::TypeId::INTEGER64: {
            int64_t value = ((int) empParty_ == party) ? srcValue.getInt64() : 0;
            emp::Integer intVal(64, value, party);
            return types::Value(types::TypeId::ENCRYPTED_INTEGER64, intVal);
        }

        case vaultdb::types::TypeId::VARCHAR: {

            std::string valueStr = ((int) empParty_ == party) ?
                srcValue.getVarchar() :
                std::to_string(0);

            emp::Integer strVal = encryptVarchar(valueStr, length, party);
            types::Value result(types::TypeId::ENCRYPTED_VARCHAR, strVal);;
            return result;
        }

        default: // unsupported type or it is already encrypted
            throw;
    }


}


emp::Integer EmpManager::encryptVarchar(std::string input, size_t stringBitCount, int party) {

    size_t stringByteCount = stringBitCount / 8;

    assert(input.length() <= stringByteCount);   // while loop will be infinite if the string is already oversized

    while(input.length() != stringByteCount) { // pad it to the right length
        input += " ";
    }

    bool *bools = DataUtilities::bytesToBool((int8_t *) input.c_str(), stringByteCount);

    emp::Integer result(stringBitCount, 0L, party);
    if(party == (int) empParty_) {
        ProtocolExecution::prot_exec->feed((block *)result.bits.data(), party, bools, stringBitCount);
    }
    else {
        ProtocolExecution::prot_exec->feed((block *)result.bits.data(), party, nullptr, stringBitCount);
    }

    flush();

    delete [] bools;


    return result;

}


void EmpManager::flush() {
    netio_->flush();
}

// reset
void EmpManager::close() {

    flush();
    delete netio_;

}

EmpManager::~EmpManager() {
    if(netio_) {
       close();
    }

    if(instance_) delete instance_;
}

void EmpManager::configureEmpManager(const char *aliceHost, int port, int empParty) {

        // TODO(jennie): debug this triggering at setup time
        //  if(netio_) delete netio_;


        std::cout << "Instantiating emp with host=" << aliceHost << " port=" << port << " empParty=" <<  empParty << std::endl;
        netio_ =  new emp::NetIO(empParty == emp::ALICE ? nullptr : aliceHost, port);
        empParty_ = empParty;
        emp::setup_semi_honest(netio_, (int) empParty_);

}




