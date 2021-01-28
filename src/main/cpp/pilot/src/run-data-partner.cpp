#include <pilot/src/common/shared_schema.h>
#include <emp-sh2pc/emp-sh2pc.h>
#include <data/DataProvider.h>
#include <util/data_utilities.h>
#include <operators/common_table_expression.h>
#include <operators/sort.h>
#include "union_hybrid_data.h"

using namespace  std;
using namespace vaultdb;
using namespace  emp;

#define TESTBED 1


void validateTable(const std::string & dbName, const std::string & sql, const SortDefinition  & expectedSortDefinition, const std::shared_ptr<QueryTable> & testTable)  {

    std::shared_ptr<QueryTable> expectedTable = DataUtilities::getQueryResults(dbName, sql, false);
    expectedTable->setSortOrder(expectedSortDefinition);

    std::cout << "analyzing " << expectedTable->getTupleCount() << " tuples." << std::endl;
    std::cout << "First tuple, test table: " << testTable->getTuple(0) << std::endl;
    // sort the inputs
    // ops deleted later using Operator framework
    CommonTableExpression *unionedData = new CommonTableExpression(testTable);
    Sort *sortOp = new Sort(expectedSortDefinition, unionedData->getPtr());
    std::shared_ptr<QueryTable> observedTable = sortOp->run();


  //  std::cout << "Expected table: \n" << *expectedTable << std::endl;
    /*
     * (5, '011', 15589, 'F', true, 4, 0, 1, 1)
       (5, '011', 15589, 'F', true, 4, 0, 0, 2)
       (5, '011', 15589, 'F', true, 4, 0, 1, 3)
     */

  //  std::cout << "Observed table:\n" << *observedTable << std::endl;
    /*
     * (5, '011', 15589, 'F', true, 4, 0, 1, 1)
       (5, '011', 15589, 'F', true, 4, 0, 1, 3)
       (5, '011', 15589, 'F', true, 4, 0, 0, 2)
       -- swapping on the last two.
       -- how is sort determining the size of the array?
     */
    assert(*expectedTable ==  *observedTable);

}

int main(int argc, char **argv) {

    // local input file is an (unencrypted) csv of local site's data
    // secret share file is a binary, e.g., Chicago Alliance input
    if(argc < 6) {
        std::cout << "usage: ./run-data-partner <alice host> <port> <party> local_input_file secret_share_file" << std::endl;
    }


    string host(argv[1]);
    int port = atoi(argv[2]);
    int party = atoi(argv[3]);
    string localInputFile(argv[4]);
    string secretShareFile(argv[5]);

    QuerySchema schema = SharedSchema::getInputSchema();
    NetIO *netio =  new emp::NetIO(party == ALICE ? nullptr : host.c_str(), port);
    setup_semi_honest(netio, party,  port);


    // read inputs from two files, assemble with data of other host as one unioned secret shared table
    // expected order: alice, bob, chi
    std::shared_ptr<QueryTable> inputData = UnionHybridData::unionHybridData(schema, localInputFile, secretShareFile, netio, party);

    // validate it against the DB for testing
    if(TESTBED) {
        std::shared_ptr<QueryTable> revealed = inputData->reveal();
        string unionedDbName = "enrich_htn_unioned";
        string query = "SELECT * FROM patient ORDER BY patid, site_id";
        SortDefinition patientSortDef{ColumnSort(0, SortDirection::ASCENDING), ColumnSort (8, SortDirection::ASCENDING)};


        CommonTableExpression *unionedData = new CommonTableExpression(inputData);
        Sort *sortOp = new Sort(patientSortDef, unionedData->getPtr());
        std::shared_ptr<QueryTable> observedTable = sortOp->run()->reveal();

        std::shared_ptr<QueryTable> expectedTable = DataUtilities::getQueryResults(unionedDbName, query, false);
        expectedTable->setSortOrder(patientSortDef);


        assert(*observedTable == *expectedTable);
        //validateTable(unionedDbName, query, patientSortDef, revealed);

        std::cout << "Input passed test!" << std::endl;
    }

    // TODO: crib test query code from enrich_test.cpp
     emp::finalize_semi_honest();
}
