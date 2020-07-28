//
// Created by Jennie Rogers on 7/25/20.
//

#include "emp_manager.h"
#include "type_utilities.h"

EmpManager * EmpManager::instance = nullptr;


std::unique_ptr<QueryTable> EmpManager::secretShareTable(QueryTable *srcTable) {
    size_t rowLength = srcTable->GetSchema()->size();
    size_t aliceSize = srcTable->getTupleCount(); // in tuples
    size_t bobSize = aliceSize;

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


    std::unique_ptr<QueryTable> dstTable(new QueryTable(aliceSize + bobSize, true));

    int readTuple = aliceSize; // last tuple


    for (int i = 0; i < aliceSize; ++i) {
        --readTuple;
        QueryTuple *srcTuple = (party_ == EmpParty::ALICE) ? srcTable->GetTuple(readTuple) : nullptr;
        QueryTuple *dstTuple = dstTable->GetTuple(i);
        secretShareTuple(srcTuple, dstTuple, srcTable->GetSchema(), (int) EmpParty::ALICE);
    }

    int writeIdx = aliceSize;
    for (int i = 0; i < bobSize; ++i) {
        QueryTuple *srcTuple =  (party_ == EmpParty::BOB) ?  srcTable->GetTuple(i) : nullptr;
        QueryTuple *dstTuple = dstTable->GetTuple(writeIdx);
        secretShareTuple(srcTuple, dstTuple, nullptr, (int) EmpParty::BOB);
        ++writeIdx;
    }

    return dstTable;

}



// srcTuple == nullptr if we are collecting secret shares of the other party's private data
void EmpManager::secretShareTuple(QueryTuple *srcTuple, QueryTuple *dstTuple, const QuerySchema *schema, int party) {
    QueryField *dstField;

    if((int) party_ == party)
        std::cout << "Encrypting " << *srcTuple << std::endl;

    for(int i = 0; i < schema->getFieldCount(); ++i) {

        const QueryField *srcField = ((int) party_ == party) ? srcTuple->GetField(i) : nullptr;
        if(party == (int) party_) {
            std::cout << "Encrypting " << *srcField << " of type "
                      << TypeUtilities::getTypeIdString(srcField->GetValue()->getType()) << std::endl;

        }
        dstField = secretShareField(srcField, i, schema->GetField(i)->GetType(), party);
        dstTuple->PutField(i, dstField);
        delete dstField;
    }

    bool dummyTag = srcTuple->GetDummyTag();
    emp::Bit encryptedDummyTag(dummyTag, party);
    types::Value valueBit(encryptedDummyTag);
    dstTuple->SetDummyTag(&valueBit);
}

// slap on the ordinal for Value
QueryField * EmpManager::secretShareField(const QueryField *srcField, int ordinal, types::TypeId type, int party) {
    if(srcField != nullptr)
    std::cout << "Secret sharing field " << srcField->GetValue()->getValueString() << " "
              << TypeUtilities::getTypeIdString(srcField->GetValue()->getType()) << " party: " << party << std::endl;

    types::Value *srcValue = ((int) party_ == party) ? srcField->GetValue() : nullptr;
    types::Value dstValue = secretShareValue(srcValue, type, party);
    return new QueryField(ordinal, dstValue);
}

types::Value EmpManager::secretShareValue(const types::Value *srcValue, types::TypeId type, int party) {

    if((int) party_ == party)
        std::cout << "Secret sharing a "
            << TypeUtilities::getTypeIdString(type) << " party: " << party << std::endl;

    switch (type) {
        case vaultdb::types::TypeId::BOOLEAN: {
            bool bit = ((int) party_ == party) ? srcValue->getBool() : 0;
            emp::Bit eBit(bit, party);
            return types::Value(eBit);
        }
        case vaultdb::types::TypeId::INTEGER32: {
            int32_t value = ((int) party_ == party) ? srcValue->getInt32() : 0;
            emp::Integer intVal(value, party);
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
            emp::Integer intVal(value, party);
            return types::Value(types::TypeId::ENCRYPTED_INTEGER64, intVal);
        }

        case vaultdb::types::TypeId::VARCHAR: {
            std::string valueStr = ((int) party_ == party) ? srcValue->getVarchar() : nullptr;
            emp::Integer strVal(valueStr.size() * 8, valueStr, party);
            return types::Value(types::TypeId::ENCRYPTED_VARCHAR, strVal);
        }

        default: // unsupported type or it is already encrypted
            throw;
    }


}



// decrypt a table to public
std::unique_ptr<QueryTable> EmpManager::revealTable(const QueryTable *srcTable, int party = (int) EmpParty::PUBLIC) {
    int colCount = srcTable->GetSchema()->getFieldCount();
    int tupleCount = srcTable->getTupleCount();
    std::unique_ptr<QueryTable> dstTable(new QueryTable(tupleCount, true));


    for(int i = 0; i < srcTable->getTupleCount(); ++i)  {
        for(int j = 0; j < colCount; ++j) {
            revealField(srcTable, dstTable.get(), i, j, party);

        }

    }

    return dstTable;
}




void EmpManager::revealField(const QueryTable *src, QueryTable *dst, int tupleIdx, int fieldIdx, int party) {
    const QueryField *srcField = src->GetTuple(tupleIdx)->GetField(fieldIdx);
    const types::Value srcValue = srcField->GetValue();
    types::TypeId type = srcValue.getType();

    QueryTuple *dstTuple = dst->GetTuple(tupleIdx);
    QueryField *dstField = nullptr; // initialized below



    switch (type) {
        case vaultdb::types::TypeId::BOOLEAN: {
            const emp::Bit eBit = srcValue.getEmpBit();
            bool bit  = eBit.reveal(party);
            dstField = new QueryField(fieldIdx, bit);
            break;
        }
        case vaultdb::types::TypeId::INTEGER32: {
            const emp::Integer *empInteger = srcValue.getEmpInt();
            int32_t value = empInteger->reveal<int32_t>(party);
            dstField = new QueryField(fieldIdx, value);
            break;
        }
        case vaultdb::types::TypeId::INTEGER64: {
            const emp::Integer *empInteger = srcValue.getEmpInt();
            int64_t value = empInteger->reveal<int64_t>(party);
            dstField = new QueryField(fieldIdx, value);
            break;
        }

        case vaultdb::types::TypeId::NUMERIC:
        case vaultdb::types::TypeId::FLOAT32: {
            emp::Float32 *floatVal = srcValue.getEmpFloat32();
            float value = floatVal->reveal<double_t>(party);
            dstField = new QueryField(fieldIdx, value);
            break;
        }
        case vaultdb::types::TypeId::FLOAT64: {
            emp::Float *floatVal = srcValue.getEmpFloat();
            double value = floatVal->reveal<double_t>(party);
            dstField = new QueryField(fieldIdx, value);
            break;
        }

        case vaultdb::types::TypeId::VARCHAR: {
            const emp::Integer *empInteger = srcValue.getEmpInt();
            std::string strVal = empInteger->reveal<std::string>(party);
            dstField = new QueryField(fieldIdx, strVal);
            break;
        }
        default: // unsupported type or it is already encrypted
            throw;
    }

    dstTuple->PutField(fieldIdx, dstField);

}





