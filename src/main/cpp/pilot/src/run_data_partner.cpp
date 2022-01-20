#include <pilot/src/common/shared_schema.h>
#include <pilot/src/common/pilot_utilities.h>
#include <emp-sh2pc/emp-sh2pc.h>
#include <util/data_utilities.h>
#include "common/union_hybrid_data.h"
#include "enrich_htn_query.h"
#include <util/utilities.h>
#include <util/logger.h>
#include <data/csv_reader.h>
#include <boost/program_options.hpp>
#include <operators/sort.h>



using namespace std;
using namespace vaultdb;
using namespace emp;
namespace po = boost::program_options;

#define TESTBED 0


auto start_time = emp::clock_start();
auto cumulative_runtime = emp::time_from(start_time);

// roll up one group-by col at a time
// input schema:
// age_strata (0), sex (1), ethnicity (2) , race (3), numerator_cnt (4), denominator_cnt (5)
shared_ptr<SecureTable>
runRollup(int idx, string colName, int party, shared_ptr<SecureTable> &data_cube, const string &output_path) {
    auto start_time = emp::clock_start();
    auto logger = vaultdb_logger::get();
    shared_ptr<SecureTable> stratified = PilotUtilities::rollUpAggregate(data_cube, idx);

   std::vector<int8_t> results = stratified->reveal(emp::XOR)->serialize();

    std::string suffix = (party == emp::ALICE) ? "alice" : "bob";
    std::string output_file = output_path + "/" + colName + "." + suffix;
    DataUtilities::writeFile(output_file, results);

    // validate it against the DB for testing
    if(TESTBED) {
        SortDefinition orderBy = DataUtilities::getDefaultSortDefinition(1);

        shared_ptr<PlainTable> revealed = stratified->reveal();
        revealed = DataUtilities::removeDummies(revealed);

        string query = PilotUtilities::getRollupExpectedResultsSql(colName);
        PilotUtilities::validateInputTable(PilotUtilities::unioned_db_name_, query, orderBy, revealed);

        // write it out
        string csv, schema;
        string out_path = Utilities::getCurrentWorkingDirectory() + "/pilot/secret_shares/validate";
        string out_file = out_path + "/" + colName + ".csv";
        Utilities::mkdir(out_path);
        shared_ptr<PlainTable> result = DataUtilities::getExpectedResults(PilotUtilities::unioned_db_name_, query, false, 1);

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

    // local input file is an (unencrypted) csv of local site's data
    // secret share file is a binary, e.g., Chicago Alliance input
    // parse args
    string host, localInputTuples, localPartialCountFile, secretShareTuples, secretSharePartialCounts, dbName;
    int port=0, party=0;
    bool semijoinOptimization = false;
    size_t cardinality_bound = 441; // 7 * 3 * 3 * 7
    bool dbInput = false; // goes to csv
    string study_year, party_name;
    string logfile_prefix = "";
    string patient_input_query;
    string partial_counts_query;



    try {
        // example invocations:
        // 1 pilot/test/input/alice-patient.csv  pilot/test/output/chi-patient.alice
        // ./run_data_partner -h 127.0.0.1 -P 54321 --party=1 -c  pilot/test/input/alice-patient.csv -rc pilot/test/output/chi-patient.alice  -rp
        po::options_description desc("Options");
       desc.add_options()
                ("help", "print help message")
                ("host,h", po::value<string>(), "alice hostname")
                ("port,P", po::value<int>(), "connection port")
                ("party", po::value<int>(), "computing party for mpc (1 = alice, 2 = bob)")
                ("c", po::value<string>(), "local, plaintext csv input file for mpc tuples")
                ("database,d", po::value<string>(), "local database name")
                ("p", po::value<string>(), "local csv file for partial counts")
                ("rc", po::value<string>(), "secret sh6are file of mpc tuples (remote version of -c)")
                ("rp", po::value<string>(), "secret share file of partial counts(remote version of -p)")
               ("log-prefix,l", po::value<string>(), "prefix of filename for log")
               ("year,y", po::value<string>(), "study year of experiment, in 2018, 2019, 2020, or all")
               ("cardinality-bound,b", po::value<size_t>()->default_value(441), "cardinality bound for output of aggregation.  Equal to the cross-product of all group-bys (e.g., age/sex/ethnicity/race)");



        po::variables_map vm;
        po::store(po::parse_command_line(argc, argv, desc), vm);
        po::notify(vm);

        if (vm.count("help")) {
            cout << desc << "\n";
            return 0;
        }

        if(vm.count("host")) {
            host =  vm["host"].as<string>();
        }
        else {
            throw std::invalid_argument("Missing hostname for mpc.");
        }


        if (vm.count("port")) {
            port = vm["port"].as<int>();
        } else {
            throw std::invalid_argument("Need port for mpc");
        }

        if (vm.count("party")) {
            party = vm["party"].as<int>();
            if(party != 1 && party != 2) {
                throw std::invalid_argument("Party of " + std::to_string(party) + "incorrect.  Must be 1 or 2");
            }
            party_name = (party == 1) ? "alice"  : "bob";

        } else {
            throw std::invalid_argument("Need party id for mpc");
        }


        if (vm.count("c")) {
            localInputTuples = vm["c"].as<string>();
        }

        if (vm.count("database")) {
            dbName = vm["database"].as<string>();
            dbInput = true;
        }

        if(vm.count("p")) {
            localPartialCountFile = vm["p"].as<string>();
            if(vm.count("database")) {
                throw std::invalid_argument(
                        "Error: two sources of input for partial count (--d and --p), please select one.");
            }
        }

        if(vm.count("rc")) {
            secretShareTuples = vm["rc"].as<string>();
        }

        if (vm.count("year")) {
            study_year = vm["year"].as<string>();
            string input_query_file = "pilot/queries/patient-multisite-" + study_year + ".sql";
            patient_input_query = DataUtilities::readTextFileToString(input_query_file);

        } else {
            throw std::invalid_argument("Need study year to know the data to input");
        }

        if(vm.count("rp")) {
            secretSharePartialCounts = vm["rp"].as<string>();
            semijoinOptimization = true;
            partial_counts_query = DataUtilities::readTextFileToString("pilot/queries/partial-count-" + study_year + ".sql");
        }

        if(vm.count("cardinality-bound")) {
            cardinality_bound = vm["cardinality-bound"].as<size_t>();
        }


        // sanity check
        if(!vm.count("c") and !vm.count("database")) {
            throw std::invalid_argument("Need input source for local data.  Use either -c or -d");
        }

        if (vm.count("log-prefix")) {
            logfile_prefix = vm["log-prefix"].as<string>();
        }

    }
    catch(exception& e) {
        cerr << "error: " << e.what() << "\n";
        return 1;
    }
    catch(...) {
        cerr << "Exception of unknown type!\n";
    }



    auto startTime = emp::clock_start();


    string output_path = Utilities::getCurrentWorkingDirectory() + "/pilot/secret_shares/xor/";
    // TODO: parameterize the default logging level
    Logger::setup(logfile_prefix);
    auto logger = vaultdb_logger::get();
    uint64_t epoch = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    BOOST_LOG(logger) << "Starting epoch " << epoch << endl;

    QuerySchema schema = SharedSchema::getInputSchema();
    NetIO *netio =  new emp::NetIO(party == ALICE ? nullptr : host.c_str(), port);
    setup_semi_honest(netio, party,  port);

    start_time = emp::clock_start(); // reset timer to account for async start of alice and bob
    startTime = start_time; // end-to-end one too
    // read inputs from two files, assemble with data of other host as one unioned secret shared table
    // expected order: alice, bob, chi

    shared_ptr<SecureTable> inputData = (dbInput) ? UnionHybridData::unionHybridData(dbName, patient_input_query,
                                                                                     secretShareTuples, netio, party)
           :  UnionHybridData::unionHybridData(schema, localInputTuples, secretShareTuples, netio, party);

    cumulative_runtime = time_from(start_time);
    
      
    // validate it against the DB for testing
    if(TESTBED) {
        assert(study_year == "all"); // only coded for no year selection
        std::shared_ptr<PlainTable> revealed = inputData->reveal();
        string query = (semijoinOptimization) ? "SELECT pat_id, age_strata, sex,ethnicity, race, numerator, denom_excl  FROM patient WHERE multisite ORDER BY pat_id, age_strata, sex, ethnicity, race, numerator, denom_excl"
                 : "SELECT pat_id, age_strata, sex,ethnicity, race, numerator, denom_excl  FROM patient ORDER BY pat_id, age_strata, sex, ethnicity, race, numerator, denom_excl";
        SortDefinition patientSortDef = DataUtilities::getDefaultSortDefinition(7);
        PilotUtilities::validateInputTable(PilotUtilities::unioned_db_name_, query, patientSortDef, revealed);


    }



    BOOST_LOG(logger) << "***Read input on " << party << " in " <<    (cumulative_runtime + 0.0) *1e6*1e-9 << " ms." << endl;
    start_time = emp::clock_start();

    // create output dir:
    Utilities::mkdir(output_path);

    EnrichHtnQuery enrich(inputData, cardinality_bound);
    inputData.reset();

    assert(enrich.dataCube->getTupleCount() == cardinality_bound);

    cumulative_runtime += time_from(start_time);
    BOOST_LOG(logger) << "***Completed cube aggregation at " << time_from(start_time)*1e6*1e-9 << " ms, cumulative runtime=" << cumulative_runtime*1e6*1e-9 << " ms." << endl;
    Utilities::checkMemoryUtilization();

    if(semijoinOptimization) {
        // add in the 1-site PIDs
        shared_ptr<SecureTable> alice, bob, chi;

        shared_ptr<PlainTable> local_partial_counts = (dbInput) ?  DataUtilities::getQueryResults(dbName, partial_counts_query, false)
                    : CsvReader::readCsv(localPartialCountFile, SharedSchema::getPartialCountSchema());

        assert(local_partial_counts->getTupleCount() == cardinality_bound);
        // ship local, partial counts - alice, then bob
            if (party == 1) { // alice
                alice = SecureTable::secret_share_send_table(local_partial_counts, netio, 1);
                bob = SecureTable::secret_share_recv_table(*local_partial_counts->getSchema(), SortDefinition(), netio,
                                                           2);
            } else { // bob
                alice = SecureTable::secret_share_recv_table(*local_partial_counts->getSchema(), SortDefinition(),
                                                             netio, 1);
                bob = SecureTable::secret_share_send_table(local_partial_counts, netio, 2);
            }


            chi = UnionHybridData::readSecretSharedInput(secretSharePartialCounts, QuerySchema::toPlain(*(alice->getSchema())), party);

            std::vector<shared_ptr<SecureTable>> partial_aggs { alice, bob, chi};
            enrich.addPartialAggregates(partial_aggs);

        if(TESTBED) {
            assert(study_year == "all"); // only coded for no year selection, repeat
            std::shared_ptr<PlainTable> revealed = enrich.dataCube->reveal();
            SortDefinition cube_sort_def = DataUtilities::getDefaultSortDefinition(4);
            PilotUtilities::validateInputTable(PilotUtilities::unioned_db_name_, PilotUtilities::data_cube_sql_, cube_sort_def, revealed);
        }


    }

    shared_ptr<SecureTable> ageRollup = runRollup(0, "age_strata", party, enrich.dataCube, output_path);
    shared_ptr<SecureTable> genderRollup = runRollup(1, "sex", party, enrich.dataCube, output_path);
    shared_ptr<SecureTable> ethnicityRollup = runRollup(2, "ethnicity", party, enrich.dataCube, output_path);
    shared_ptr<SecureTable> raceRollup = runRollup(3, "race", party, enrich.dataCube, output_path);

     emp::finalize_semi_honest();

     delete netio;
    double runtime = time_from(startTime);
    epoch = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    BOOST_LOG(logger) << "Ending epoch " << epoch << endl;
    BOOST_LOG(logger) <<  "Test completed on " << party_name << " in " <<    (runtime+0.0)*1e6*1e-9 << " secs." <<  endl;
}
