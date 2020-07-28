//
// Created by Jennie Rogers on 7/25/20.
//

#include "emp_manager.h"


QueryTable *EmpManager::secretShareTable(QueryTable *srcTable) {
    bool *localData = srcTable->serialize();
    size_t rowLength = srcTable->GetSchema()->size();
    size_t aliceSize = srcTable->GetNumTuples(); // in tuples
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


    QueryTable *dstTable = new QueryTable(aliceSize + bobSize, true);

    emp::Integer *res = new emp::Integer[aliceSize + bobSize];  // enough space for all inputs, one Integer per tuple
    emp::Bit *tmp = new emp::Bit[rowLength * (aliceSize + bobSize)]; //  bit representation of res


    // bool *readPos = localData;

    //int writePos = 0;
    int readTuple = aliceSize; // last tuple

    for (int i = 0; i < aliceSize; ++i) {
        --readTuple;
        for(int j = 0; j < rowLength; ++j) {
            encryptField(srcTable, dstTable, readTuple, i, j, (int) EmpParty::ALICE);
        }
    }

    for (int i = 0; i < bobSize; ++i) {
        for(int j = 0; j < rowLength; ++j) {
            encryptField(srcTable, dstTable, i, i + aliceSize, j, (int) EmpParty::BOB);
        }
    }


    // don't need batcher here because we do all of the heavy lifting locally
        // reverse Alice's order for correct Bitonic Merge ( Increasing -> Max -> Decreasing ) -- this may need refactoring
        /*for (int i = 0;  i < aliceSize; ++i) {
            --writeTuple;
            writePos = writeTuple * rowLength;

            for (int j = 0; j < rowLength; ++j) {
                // updating tmp pointer with alice's
                tmp[writeTuple*rowLength + j] = emp::Bit((party_ == EmpParty::ALICE) ? *readPos : 0, (int) EmpParty::ALICE);
                ++readPos;
                ++writePos;
            }
        }

        // Initialize Bob's Batcher
        emp::Batcher batcher;

        //emp::Float32
        // emp::Float *f;


        for (int i = 0; i < bobSize*rowLength; ++i)
            batcher.add<emp::Bit>((party_ == EmpParty::BOB) ? localData[i]:0);

        batcher.make_semi_honest((int) EmpParty::BOB); // Secret Share



        // add Bob's encrypted tuples
        for (int i = 0; i < bobSize*rowLength; ++i)
            tmp[i+aliceSize*rowLength] = batcher.next<emp::Bit>();

        // create full Integer representation with both Bob and Alice's information
        for(int i = 0; i < aliceSize + bobSize; ++i)
            res[i] = emp::Integer(rowLength, tmp+rowLength*i);


        // TODO: unwind this into a query table
        throw;
    */
    //}
}

    void EmpManager::encryptField(QueryTable *src, QueryTable *dst, int srcTupleIdx, int dstTupleIdx, int fieldIdx,
                                  int party) {

        const QueryField *srcField = src->GetTuple(srcTupleIdx)->GetField(fieldIdx);
        const types::Value srcValue = srcField->GetValue();
        types::TypeId type = srcValue.getType();
        size_t valSize = src->GetSchema()->GetField(fieldIdx)->size();

        QueryTuple *dstTuple = dst->GetTuple(dstTupleIdx);
        QueryField *dstField = nullptr; // initialized below

        switch (type) {
            case vaultdb::types::TypeId::BOOLEAN: {
                bool bit = srcValue.getBool();
                emp::Bit *eBit = new emp::Bit(bit, party);
                dstField = new QueryField(fieldIdx, eBit);
                break;
            }
            case vaultdb::types::TypeId::INTEGER32: {
                int32_t value = ((int) party_ == party) ? srcValue.getInt32() : 0;
                emp::Integer *intVal = new emp::Integer(value, party);
                dstField = new QueryField(fieldIdx, intVal, vaultdb::types::TypeId::INTEGER32);
                break;
            }
            case vaultdb::types::TypeId::NUMERIC:
            case vaultdb::types::TypeId::FLOAT32: {
                float value = ((int) party_ == party) ? srcValue.getFloat32() : 0;
                emp::Float32 *floatVal = new  emp::Float32(value, party);
                dstField = new QueryField(fieldIdx, floatVal);
                break;
            }
            case vaultdb::types::TypeId::FLOAT64: {
                double value =  ((int) party_ == party) ? srcValue.getFloat64() : 0;
                emp::Float *floatVal = new  emp::Float(24, 9, value, party);
                dstField = new QueryField(fieldIdx, floatVal);
                break;
            }
            case vaultdb::types::TypeId::INTEGER64: {
                int64_t value =  ((int) party_ == party) ? srcValue.getInt64(): 0;
                emp::Integer *intVal = new emp::Integer(value, party);
                dstField = new QueryField(fieldIdx, intVal, vaultdb::types::TypeId::INTEGER64);
                break;
            }

            case vaultdb::types::TypeId::VARCHAR: {
                std::string valueStr = ((int) party_ == party) ? srcValue.getVarchar() : nullptr;
                emp::Integer *strVal = new emp::Integer(valueStr.size() * 8, valueStr, party);
                dstField = new QueryField(fieldIdx, strVal, vaultdb::types::TypeId::VARCHAR);
                break;
            }
            default: // unsupported type or it is already encrypted
                throw;
        }

        dstTuple->PutField(fieldIdx, dstField);
    }





