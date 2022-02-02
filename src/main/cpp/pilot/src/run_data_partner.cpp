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
#include <unordered_map>


using namespace std;
using namespace vaultdb;
using namespace emp;
namespace po = boost::program_options;

#define TESTBED 0


auto start_time = emp::clock_start();
auto cumulative_runtime = emp::time_from(start_time);
//unordered_map<string, long> runtime_per_step;

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

    BOOST_LOG(logger) <<  "***Done " << colName << " rollup at " << delta*1e6*1e-9 << " ms, cumulative time: " << cumulative_runtime << " epoch " << Utilities::getEpoch() <<  endl;


    return stratified;
}



int main(int argc, char **argv) {

    // local input file is an (unencrypted) csv of local site's data
    // secret share file is a binary, e.g., Chicago Alliance input
    // parse args
    string host, remote_patient_file, remote_patient_partial_count_file, db_name;
    int port=0, party=0;
    bool semijoin_optimization = false;
    size_t cardinality_bound = 441; // 7 * 3 * 3 * 7
    string study_year, party_name;
    string logfile_prefix = "";
    string patient_input_query = DataUtilities::readTextFileToString("pilot/queries/patient.sql");
    string partial_count_query = DataUtilities::readTextFileToString("pilot/queries/partial-count.sql");
    string selection_clause;
    string partial_count_selection_clause;


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
                ("database,d", po::value<string>(), "local database name")
                ("semijoin-optimization,s", "enables semijoin optimization, only evaluates multisite patients in MPC") // boolean flag
                ("remote-tuples-file,r", po::value<string>(), "secret share file root of mpc tuples") // will have $FILE_ROOT.1.alice, $FILE_ROOT.2.alice (or bob) and so on.
                ("remote-partial-counts-file,p", po::value<string>(), "secret share file of partial counts")
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


        if(vm.count("semijoin-optimization")) {
            semijoin_optimization = true;
            selection_clause = "multisite";
        }



        if (vm.count("database")) {
            db_name = vm["database"].as<string>();
        }
        else {
            throw std::invalid_argument("Need database name for local inputs.");
        }


        if(vm.count("remote-tuples-file")) {
            remote_patient_file = vm["remote-tuples-file"].as<string>();
        }

        if (vm.count("year")) {
            study_year = vm["year"].as<string>();
            if(study_year != "all") {
                string year_selection = "study_year = " + study_year;
                selection_clause = PilotUtilities::appendToConjunctivePredicate(selection_clause, year_selection);
                partial_count_selection_clause = PilotUtilities::appendToConjunctivePredicate(partial_count_selection_clause, year_selection);
            }

        } else {
            throw std::invalid_argument("Need study year to know the data to input");
        }

        if(vm.count("remote-partial-counts-file")) {
            remote_patient_partial_count_file = vm["remote-partial-counts-file"].as<string>();
        }

        if(vm.count("cardinality-bound")) {
            cardinality_bound = vm["cardinality-bound"].as<size_t>();
        }




        // if semijoin optimization enabled and no partial input from chi OR vice versa
        if((semijoin_optimization && !remote_patient_file.empty()) && remote_patient_partial_count_file.empty())
            throw std::invalid_argument("Can't have secret shares from third party and no partial counts for them in semijoin optimization");
        if(!semijoin_optimization && !remote_patient_partial_count_file.empty())
            throw std::invalid_argument("Need secret shares for multisite tuples to go along with encrypted partial counts.");


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





    string output_path = Utilities::getCurrentWorkingDirectory() + "/pilot/secret_shares/xor/";
    // TODO: parameterize the default logging level
    Logger::setup(logfile_prefix);
    auto logger = vaultdb_logger::get();

    QuerySchema schema = SharedSchema::getInputSchema();
    NetIO *netio =  new emp::NetIO(party == ALICE ? nullptr : host.c_str(), port);
    setup_semi_honest(netio, party,  port);
    BOOST_LOG(logger) << "Starting epoch " << Utilities::getEpoch() << endl;
    auto e2e_start_time = emp::clock_start();


    start_time = emp::clock_start(); // reset timer to account for async start of alice and bob
    string measurements = "year,start_epoch,dedupe_and_setup,semijoin_optimization,rollup_end_epoch\n";
    measurements += study_year + "," = std::to_string(Utilities::getEpoch());

    // read inputs from two files, assemble with data of other host as one unioned secret shared table
    // expected order: alice, bob, chi

    patient_input_query = PilotUtilities::replaceSelection(patient_input_query, selection_clause);
    shared_ptr<SecureTable> inputData = UnionHybridData::unionHybridData(db_name, patient_input_query, remote_patient_file, netio, party);

    cumulative_runtime = time_from(start_time);


    // validate it against the DB for testing
    if(TESTBED) {
        std::shared_ptr<PlainTable> revealed = inputData->reveal();
        string query ="SELECT pat_id, age_strata, sex, ethnicity, race, numerator, denom_excl  FROM patient WHERE :selection ORDER BY pat_id, age_strata, sex, ethnicity, race, numerator, denom_excl";
        query = PilotUtilities::replaceSelection(query, selection_clause);

        SortDefinition patient_sort_def = DataUtilities::getDefaultSortDefinition(7);
        PilotUtilities::validateInputTable(PilotUtilities::unioned_db_name_, query, patient_sort_def, revealed);


    }



    BOOST_LOG(logger) << "***Read input on " << party << " in " <<    (cumulative_runtime + 0.0) *1e6*1e-9 << " ms, epoch " << Utilities::getEpoch() <<  endl;
    start_time = emp::clock_start();

    // create output dir:
    Utilities::mkdir(output_path);

    EnrichHtnQuery enrich(inputData, cardinality_bound);
    inputData.reset();

    assert(enrich.data_cube_->getTupleCount() == cardinality_bound);

    cumulative_runtime += time_from(start_time);
    measurements += "," + std::to_string(Utilities::getEpoch());

    BOOST_LOG(logger) << "***Completed cube aggregation at " << time_from(start_time)*1e6*1e-9 << " ms, cumulative runtime=" << cumulative_runtime*1e6*1e-9 << " ms, epoch " << Utilities::getEpoch() <<  endl;

    if(semijoin_optimization) {
        // add in the 1-site PIDs
        shared_ptr<SecureTable> alice, bob, chi;
        partial_count_query = PilotUtilities::replaceSelection(partial_count_query, partial_count_selection_clause);
        shared_ptr<PlainTable> local_partial_counts = DataUtilities::getQueryResults(db_name, partial_count_query, false);

        assert(local_partial_counts->getTupleCount() == cardinality_bound);
        // ship local, partial counts - alice, then bob
        if (party == 1) { // alice
            alice = SecureTable::secret_share_send_table(local_partial_counts, netio, ALICE);
            bob = SecureTable::secret_share_recv_table(*local_partial_counts->getSchema(), SortDefinition(), netio,
                                                       BOB);
        } else { // bob
            alice = SecureTable::secret_share_recv_table(*local_partial_counts->getSchema(), SortDefinition(),
                                                         netio, ALICE);
            bob = SecureTable::secret_share_send_table(local_partial_counts, netio, BOB);
        }


        chi = UnionHybridData::readSecretSharedInput(remote_patient_partial_count_file, QuerySchema::toPlain(*(alice->getSchema())), party);

        std::vector<shared_ptr<SecureTable>> partial_aggs { alice, bob, chi};
        enrich.unionWithPartialAggregates(partial_aggs);

        if(TESTBED) {
            std::shared_ptr<PlainTable> revealed = enrich.data_cube_->reveal();
            SortDefinition cube_sort_def = DataUtilities::getDefaultSortDefinition(4);

            string query = PilotUtilities::replaceSelection(PilotUtilities::data_cube_sql_, partial_count_selection_clause);
            PilotUtilities::validateInputTable(PilotUtilities::unioned_db_name_, query, cube_sort_def, revealed);
        }


    }
    measurements += "," + std::to_string(Utilities::getEpoch());

    BOOST_LOG(logger) << "Completed unioning for semijoin at epoch " << Utilities::getEpoch() << endl;

    shared_ptr<SecureTable> ageRollup = runRollup(0, "age_strata", party, enrich.data_cube_, output_path);
    shared_ptr<SecureTable> genderRollup = runRollup(1, "sex", party, enrich.data_cube_, output_path);
    shared_ptr<SecureTable> ethnicityRollup = runRollup(2, "ethnicity", party, enrich.data_cube_, output_path);
    shared_ptr<SecureTable> raceRollup = runRollup(3, "race", party, enrich.data_cube_, output_path);

    emp::finalize_semi_honest();

    delete netio;
    double runtime = time_from(e2e_start_time);

    measurements += "," + std::to_string(Utilities::getEpoch());

    BOOST_LOG(logger) << "Ending epoch " << Utilities::getEpoch() << endl;
    BOOST_LOG(logger) <<  "Test completed on " << party_name << " in " <<    (runtime+0.0)*1e6*1e-9 << " secs." <<  endl;
    cout << measurements << endl;

}
