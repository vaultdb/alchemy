#include "study_parser.h"
#include <boost/property_tree/json_parser.hpp>

using namespace catalyst;
using namespace vaultdb;
using boost::property_tree::ptree;


// modeled after vaultdb::PlanParser
StudyParser::StudyParser(const std::string plan_filename) {
    stringstream ss;
    std::vector<std::string> json_lines = DataUtilities::readTextFile(plan_filename);
    for(vector<string>::iterator pos = json_lines.begin(); pos != json_lines.end(); ++pos)
        ss << *pos << endl;

    // parse the JSON in boost
    boost::property_tree::ptree pt;
    boost::property_tree::read_json(ss, pt);



    // parse the study name
    study_.study_name_ = pt.get<std::string>("name");
    study_.db_name_ = pt.get<std::string>("db");
    study_.secret_shares_root_ = pt.get<std::string>("secret_shares_root");

    // drop and recreate DB if it exists
    Utilities::runCommand("psql -c 'DROP DATABASE IF EXISTS " + study_.db_name_ + "'");
    Utilities::runCommand("psql -c 'CREATE DATABASE " + study_.db_name_ + "'");

    // parse the input tables
    for (auto &table : pt.get_child("tables")) {
        InputTable input_table;
        input_table.name_ = table.second.get<std::string>("name");
        input_table.schema_ = QuerySchema(table.second.get<std::string>("schema"));
        for (auto &contributor : table.second.get_child("input_parties")) {
            input_table.data_contributors_.push_back(contributor.second.get_value<int>());
        }
        study_.input_tables_.push_back(input_table);

        // set up the schema in psql backend for use in parsing queries
        string create_table_statement = input_table.schema_.createTableStatement(input_table.name_);
        DataUtilities::runQueryNoOutput(study_.db_name_, create_table_statement);
    }


    // parse the queries
    for (auto &query : pt.get_child("queries")) {
        study_.query_path_ = query.second.get<std::string>("query_path");
        study_.dst_path_ = query.second.get<std::string>("dst_path");
        for(auto &query_file : query.second.get_child("names")) {
            cout << "Parsed query " << query_file.second.get_value<string>() << endl;
            // TODO: concat with path and ".json" suffix to get file
            // pass file into plan parser
        }
    }
}