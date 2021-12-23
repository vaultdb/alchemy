#include <pilot/src/common/shared_schema.h>
#include <emp-sh2pc/emp-sh2pc.h>
#include <data/data_provider.h>
#include <util/data_utilities.h>
#include <operators/sort.h>
#include "common/union_hybrid_data.h"
#include "enrich_htn_query.h"
#include <util/utilities.h>

using namespace  std;
using namespace vaultdb;
using namespace  emp;

#define TESTBED 0


auto start_time = emp::clock_start();
auto cumulative_runtime = emp::time_from(start_time);

void validateInputTable(const string & dbName, const string & sql, const SortDefinition  & expectedSortDefinition, const shared_ptr<PlainTable> & testTable)  {

    shared_ptr<PlainTable> expectedTable = DataUtilities::getQueryResults(dbName, sql, false);
    expectedTable->setSortOrder(expectedSortDefinition);

    // sort the inputs
    // ops deleted later using Operator framework
    Sort sort(testTable, expectedSortDefinition);
    shared_ptr<PlainTable> observedTable = sort.run();

    assert(*expectedTable ==  *observedTable);


}

string getRollupExpectedResultsSql(const string &groupByColName) {
    string expectedResultSql = "    WITH labeled as (\n"
                                    "        SELECT patid, zip_marker, CASE WHEN age_days <= 28*365 THEN 0\n"
                                    "                WHEN age_days > 28*365 AND age_days <= 39*365 THEN 1\n"
                                    "              WHEN age_days > 39*365  AND age_days <= 50*365 THEN 2\n"
                                    "              WHEN age_days > 50*365 AND age_days <= 61*365 THEN 3\n"
                                    "              WHEN age_days > 61*365 AND age_days <= 72*365 THEN 4\n"
                                    "                WHEN age_days > 72*365  AND age_days <= 83*365 THEN 5\n"
                                    "                ELSE 6 END age_strata,\n"
                                    "            sex, ethnicity, race, numerator, site_id, denom_excl \n"
                                    "        FROM patient\n"
                                    "        ORDER BY patid), \n"
                                    "  deduplicated AS ("
                                    "    SELECT p.patid, zip_marker, age_strata, sex, ethnicity, race, MAX(p.numerator) numerator, COUNT(*) cnt\n"
                                    "    FROM labeled p \n"
                                    "    GROUP BY p.patid, zip_marker, age_strata, sex, ethnicity, race \n"
                                    "    HAVING MAX(denom_excl) = 0 \n"
                                    "    ORDER BY p.patid, zip_marker, age_strata, sex, ethnicity, race ),  \n"
                                    "  data_cube AS (SELECT  zip_marker, age_strata, sex, ethnicity, race, SUM(numerator) numerator, COUNT(*) denominator, SUM((numerator = 1 AND cnt> 1)::INT) numerator_multisite, SUM(CASE WHEN cnt > 1 THEN 1 else 0 END)  denominator_multisite \n"
                                    "  FROM deduplicated \n"
                                    "  GROUP BY zip_marker, age_strata, sex, ethnicity, race \n"
                                    "  ORDER BY zip_marker, age_strata, sex, ethnicity, race ) \n";

    // JMR: not clear why we need to cast to BIGINT here, comes out as float by default despite inputs being BIGINT
    expectedResultSql += "SELECT " + groupByColName + ", SUM(numerator)::BIGINT numerator, SUM(denominator)::BIGINT denominator, SUM(numerator_multisite)::BIGINT numerator_multisite, SUM(denominator_multisite)::BIGINT denominator_multisite \n";
    expectedResultSql += " FROM data_cube \n"
                         " GROUP BY " + groupByColName + " \n"
                                                         " ORDER BY " + groupByColName;

    return expectedResultSql;

}


shared_ptr<SecureTable>
runRollup(int idx, string colName, int party, EnrichHtnQuery &enrich, const string &output_path) {


    shared_ptr<SecureTable> stratified = enrich.rollUpAggregate(idx);
   std::vector<int8_t> results = stratified->reveal(emp::XOR)->serialize();

    std::string suffix = (party == emp::ALICE) ? "alice" : "bob";
    std::string output_file = output_path + "/" + colName + "." + suffix;
    DataUtilities::writeFile(output_file, results);

    // validate it against the DB for testing
    if(TESTBED) {
        string unionedDbName = "enrich_htn_unioned";
        SortDefinition orderBy = DataUtilities::getDefaultSortDefinition(1);

        shared_ptr<PlainTable> revealed = stratified->reveal();
        revealed = DataUtilities::removeDummies(revealed);

        string query = getRollupExpectedResultsSql(colName);
        validateInputTable(unionedDbName, query, orderBy, revealed);

        // write it out
        string csv, schema;
        string out_path = Utilities::getCurrentWorkingDirectory() + "/pilot/secret_shares/validate";
        string out_file = out_path + "/" + colName + ".csv";
        Utilities::mkdir(out_path);
        shared_ptr<PlainTable> result = DataUtilities::getExpectedResults(unionedDbName, query, false, 1);

        std::stringstream schema_str;
        schema_str << *result->getSchema() << std::endl;
        csv = schema_str.str();

        for(size_t i = 0; i < result->getTupleCount(); ++i) {
                csv += (*result)[i].toString() + "\n";
        }
        DataUtilities::writeFile(out_file, csv);


    }
    

    return stratified;
}


