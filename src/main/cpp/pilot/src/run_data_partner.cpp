#include <pilot/src/common/shared_schema.h>
#include <emp-sh2pc/emp-sh2pc.h>
#include <data/data_provider.h>
#include <util/data_utilities.h>
#include <operators/sort.h>
#include "common/union_hybrid_data.h"
#include "enrich_htn_query.h"
#include <util/utilities.h>
#include <util/logger.h>


using namespace std;
using namespace vaultdb;
using namespace emp;

#define TESTBED 1


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
    string expectedResultSql = "   WITH labeled as (\n"
                               "        SELECT pat_id, age_strata, sex, ethnicity, race, numerator, denom_excl\n"
                               "        FROM patient\n"
                               "        ORDER BY pat_id),\n"
                               "  deduplicated AS (    SELECT p.pat_id,  age_strata, sex, ethnicity, race, MAX(p.numerator::INT) numerator, COUNT(*) cnt\n"
                               "    FROM labeled p\n"
                               "    GROUP BY p.pat_id, age_strata, sex, ethnicity, race\n"
                               "    HAVING MAX(denom_excl::INT) = 0\n"
                               "    ORDER BY p.pat_id, age_strata, sex, ethnicity, race ),\n"
                               "  data_cube AS (SELECT  age_strata, sex, ethnicity, race, SUM(numerator::INT) numerator, COUNT(*) denominator,\n"
                               "                       SUM(CASE WHEN (numerator > 0 AND cnt> 1) THEN 1 ELSE 0 END) numerator_multisite, SUM(CASE WHEN (cnt > 1) THEN 1 else 0 END)  denominator_multisite\n"
                               "  FROM deduplicated\n"
                               "  GROUP BY  age_strata, sex, ethnicity, race\n"
                               "  ORDER BY  age_strata, sex, ethnicity, race )\n";
    // JMR: not clear why we need to cast to BIGINT here, comes out as float by default despite inputs being BIGINT
    expectedResultSql += "SELECT " + groupByColName + ", SUM(numerator)::BIGINT numerator, SUM(denominator)::BIGINT denominator, SUM(numerator_multisite)::BIGINT numerator_multisite, SUM(denominator_multisite)::BIGINT denominator_multisite \n";
    expectedResultSql += " FROM data_cube \n"
                         " GROUP BY " + groupByColName + " \n"
                                                         " ORDER BY " + groupByColName;

    return expectedResultSql;

}


