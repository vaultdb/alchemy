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


    std::cout << "Allocating " << aliceSize + bobSize << " tuples in encrypted query table." << std::endl;
    std::unique_ptr<QueryTable> dstTable(new QueryTable(aliceSize + bobSize, colCount, true));

    dstTable->SetSchema(srcTable->GetSchema());

    int readTuple = aliceSize; // last tuple


    for (int i = 0; i < aliceSize; ++i) {
        --readTuple;
        QueryTuple *srcTuple = (party_ == EmpParty::ALICE) ? srcTable->GetTuple(readTuple) : nullptr;
        dstTuple = secretShareTuple(srcTuple,srcTable->GetSchema(), (int) EmpParty::ALICE);

        std::cout << " have new tuple: " << dstTuple << std::endl;
        dstTable->putTuple(i, dstTuple);
    }

    std::cout << "Secret sharing bob's data!" << std::endl;
    int writeIdx = aliceSize;
    for (int i = 0; i < bobSize; ++i) {
        QueryTuple *srcTuple =  (party_ == EmpParty::BOB) ?  srcTable->GetTuple(i) : nullptr;
        dstTuple = secretShareTuple(srcTuple, srcTable->GetSchema(), (int) EmpParty::BOB);
        dstTable->putTuple(writeIdx, dstTuple);
        ++writeIdx;
    }

    return dstTable;

}



// srcTuple == nullptr if we are collecting secret shares of the other party's private data
QueryTuple EmpManager::secretShareTuple(QueryTuple *srcTuple, const QuerySchema *schema, int party) {
    int fieldCount = schema->getFieldCount();
    QueryTuple dstTuple(fieldCount, true);


    if((int) party_ == party)
        std::cout << "Encrypting " << *srcTuple << std::endl;

    for(int i = 0; i < fieldCount; ++i) {
        QueryField dstField;
        const QueryField *srcField = ((int) party_ == party) ? srcTuple->GetField(i) : nullptr;
        if(party == (int) party_) {
            std::cout << "Encrypting " << *srcField << " of type "
                      << TypeUtilities::getTypeIdString(srcField->GetValue()->getType()) << std::endl;

        }
        QueryField tmp(secretShareField(srcField, i, schema->GetField(i)->GetType(), party));
        std::cout << "Encrypted  field: " << tmp << " of type " << TypeUtilities::getTypeIdString(tmp.GetValue()->getType()) << " writing to field " << i << std::endl;
        dstTuple.PutField(i, &tmp);
    }

    bool dummyTag = srcTuple->GetDummyTag();
    emp::Bit encryptedDummyTag(dummyTag, party);
    types::Value valueBit(encryptedDummyTag);
    dstTuple.SetDummyTag(&valueBit);

    std::cout << "Secret shared tuple: " << dstTuple << std::endl;
    return dstTuple;
}

// slap on the ordinal for Value
QueryField EmpManager::secretShareField(const QueryField *srcField, int ordinal, types::TypeId type, int party) {
    if(srcField != nullptr)
    std::cout << "Secret sharing field " << srcField->GetValue()->getValueString() << " "
              << TypeUtilities::getTypeIdString(srcField->GetValue()->getType()) << " party: " << party << std::endl;

    types::Value *srcValue = ((int) party_ == party) ? srcField->GetValue() : nullptr;
    types::Value dstValue = secretShareValue(srcValue, type, party);
    QueryField dstField(ordinal, dstValue);
    std::cout << "**Encrypted  field: " << dstField << " of type " << TypeUtilities::getTypeIdString(dstField.GetValue()->getType()) << " writing to field " << ordinal << std::endl;
    return dstField; // ok through here
}

types::Value EmpManager::secretShareValue(const types::Value *srcValue, types::TypeId type, int party) {

        std::cout << "Secret sharing value "
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

    std::unique_ptr<QueryTable> dstTable(new QueryTable(tupleCount, colCount, true));
    dstTable->SetSchema(srcTable->GetSchema());
    QueryTuple *srcTuple; // initialized below
    QueryField *dstField;

    for(int i = 0; i < srcTable->getTupleCount(); ++i)  {
        srcTuple = srcTable->GetTuple(i);
        QueryTuple dstTuple(srcTuple->getFieldCount(), false);


        for(int j = 0; j < colCount; ++j) {
            dstField = revealField(srcTuple->GetField(j), party);
            dstTuple.PutField(i, dstField);
        }
        dstTable->putTuple(i, dstTuple);

    }

    return dstTable;
}




QueryField * EmpManager::revealField(const QueryField *srcField, int party) {
    const types::Value *srcValue = srcField->GetValue();
    types::TypeId type = srcValue->getType();

    QueryField *dstField = nullptr; // initialized below
    size_t fieldIdx = srcField->getOrdinal();

      switch (type) {
          case vaultdb::types::TypeId::ENCRYPTED_BOOLEAN: {
              const emp::Bit eBit = srcValue->getEmpBit();
              bool bit  = eBit.reveal(party);
              dstField = new QueryField(fieldIdx, bit);
              break;
          }
          case vaultdb::types::TypeId::ENCRYPTED_INTEGER32: {
              const emp::Integer *empInteger = srcValue->getEmpInt();
              int32_t value = empInteger->reveal<int32_t>(party);
              std::cout << "Decrypted " << value << std::endl;
              dstField = new QueryField(fieldIdx, value);
              break;
          }
          case vaultdb::types::TypeId::ENCRYPTED_INTEGER64: {
              const emp::Integer *empInteger = srcValue->getEmpInt();
              int64_t value = empInteger->reveal<int64_t>(party);
              dstField = new QueryField(fieldIdx, value);
              break;
          }

          case vaultdb::types::TypeId::ENCRYPTED_FLOAT32: {
              emp::Float32 *floatVal = srcValue->getEmpFloat32();
              float value = floatVal->reveal<double_t>(party);
              dstField = new QueryField(fieldIdx, value);
              break;
          }
          case vaultdb::types::TypeId::ENCRYPTED_FLOAT64: {
              emp::Float *floatVal = srcValue->getEmpFloat64();
              double value = floatVal->reveal<double_t>(party);
              dstField = new QueryField(fieldIdx, value);
              break;
          }

          case vaultdb::types::TypeId::ENCRYPTED_VARCHAR: {
              const emp::Integer *empInteger = srcValue->getEmpInt();
              std::string strVal = empInteger->reveal<std::string>(party);
              dstField = new QueryField(fieldIdx, strVal);
              break;
          }
          default: // unsupported type or it is already encrypted
              throw;
      }

      return dstField;

}





