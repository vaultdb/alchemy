//
// Created by Jennie Rogers on 10/24/20.
//

#include <util/type_utilities.h>
#include "EmpBaseTest.h"

DEFINE_int32(party, 1, "party for EMP execution");
DEFINE_int32(port, 54323, "port for EMP execution");
DEFINE_string(alice_host, "127.0.0.1", "alice hostname for execution");


class EmpTableTest : public EmpBaseTest {};


// test encrypting a query table with a single int in EMP
// Testing absent psql dependency
TEST_F(EmpTableTest, encrypt_table_one_column) {
    const uint32_t tupleCount = 10;
    int32_t aliceInputData[10] = {1, 1, 1, 1, 1, 1, 2, 3, 3, 3};
    int32_t bobInputData[10] = {4, 33, 33, 33, 33, 35, 35, 35, 35, 35};
    int32_t *inputData = (FLAGS_party == emp::ALICE) ?  aliceInputData : bobInputData;

    QuerySchema schema(1);
    schema.putField(QueryFieldDesc(0, false, "test", "test_table", types::TypeId::INTEGER32));

    std::unique_ptr<QueryTable> inputTable(new QueryTable(tupleCount, 1, false));
    inputTable->setSchema(schema);

    for(uint32_t i = 0; i < tupleCount; ++i) {
        types::Value val(inputData[i]);
        inputTable->getTuplePtr(i)->setDummyTag(false);
        QueryField *fieldPtr = inputTable->getTuplePtr(i)->getFieldPtr(0);
        fieldPtr->setValue(val);
        fieldPtr->setOrdinal(0);
    }


    std::cout << "Initial table: " << *inputTable << std::endl;
    std::shared_ptr<QueryTable> encryptedTable = inputTable->secretShare(netio, FLAGS_party);

    netio->flush();

    std::unique_ptr<QueryTable> decryptedTable = encryptedTable->reveal(emp::PUBLIC);

    // set up expected result
    std::unique_ptr<QueryTable> expectedTable(new QueryTable(2*tupleCount, 1, false));
    expectedTable->setSchema(schema);
    // insert alice data first to last
    for(uint32_t i = 0; i < tupleCount; ++i) {
        types::Value val(aliceInputData[i]);
        expectedTable->getTuplePtr(i)->initDummy();
        QueryField *fieldPtr = expectedTable->getTuplePtr(i)->getFieldPtr(0);
        fieldPtr->setValue(val);
        fieldPtr->setOrdinal(0);
    }

    int offset = tupleCount;

    // add bob's tuples from last to first
    int readIdx = tupleCount;
    for(uint32_t i = 0; i < tupleCount; ++i) {
        --readIdx;
        types::Value val(bobInputData[readIdx]);
        expectedTable->getTuplePtr(i+offset)->initDummy();
        QueryField *fieldPtr = expectedTable->getTuplePtr(i + offset)->getFieldPtr(0);
        fieldPtr->setValue(val);
        fieldPtr->setOrdinal(0);
    }




    //verify output
    ASSERT_EQ(*expectedTable, *decryptedTable) << "Query table was not processed correctly.";




}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    gflags::ParseCommandLineFlags(&argc, &argv, false);
    return RUN_ALL_TESTS();
}