shared_ptr<SecureTable>
runRollup(int idx, string colName, int party, EnrichHtnQuery &enrich, const string &output_path) {
    auto start_time = emp::clock_start();
    auto logger = vaultdb_logger::get();


    shared_ptr<SecureTable> stratified = enrich.rollUpAggregate(idx);
   std::vector<int8_t> results = stratified->reveal(emp::XOR)->serialize();

    std::string suffix = (party == emp::ALICE) ? "alice" : "bob";
    std::string output_file = output_path + "/" + colName + "." + suffix;
    DataUtilities::writeFile(output_file, results);

    // validate it against the DB for testing
    if(TESTBED) {
        string unionedDbName = "enrich_htn_unioned_3pc";
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


    auto delta = time_from(start_time);
    cumulative_runtime += delta;

    Utilities::checkMemoryUtilization(colName);
    BOOST_LOG(logger) <<  "***Done " << colName << " rollup at " << delta*1e6*1e-9 << " ms, cumulative time: " << cumulative_runtime <<  endl;


    return stratified;
}



int main(int argc, char **argv) {

    auto startTime = emp::clock_start();
    // local input file is an (unencrypted) csv of local site's data
    // secret share file is a binary, e.g., Chicago Alliance input
    if(argc < 5) {
      cout << "usage: ./run-data-partner <alice host> <port> <party> local_input_file < optional secret_share_file>" << endl;
    }


    string host(argv[1]);
    int port = atoi(argv[2]);
    int party = atoi(argv[3]);
    string localInputFile(argv[4]);
    string secretShareFile = "";
    if(argc == 6)
      secretShareFile = argv[5];

    size_t cardinality_bound = 441; // 7 * 3 * 3 * 7

    string output_path = Utilities::getCurrentWorkingDirectory() + "/pilot/secret_shares/xor/";
    string party_name = (party == 1) ? "alice"  : "bob";
    // TODO: paramaterize the default logging level
    Logger::setup();

    //Logger::setup("pilot-" + party_name);
    auto logger = vaultdb_logger::get();

    QuerySchema schema = SharedSchema::getInputSchema();
    NetIO *netio =  new emp::NetIO(party == ALICE ? nullptr : host.c_str(), port);
    setup_semi_honest(netio, party,  port);
    BOOST_LOG(logger) << "Finished netio setup" << endl;

    start_time = emp::clock_start(); // reset timer to account for async start of alice and bob
    startTime = start_time; // end-to-end one too
    // read inputs from two files, assemble with data of other host as one unioned secret shared table
    // expected order: alice, bob, chi
    shared_ptr<SecureTable> inputData = UnionHybridData::unionHybridData(schema, localInputFile, secretShareFile, netio, party);

    Utilities::checkMemoryUtilization("read input");
    cumulative_runtime = time_from(start_time);
    
      
    // validate it against the DB for testing
    if(TESTBED) {
        std::shared_ptr<PlainTable> revealed = inputData->reveal();
        string unionedDbName = "enrich_htn_unioned_3pc";  // enrich_htn_prod for in-the-field runs
        string query = "SELECT pat_id, age_strata, sex,ethnicity, race, numerator, denom_excl  FROM patient ORDER BY pat_id, age_strata, sex, ethnicity, race, numerator, denom_excl";
        SortDefinition patientSortDef = DataUtilities::getDefaultSortDefinition(7);


        validateInputTable(unionedDbName, query, patientSortDef, revealed);

        Logger::write("Input passed test!");


    }



    BOOST_LOG(logger) << "***Read input on " << party << " in " <<    (cumulative_runtime + 0.0) *1e6*1e-9 << " ms." << endl;
	        start_time = emp::clock_start();
    BOOST_LOG(logger)  << "********* start processing ***************" << endl;

    // create output dir:
    Utilities::mkdir(output_path);

    EnrichHtnQuery enrich(inputData, cardinality_bound);
    inputData.reset();

    assert(enrich.dataCube->getTupleCount() == cardinality_bound);

    cumulative_runtime += time_from(start_time);
    BOOST_LOG(logger) << "***Completed cube aggregation at " << time_from(start_time)*1e6*1e-9 << " ms, cumulative runtime=" << cumulative_runtime*1e6*1e-9 << " ms." << endl;
    Utilities::checkMemoryUtilization();


    start_time = emp::clock_start();
    shared_ptr<SecureTable> ageRollup = runRollup(0, "age_strata", party, enrich, output_path);
    auto delta = time_from(start_time);
    cumulative_runtime += delta;

    Utilities::checkMemoryUtilization("age_strata");
    BOOST_LOG(logger) <<  "***Done age rollup at " << delta*1e6*1e-9 << " ms, cumulative time: " << cumulative_runtime <<  endl;

    start_time = emp::clock_start();
    shared_ptr<SecureTable> genderRollup = runRollup(1, "sex", party, enrich, output_path);
    delta = time_from(start_time);
    cumulative_runtime += delta;
    
    Utilities::checkMemoryUtilization("gender");
    BOOST_LOG(logger) <<  "***Done sex rollup at " << delta*1e6*1e-9 << " ms, cumulative time: " << cumulative_runtime <<  endl;

    start_time = emp::clock_start();
    shared_ptr<SecureTable> ethnicityRollup = runRollup(2, "ethnicity", party, enrich, output_path);
    delta = time_from(start_time);
    cumulative_runtime += delta;
    Utilities::checkMemoryUtilization("ethnicity");
    BOOST_LOG(logger) <<  "***Done ethnicity rollup at " << delta*1e6*1e-9 << " ms, cumulative time: " << cumulative_runtime <<  endl;

    start_time = emp::clock_start();
    shared_ptr<SecureTable> raceRollup = runRollup(3, "race", party, enrich, output_path);
    delta = time_from(start_time);
    cumulative_runtime += delta;
    Utilities::checkMemoryUtilization("race");

    BOOST_LOG(logger) << "***Done race rollup at " << delta*1e6*1e-9 << " ms, cumulative time: " << cumulative_runtime <<  endl;

     emp::finalize_semi_honest();

     delete netio;
    double runtime = time_from(startTime);
    BOOST_LOG(logger) <<  "Test completed on party " << party << " in " <<    (runtime+0.0)*1e6*1e-9 << " ms." << endl;
}
