#include <pilot/src/common/shared_schema.h>
#include <emp-sh2pc/emp-sh2pc.h>
#include <util/data_utilities.h>
#include <operators/sort.h>
#include "common/union_hybrid_data.h"
#include "common/pilot_utilities.h"
using namespace  std;
using namespace vaultdb;
using namespace  emp;

#define TESTBED 1


void validateInputTable(const std::string & dbName, const std::string & sql, const SortDefinition  & expectedSortDefinition, PlainTable *testTable)  {



      PlainTable *expectedTable = DataUtilities::getQueryResults(dbName, sql, false);
      expectedTable->order_by_ = expectedSortDefinition;


    // sort the inputs
    Sort sortOp(testTable, expectedSortDefinition);
    PlainTable *observedTable = sortOp.run();

    assert(*expectedTable ==  *observedTable);
    delete expectedTable;

}

// example run:
// Alice:
// ./bin/load-tuples-data-partner 127.0.0.1 54321 1 pilot/test/input/alice-patient.csv  pilot/test/output/chi-patient.alice
// Bob:
//  ./bin/load-tuples-data-partner 127.0.0.1 54321 2 pilot/test/input/bob-patient.csv  pilot/test/output/chi-patient.bob


int main(int argc, char **argv) {

    // local input file is an (unencrypted) csv of local site's data
    // secret share file is a binary, e.g., Chicago Alliance input
    if(argc < 5) {
        std::cout << "usage: ./load-tuples-data-partner <alice host> <port> <party> local_input_file <optional secret_share_file>" << std::endl;
    }

    auto startTime = emp::clock_start();


    string host(argv[1]);
    int port = atoi(argv[2]);
    int party = atoi(argv[3]);
    string localInputFile(argv[4]);
    string secretShareFile = "";
    if(argc == 6)
      secretShareFile = argv[5];

    QuerySchema schema = SharedSchema::getInputSchema();
    PilotUtilities::setupSystemConfiguration(party, host, port);


    // read inputs from two files, assemble with data of other host as one unioned secret shared table
    // expected order: alice, bob, chi
    SecureTable *input_data = UnionHybridData::unionHybridData(schema, localInputFile, secretShareFile);

    //Logger::write("Done unioning!");
    // validate it against the DB for testing
    if(TESTBED) {
        PlainTable *revealed = input_data->reveal();
        string unioned_db_name = "enrich_htn_unioned_3pc";  // enrich_htn_prod for in-the-field runs
        string query = "SELECT pat_id, age_strata, sex,ethnicity, race, numerator, denom_excl  FROM patient ORDER BY pat_id, age_strata, sex, ethnicity, race, numerator, denom_excl";
        SortDefinition patientSortDef = DataUtilities::getDefaultSortDefinition(7);

	
        validateInputTable(unioned_db_name, query, patientSortDef, revealed);

        //Logger::write("Input passed test!");
        delete revealed;
    }


    delete input_data; // frees inputData

    double runtime = time_from(startTime);
    //Logger::write("Read and validated input on " + std::to_string(party)  + " in "  +    std::to_string((runtime+0.0)*1e6*1e-9) + " ms.");

    //Logger::write("Runtime: " +    std::to_string((runtime+0.0)*1e6*1e-9) + " ms.");
}
