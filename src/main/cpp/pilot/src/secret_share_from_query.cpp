#include <pilot/src/common/shared_schema.h>
#include <pilot/src/common/pilot_utilities.h>
#include <util/data_utilities.h>
#include <boost/program_options.hpp>


using namespace vaultdb;
using namespace std;
namespace po = boost::program_options;


int main(int argc, char **argv) {

    string selection_clause, db_name, dst_root, query;

    try {
        po::options_description desc("Options");
        desc.add_options()
                ("help", "print help message")
                ("database,D", po::value<string>(), "local database name")
                ("query-file,q", po::value<string>(), "file with query to run")
                ("year,y", po::value<string>(), "study year of experiment, in 2018, 2019, 2020, or all (or range, e.g., 2018-2019, inclusive)")
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


    string selection_query = PilotUtilities::replaceSelection(query, selection_clause);

    PilotUtilities::setupSystemConfiguration();
    PilotUtilities::secretShareFromQuery(db_name, selection_query, dst_root);

}

