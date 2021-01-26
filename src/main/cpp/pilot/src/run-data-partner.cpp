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

    // sort the inputs
    CommonTableExpression *unionedData = new CommonTableExpression(testTable);

    std::shared_ptr<Operator> sortOp( new Sort(expectedSortDefinition, unionedData->getPtr()));
    std::shared_ptr<QueryTable> observedTable = sortOp->run();


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
        string unionedDb = "enrich_htn_unioned";
        string query = "SELECT * FROM patient WHERE site_id=3 ORDER BY patid, site_id";
        SortDefinition patientSortDef{ColumnSort(0, SortDirection::ASCENDING), ColumnSort (8, SortDirection::ASCENDING)};

        validateTable(unionedDb, query, patientSortDef, revealed);

        std::cout << "Input passed test!" << std::endl;
    }

    // TODO: crib test code from enrich_test.cpp
     emp::finalize_semi_honest();
}
