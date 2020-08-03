//
// Created by Jennie Rogers on 7/25/20.
//

#include "emp_manager.h"
#include "type_utilities.h"

EmpManager * EmpManager::instance = nullptr;


std::unique_ptr<QueryTable> EmpManager::secretShareTable(QueryTable *srcTable) {
    size_t aliceSize = srcTable->getTupleCount(); // in tuples
    size_t bobSize = aliceSize;
    int colCount = srcTable->GetSchema()->getFieldCount();
    QueryTuple dstTuple(colCount, true);

    if (party_ == EmpParty::ALICE) {
        netio_->send_data(&aliceSize, 4);
        netio_->flush();
        netio_->recv_data(&bobSize, 4);
        netio_->flush();
    } else if (party_ == EmpParty::BOB) {
        netio_->recv_data(&aliceSize, 4);
        netio_->flush();
        netio_->send_data(&bobSize, 4);
        netio_->flush();
    }


    std::unique_ptr<QueryTable> dstTable(new QueryTable(aliceSize + bobSize, colCount, true));

    dstTable->SetSchema(srcTable->GetSchema());

    int readTuple = aliceSize; // last tuple


    for (int i = 0; i < aliceSize; ++i) {
        --readTuple;
        QueryTuple *srcTuple = (party_ == EmpParty::ALICE) ? srcTable->GetTuple(readTuple) : nullptr;
        dstTuple = secretShareTuple(srcTuple,srcTable->GetSchema(), (int) EmpParty::ALICE);
        dstTable->putTuple(i, dstTuple);
    }

    netio_->flush();
    std::cout << "Secret sharing bob's data!" << std::endl;

    int writeIdx = aliceSize;
    for (int i = 0; i < bobSize; ++i) {
        QueryTuple *srcTuple =  (party_ == EmpParty::BOB) ?  srcTable->GetTuple(i) : nullptr;
        dstTuple = secretShareTuple(srcTuple, srcTable->GetSchema(), (int) EmpParty::BOB);
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
        const QueryField *srcField = ((int) party_ == party) ? srcTuple->GetField(i) : nullptr;
        QueryField dstField(secretShareField(srcField, i, schema->GetField(i)->GetType(), schema->GetField(i)->size(), party));
        dstTuple.PutField(i, &dstField);
    }

    bool dummyTag = srcTuple->GetDummyTag();
    emp::Bit encryptedDummyTag(dummyTag, party);
    types::Value valueBit(encryptedDummyTag);
    dstTuple.SetDummyTag(&valueBit);

    return dstTuple;
}

// slap on the ordinal for Value
QueryField
EmpManager::secretShareField(const QueryField *srcField, int ordinal, types::TypeId type, size_t length, int party) {

    types::Value *srcValue = ((int) party_ == party) ? srcField->GetValue() : nullptr;

    if(srcValue != nullptr)
        std::cout << "Secret sharing " << srcValue->getValueString() << std::endl;

    types::Value dstValue = secretShareValue(srcValue, type, length, party);
    QueryField dstField(ordinal, dstValue);
    return dstField;
}

types::Value EmpManager::secretShareValue(const types::Value *srcValue, types::TypeId type, size_t length, int party) {


    switch (type) {
        case vaultdb::types::TypeId::BOOLEAN: {
            bool bit = ((int) party_ == party) ? srcValue->getBool() : 0;
            emp::Bit eBit(bit, party);
            return types::Value(eBit);
        }
        case vaultdb::types::TypeId::INTEGER32: {
            int32_t value = ((int) party_ == party) ? srcValue->getInt32() : 0;
            emp::Integer intVal(32, value, party);
            return types::Value(types::TypeId::ENCRYPTED_INTEGER32, intVal);
        }
        case vaultdb::types::TypeId::NUMERIC:
        case vaultdb::types::TypeId::FLOAT32: {
            float value = ((int) party_ == party) ? srcValue->getFloat32() : 0;
            emp::Float32 floatVal(value, party);
            return types::Value(floatVal);
        }
        case vaultdb::types::TypeId::FLOAT64: {
            double value =  ((int) party_ == party) ? srcValue->getFloat64() : 0;
            emp::Float floatVal(24, 9, value, party);
            return types::Value(floatVal);
        }
        case vaultdb::types::TypeId::INTEGER64: {
            int64_t value =  ((int) party_ == party) ? srcValue->getInt64(): 0;
            emp::Integer intVal(64, value, party);
            return types::Value(types::TypeId::ENCRYPTED_INTEGER64, intVal);
        }

        case vaultdb::types::TypeId::VARCHAR: {

            std::string valueStr = ((int) party_ == party) ?
                srcValue->getVarchar() :
                std::to_string(0);
            emp::Integer strVal(length, valueStr, party);
            return types::Value(types::TypeId::ENCRYPTED_VARCHAR, strVal);
        }

        default: // unsupported type or it is already encrypted
            throw;
    }


}




void EmpManager::flush() {
    netio_->flush();
}

// reset
void EmpManager::close() {

    flush();
    delete netio_;

}





