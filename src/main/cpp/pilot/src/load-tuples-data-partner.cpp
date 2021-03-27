#include <pilot/src/common/shared_schema.h>
#include <emp-sh2pc/emp-sh2pc.h>
#include <data/DataProvider.h>
#include <util/data_utilities.h>
#include <operators/sort.h>
#include "common/union_hybrid_data.h"

using namespace  std;
using namespace vaultdb;
using namespace  emp;

#define TESTBED 1


void validateInputTable(const std::string & dbName, const std::string & sql, const SortDefinition  & expectedSortDefinition, const std::shared_ptr<PlainTable> & testTable)  {

    std::shared_ptr<PlainTable> expectedTable = DataUtilities::getQueryResults(dbName, sql, false);
    expectedTable->setSortOrder(expectedSortDefinition);


    // sort the inputs
    Sort sortOp(testTable, expectedSortDefinition);
    std::shared_ptr<PlainTable> observedTable = sortOp.run();

    assert(*expectedTable ==  *observedTable);

}

// example run:
// Alice:
// ./bin/load-tuples-data-partner 127.0.0.1 54321 1 pilot/test/input/alice-patient.csv  pilot/test/output/chi-patient.alice
// Bob:
//  ./bin/load-tuples-data-partner 127.0.0.1 54321 2 pilot/test/input/bob-patient.csv  pilot/test/output/chi-patient.bob


int main(int argc, char **argv) {

    // local input file is an (unencrypted) csv of local site's data
    // secret share file is a binary, e.g., Chicago Alliance input
    if(argc < 6) {
        std::cout << "usage: ./run-data-partner <alice host> <port> <party> local_input_file secret_share_file" << std::endl;
    }

    auto startTime = emp::clock_start();


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
    std::shared_ptr<SecureTable> inputData = UnionHybridData::unionHybridData(schema, localInputFile, secretShareFile, netio, party);

    // validate it against the DB for testing
    if(TESTBED) {
        std::shared_ptr<PlainTable> revealed = inputData->reveal();
        string unionedDbName = "enrich_htn_unioned";
        string query = "SELECT * FROM patient ORDER BY patid, site_id";
        SortDefinition patientSortDef{ColumnSort(0, SortDirection::ASCENDING), ColumnSort (8, SortDirection::ASCENDING)};


        validateInputTable(unionedDbName, query, patientSortDef, revealed);

        std::cout << "Input passed test!" << std::endl;
    }


    inputData.reset(); // frees inputData

     emp::finalize_semi_honest();
    double runtime = time_from(startTime);
    cout << "Read and validated input on " << party << " in " <<    (runtime+0.0)*1e6*1e-9 << " ms." << endl;

     std::cout << "Runtime: " <<    (runtime+0.0)*1e6*1e-9 << " ms." << endl;
}
