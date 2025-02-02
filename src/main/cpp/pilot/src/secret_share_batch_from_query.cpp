#include <pilot/src/common/shared_schema.h>
#include <pilot/src/common/pilot_utilities.h>
#include <util/data_utilities.h>
#include <boost/program_options.hpp>


using namespace vaultdb;
using namespace std;
namespace po = boost::program_options;

// paths are relative to $VAULTDB_ROOT/src/main/cpp
// e.g., ./bin/secret_share_from_query enrich_htn_prod  my_query.sql pilot/secret_shares/output/example
// writes to  pilot/secret_shares/output/example.alice and  pilot/secret_shares/output/example.bob
int main(int argc, char **argv) {

    string selection_clause, db_name, dst_root, query;
    uint32_t batch_count;

    try {
        po::options_description desc("Options");
        desc.add_options()
                ("help", "print help message")
                ("database,D", po::value<string>(), "local database name")
                ("query-file,q", po::value<string>(), "file with query to run")
                ("year,y", po::value<string>(), "study year of experiment, in 2018, 2019, 2020, or all (or range, e.g., 2018-2019, inclusive)")
                ("batch-count,b", po::value<uint32_t>(), "Number of batches to partition query work")
                ("semijoin-optimization,s", "enable this to read in multisite tuples only")
                ("dest-root,d",  po::value<string>(), "destination root");



        po::variables_map vm;
        po::store(po::parse_command_line(argc, argv, desc), vm);
        po::notify(vm);

        if (vm.count("help")) {
            cout << desc << "\n";
            return 0;
        }

        if (vm.count("database")) {
            db_name = vm["database"].as<string>();
        }
        else {
            throw std::invalid_argument("Error: need target db on which the query will run!");
        }


        if(vm.count("query-file")) {
            query = DataUtilities::readTextFileToString(vm["query-file"].as<string>());
        }
        else {
            throw std::invalid_argument("Need sql file with query to run.");
        }

        if (vm.count("year")) {
            string study_year = vm["year"].as<string>();
            if(study_year != "all" && (study_year.find('-') == string::npos)) {
                selection_clause = "study_year = " + study_year;
            }
            else if(study_year.find('-') != string::npos) {
                selection_clause = PilotUtilities::getRangePredicate(study_year);
            }
            else
                assert(study_year == "all");
        }

        if(vm.count("semijoin-optimization")) {
            selection_clause = PilotUtilities::appendToConjunctivePredicate(selection_clause, "multisite");
        }

        if(vm.count("batch-count")) {
            batch_count = vm["batch-count"].as<uint32_t>();
        }
        else {
            throw std::invalid_argument("Need a batch count > 0 to run.  Otherwise use ./bin/secret_share_from_query for a single batch");
        }

        if(vm.count("dest-root")) {
            dst_root = vm["dest-root"].as<string>();
        }
        else
        {
            throw std::invalid_argument("Need a file root with which to write secret share files.");
        }

    }
    catch(exception& e) {
        cerr << "error: " << e.what() << "\n";
        return 1;
    }
    catch(...) {
        cerr << "Exception of unknown type!\n";
    } // end parsing


    PilotUtilities::setupSystemConfiguration();



    for(uint32_t batch_id  = 0; batch_id < batch_count; ++batch_id) {
        string batch_predicate = "MOD(hash, " + std::to_string(batch_count) + ") = " + std::to_string(batch_id);
        batch_predicate = PilotUtilities::appendToConjunctivePredicate(selection_clause, batch_predicate);
        string batch_query = PilotUtilities::replaceSelection(query, batch_predicate);
        string batch_dst_root = dst_root + "." + std::to_string(batch_id + 1); // [1...N]
        PilotUtilities::secretShareFromQuery(db_name, batch_query, batch_dst_root);
    }


}


