#include <pilot/src/common/shared_schema.h>
#include <pilot/src/common/pilot_utilities.h>
#include <emp-sh2pc/emp-sh2pc.h>
#include <util/data_utilities.h>
#include "common/union_hybrid_data.h"
#include "enrich_htn_query.h"
#include <util/utilities.h>
#include <util/logger.h>
#include <boost/program_options.hpp>
#include <operators/union.h>
#include <operators/sort.h>



using namespace std;
using namespace vaultdb;
using namespace emp;
namespace po = boost::program_options;

#define TESTBED 1


auto start_time = emp::clock_start();
auto cumulative_runtime = emp::time_from(start_time);

// roll up one group-by col at a time
// input schema:
// age_strata (0), sex (1), ethnicity (2) , race (3), numerator_cnt (4), denominator_cnt (5)
shared_ptr<SecureTable>
runRollup(int idx, string colName, int party, shared_ptr<SecureTable> &data_cube, const string &output_path) {
    auto local_start_time = emp::clock_start();
    auto logger = vaultdb_logger::get();
    shared_ptr<SecureTable> stratified = PilotUtilities::rollUpAggregate(data_cube, idx);

    std::vector<int8_t> results = stratified->reveal(emp::XOR)->serialize();

    std::string suffix = (party == emp::ALICE) ? "alice" : "bob";
    std::string output_file = output_path + "/" + colName + "." + suffix;
    DataUtilities::writeFile(output_file, results);

    // validate it against the DB for testing
    if(TESTBED) {
        SortDefinition orderBy = DataUtilities::getDefaultSortDefinition(2);

        shared_ptr<PlainTable> revealed = stratified->reveal();
        revealed = DataUtilities::removeDummies(revealed);

        string query = PilotUtilities::getRollupExpectedResultsSql(colName);
        PilotUtilities::validateInputTable(PilotUtilities::unioned_db_name_, query, orderBy, revealed);

        // write it out
        string csv, schema;
        string out_path = Utilities::getCurrentWorkingDirectory() + "/pilot/secret_shares/validate";
        string out_file = out_path + "/" + colName + ".csv";
        Utilities::mkdir(out_path);

        std::stringstream schema_str;
        schema_str << *revealed->getSchema() << std::endl;
        csv = schema_str.str();

        for(unsigned int i = 0; i < revealed->getTupleCount(); ++i) {
            csv += revealed->getTuple(i).toString();
        }

        DataUtilities::writeFile(out_file, revealed->toString());


    }


    auto delta = time_from(local_start_time);
    cumulative_runtime += delta;

    BOOST_LOG(logger) <<  "***Done " << colName << " rollup at " << delta*1e6*1e-9 << " ms, cumulative time: " << cumulative_runtime << " epoch " << Utilities::getEpoch() <<  endl;


    return stratified;
}



