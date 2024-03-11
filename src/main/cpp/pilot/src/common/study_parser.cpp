#include "study_parser.h"
#include <boost/property_tree/json_parser.hpp>
#include "util/table_manager.h"
#include "parser/plan_parser.h"

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

    if(study_.secret_shares_root_[0] != '/') {
        study_.secret_shares_root_ = Utilities::getCurrentWorkingDirectory() + "/" + study_.secret_shares_root_;
    }

    port_ = pt.get<int>("port");
    alice_host_ = pt.get<std::string>("alice_host");
    protocol_ = pt.get<std::string>("protocol");

    // drop and recreate DB if it exists
//    Utilities::runCommand("psql -c 'DROP DATABASE IF EXISTS " + study_.db_name_ + "'");
//    Utilities::runCommand("psql -c 'CREATE DATABASE " + study_.db_name_ + "'");
    TableManager & table_manager = TableManager::getInstance();

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
//        string create_table_statement = input_table.schema_.createTableStatement(input_table.name_);
//        DataUtilities::runQueryNoOutput(study_.db_name_, create_table_statement);
        table_manager.addEmptyTable<Bit>(input_table.name_, input_table.schema_);
    }


    // parse the queries
    study_.query_path_ = pt.get_child("queries").get<std::string>("query_path");
    study_.dst_path_ = pt.get_child("queries").get<std::string>("dst_path");
    for(auto &query_file : pt.get_child("queries").get_child("names")) {
        string query_name = query_file.second.get_value<string>();
        string fq_json_filename =  study_.query_path_ + "/" + query_name + ".json";
        if(fq_json_filename[0] != '/') {
            fq_json_filename = Utilities::getCurrentWorkingDirectory() + "/" + fq_json_filename;
        }
        // need to delay query parsing until EMP manager set up in case of local secret sharing
        // since port and host info are stored in JSON, we need to parse JSON to set up EMPManager
        query_files_[query_name] = fq_json_filename;
//        cout << "Parsing query " << fq_json_filename << endl;
//        study_.queries_[query_name] = PlanParser<Bit>::parse(study_.db_name_, fq_json_filename);

        }

}