#include "study_parser.h"
#include <boost/property_tree/json_parser.hpp>
#include "util/table_manager.h"

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
    study_.secret_shares_path_ = Utilities::getFullyQualifiedPath(pt.get<std::string>("secret_shares_path"));

    port_ = pt.get<int>("port");
    alice_host_ = pt.get<std::string>("alice_host");
    protocol_ = pt.get<std::string>("protocol");
    study_.min_cell_cnt_ = pt.get<int>("min_cell_count");
    string query_path = Utilities::getFullyQualifiedPath(pt.get<std::string>("query_path"));
    study_.dst_path_ = Utilities::getFullyQualifiedPath(pt.get<std::string>("dst_path"));

    Utilities::mkdir(study_.dst_path_);


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
        table_manager.addEmptyTable(input_table.name_, input_table.schema_);
    }


    // parse the queries

    for(auto &query_spec : pt.get_child("queries")) {
        QuerySpec s;
        s.name_ = query_spec.second.get<std::string>("name");

        string fq_json_filename =  query_path + "/" + s.name_ + ".json";

        // need to delay query parsing until EMP manager set up in case of local secret sharing
        // since port and host info are stored in JSON, we need to parse JSON to set up EMPManager
        s.plan_file_ = fq_json_filename;
        for(auto &col : query_spec.second.get_child("count_cols")) {
            s.count_cols_.push_back(col.second.get_value<int>());
        }
        study_.queries_[s.name_] = s;
    }

}