int main(int argc, char **argv) {

    auto startTime = emp::clock_start();
    // local input file is an (unencrypted) csv of local site's data
    // secret share file is a binary, e.g., Chicago Alliance input
    if(argc < 6) {
        cout << "usage: ./run-data-partner <alice host> <port> <party> local_input_file secret_share_file" << endl;
    }


    string host(argv[1]);
    int port = atoi(argv[2]);
    int party = atoi(argv[3]);
    string localInputFile(argv[4]);
    string secretShareFile(argv[5]);

    string output_path = Utilities::getCurrentWorkingDirectory() + "/pilot/secret_shares/xor/";


    QuerySchema schema = SharedSchema::getInputSchema();
    NetIO *netio =  new emp::NetIO(party == ALICE ? nullptr : host.c_str(), port);
    setup_semi_honest(netio, party,  port);
    cout << "Finished netio setup" << endl;

    start_time = emp::clock_start(); // reset timer to account for async start of alice and bob
    startTime = start_time; // end-to-end one too
    // read inputs from two files, assemble with data of other host as one unioned secret shared table
    // expected order: alice, bob, chi
    shared_ptr<SecureTable> inputData = UnionHybridData::unionHybridData(schema, localInputFile, secretShareFile, netio, party);

    Utilities::checkMemoryUtilization("read input");
    cumulative_runtime = time_from(start_time);
    
      
    // validate it against the DB for testing
    if(TESTBED) {
        string unionedDbName = "enrich_htn_unioned";

        shared_ptr<PlainTable> revealed = inputData->reveal();
        string query = "SELECT * FROM patient ORDER BY patid, site_id";
        SortDefinition patientSortDef{ColumnSort(0, SortDirection::ASCENDING), ColumnSort (8, SortDirection::ASCENDING)};
        validateInputTable(unionedDbName, query, patientSortDef, revealed);


    }
    


	    cout << "***Read input on " << party << " in " <<    (cumulative_runtime + 0.0) *1e6*1e-9 << " ms." << endl;
	        start_time = emp::clock_start();
    cout << "********* start processing ***************" << endl;

    // create output dir:
    Utilities::mkdir(output_path);

    EnrichHtnQuery enrich(inputData);
    inputData.reset();

    cumulative_runtime += time_from(start_time);
    cout << "***Completed cube aggregation at " << time_from(start_time)*1e6*1e-9 << " ms, cumulative runtime=" << cumulative_runtime*1e6*1e-9 << " ms." << endl;
    Utilities::checkMemoryUtilization();


    start_time = emp::clock_start();
    shared_ptr<SecureTable> zipRollup = runRollup(0, "zip_marker", party, enrich, output_path);
    auto delta = time_from(start_time);
    cumulative_runtime += delta;
    
    cout << "***Done zip_marker rollup at " << delta*1e6*1e-9 << " ms, cumulative time: " << cumulative_runtime <<  endl;
    Utilities::checkMemoryUtilization();

    start_time = emp::clock_start();
    shared_ptr<SecureTable> ageRollup = runRollup(1, "age_strata", party, enrich, output_path);
    delta = time_from(start_time);
    cumulative_runtime += delta;

    Utilities::checkMemoryUtilization("rollup 2");
    cout << "***Done age rollup at " << delta*1e6*1e-9 << " ms, cumulative time: " << cumulative_runtime <<  endl;

    start_time = emp::clock_start();
    shared_ptr<SecureTable> genderRollup = runRollup(2, "sex", party, enrich, output_path);
    delta = time_from(start_time);
    cumulative_runtime += delta;
    
    Utilities::checkMemoryUtilization("rollup 3");
    cout << "***Done sex rollup at " << delta*1e6*1e-9 << " ms, cumulative time: " << cumulative_runtime <<  endl;

    start_time = emp::clock_start();
    shared_ptr<SecureTable> ethnicityRollup = runRollup(3, "ethnicity", party, enrich, output_path);
    delta = time_from(start_time);
    cumulative_runtime += delta;
    Utilities::checkMemoryUtilization("rollup 4");
    cout << "***Done ethnicity rollup at " << delta*1e6*1e-9 << " ms, cumulative time: " << cumulative_runtime <<  endl;

    start_time = emp::clock_start();
    shared_ptr<SecureTable> raceRollup = runRollup(4, "race", party, enrich, output_path);
    delta = time_from(start_time);
    cumulative_runtime += delta;
    Utilities::checkMemoryUtilization("rollup 5");

    cout << "***Done race rollup at " << delta*1e6*1e-9 << " ms, cumulative time: " << cumulative_runtime <<  endl;

     emp::finalize_semi_honest();

     delete netio;
    double runtime = time_from(startTime);
     cout << "Test completed on party " << party << " in " <<    (runtime+0.0)*1e6*1e-9 << " ms." << endl;
}