int main(int argc, char **argv) {

    // local input file is an (unencrypted) csv of local site's data
    // secret share file is a binary, e.g., Chicago Alliance input
    // parse args
    // local input file is an (unencrypted) csv of local site's data
    // secret share file is a binary, e.g., Chicago Alliance input
    // parse args
    string host, remote_patient_file, remote_patient_partial_count_file, db_name;
    int port=0, party=0;
    bool semijoin_optimization = false;
    size_t cardinality_bound = 441; // 7 * 3 * 3 * 7 * X study_years
    string study_year, party_name;
    string logfile_prefix;
    string patient_input_query = DataUtilities::readTextFileToString("pilot/queries/patient.sql");
    string partial_count_query = DataUtilities::readTextFileToString("pilot/queries/partial-count.sql");
    string selection_clause;
    string partial_count_selection_clause;
    size_t batch_count;
    string output_path = Utilities::getCurrentWorkingDirectory() + "/pilot/secret_shares/xor/";



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
                ("cardinality-bound,b", po::value<size_t>(), "cardinality bound for output of aggregation.  Equal to the cross-product of all group-bys (e.g., age/sex/ethnicity/race)")
                ("batch-count", po::value<uint32_t>(), "number of partitions with which to evaluate query");


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


        if (vm.count("batch-count")) {
            batch_count = vm["batch-count"].as<uint32_t>();
        }
        else {
            throw std::invalid_argument("Need batch count to partition inputs.");
        }

        if(vm.count("remote-tuples-file")) {
            remote_patient_file = vm["remote-tuples-file"].as<string>();
        }

        if (vm.count("year")) {
            study_year = vm["year"].as<string>();
            string year_selection = PilotUtilities::parseYearSelection(study_year);
            if(!year_selection.empty()) {
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
        else {
            int study_length = PilotUtilities::getStudyLength(study_year);
            cardinality_bound = 441*study_length;
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





    Logger::setup(logfile_prefix);
    auto logger = vaultdb_logger::get();
    EnrichHtnQuery enrich; // empty for now

    auto *netio =  new emp::NetIO(party == ALICE ? nullptr : host.c_str(), port);
    setup_semi_honest(netio, party,  port);
    BOOST_LOG(logger) << "Starting epoch " << Utilities::getEpoch() << endl;
    auto e2e_start_time = emp::clock_start();
    shared_ptr<SecureTable> partial_counts;
    start_time = emp::clock_start(); // reset timer to account for async start of alice and bob


    string measurements = "year,batches,start_epoch,dedupe_and_setup,semijoin_optimization,rollup_end_epoch\n";
    measurements += study_year + "," + std::to_string(batch_count) + "," + std::to_string(Utilities::getEpoch());

    // read inputs from two files, assemble with data of other host as one unioned secret shared table
    // expected order: alice, bob, chi
    // replace :selection variable
    for(uint32_t batch_id = 0; batch_id < batch_count; ++batch_id) {
        string batch_predicate = selection_clause;
        batch_predicate = PilotUtilities::appendToConjunctivePredicate(selection_clause, "MOD(hash, " + std::to_string(batch_count) + ") = " + std::to_string(batch_id));
        string batch_input_query = PilotUtilities::replaceSelection(patient_input_query,  batch_predicate);
        string batch_input_file = remote_patient_file + "." + std::to_string(batch_id + 1) + "." + party_name;
        shared_ptr<SecureTable> inputData =  UnionHybridData::unionHybridData(db_name, batch_input_query,
                                                                                         batch_input_file, netio,
                                                                                         party);
        cumulative_runtime = time_from(start_time);


        // validate it against the DB for testing
        if (TESTBED) {
            SortDefinition patientSortDef = DataUtilities::getDefaultSortDefinition(9);
            std::shared_ptr<PlainTable> revealed = inputData->reveal();

            string query =  DataUtilities::readTextFileToString("pilot/queries/patient.sql");
            query = PilotUtilities::replaceSelection(query, batch_predicate);

            PilotUtilities::validateInputTable(PilotUtilities::unioned_db_name_, query, patientSortDef, revealed);
        }


        BOOST_LOG(logger) << "***Read input on " << party << " in " << (cumulative_runtime + 0.0) * 1e6 * 1e-9
                          << " ms, epoch " << Utilities::getEpoch() << endl;
        start_time = emp::clock_start();

        // create output dir:
        Utilities::mkdir(output_path);

        EnrichHtnQuery batch_enrich(inputData, cardinality_bound);
        inputData.reset();

        if(partial_counts.get() == nullptr) {
            partial_counts = batch_enrich.data_cube_;
        }
        else {
            // suspect this might be a problem of bits vs bytes in the copy for Union
            Union u(partial_counts, batch_enrich.data_cube_);
            partial_counts = u.run();

        }

        cumulative_runtime += time_from(start_time);
        BOOST_LOG(logger) << "***Completed cube aggregation " << batch_id + 1 << " of " << batch_count << " at " << time_from(start_time) * 1e6 * 1e-9
                          << " ms, cumulative runtime=" << cumulative_runtime * 1e6 * 1e-9 << " ms, epoch "
                          << Utilities::getEpoch() << endl;

    } // end for-each batch

    // can't just add up partial counts because the domains might be different in each set
    // instead concatenate them all into a single array and aggregate them
    enrich.data_cube_  = EnrichHtnQuery::aggregatePartialPatientCounts(partial_counts, cardinality_bound);

    measurements += "," + std::to_string(Utilities::getEpoch());

    if(semijoin_optimization) {
        // add in the 1-site PIDs
        shared_ptr<SecureTable> alice, bob, chi;
        partial_count_query = PilotUtilities::replaceSelection(partial_count_query, partial_count_selection_clause);
        shared_ptr<PlainTable> local_partial_counts =  DataUtilities::getQueryResults(db_name, partial_count_query, false);
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


        chi = UnionHybridData::readSecretSharedInput(remote_patient_partial_count_file, QuerySchema::toPlain(*(alice->getSchema())), party);


        std::vector<shared_ptr<SecureTable>> partial_aggs { alice, bob, chi};
        enrich.unionWithPartialAggregates(partial_aggs);


    }



    if(TESTBED) {
        std::shared_ptr<PlainTable> revealed = enrich.data_cube_->reveal();
        SortDefinition cube_sort_def = DataUtilities::getDefaultSortDefinition(5);
        string query = PilotUtilities::replaceSelection(PilotUtilities::data_cube_sql_no_dummies_, selection_clause);
        PilotUtilities::validateInputTable(PilotUtilities::unioned_db_name_, query, cube_sort_def, revealed);
    }


    measurements += "," + std::to_string(Utilities::getEpoch());

    BOOST_LOG(logger) << "Completed unioning for semijoin at epoch " << Utilities::getEpoch() << endl;

    shared_ptr<SecureTable> ageRollup = runRollup(1, "age_strata", party, enrich.data_cube_, output_path);
    shared_ptr<SecureTable> genderRollup = runRollup(2, "sex", party, enrich.data_cube_, output_path);
    shared_ptr<SecureTable> ethnicityRollup = runRollup(3, "ethnicity", party, enrich.data_cube_, output_path);
    shared_ptr<SecureTable> raceRollup = runRollup(4, "race", party, enrich.data_cube_, output_path);

    double runtime = time_from(e2e_start_time);
    measurements += "," + std::to_string(Utilities::getEpoch());



    BOOST_LOG(logger) << "Ending epoch " << Utilities::getEpoch() << endl;
    BOOST_LOG(logger) <<  "Test completed on " << party_name << " in " <<    (runtime+0.0)*1e6*1e-9 << " secs." <<  endl;
    cout << measurements << endl;

    cout << "Age rollup: " << endl;
    cout << ageRollup->reveal()->toString() << endl;

    cout << "Sex rollup: " << endl;
    cout << genderRollup->reveal()->toString() << endl;


    cout << "Ethnicity rollup: " << endl;
    cout << ethnicityRollup->reveal()->toString() << endl;

    cout << "Race rollup: " << endl;
    cout << raceRollup->reveal()->toString() << endl;



    emp::finalize_semi_honest();

    delete netio;


}


