#include "util/data_utilities.h"
#include "query_table/query_table.h"
#include "util/emp_manager/outsourced_mpc_manager.h"
#include "util/field_utilities.h"
#include "query_table/column_table.h"
#include "util/table_manager.h"
#include <boost/property_tree/json_parser.hpp>



using namespace std;
using namespace vaultdb;

// customize this as needed
const string empty_db_ = "tpch_empty";
vector<string> hosts_ = vector<string>(N + 1, "127.0.0.1");
int port_ = 55370;
int ctrl_port_ = 55380;
string db_name_;
string dst_root_;
int party_count_ = -1;
int party_ = -1;
SystemConfiguration & conf_ = SystemConfiguration::getInstance();

map<string, string> table_to_query;
map<string, string> table_to_schema;
map<string, int> table_to_tuple_count;
map<string, string> table_to_collation_;

void parseIPsFromJson(const std::string &config_json_path) {
    stringstream ss;
    std::vector<std::string> json_lines = DataUtilities::readTextFile(config_json_path);
    for(vector<string>::iterator pos = json_lines.begin(); pos != json_lines.end(); ++pos)
        ss << *pos << endl;

    boost::property_tree::ptree pt;
    boost::property_tree::read_json(ss, pt);

    if(pt.count("hosts") > 0) {
        boost::property_tree::ptree hosts = pt.get_child("hosts");

        for(ptree::const_iterator it = hosts.begin(); it != hosts.end(); ++it) {
            for(int host_idx = 1; host_idx <= emp::N; ++host_idx) {
                if(it->second.count(std::to_string(host_idx)) > 0) {
                    hosts_[host_idx-1] = it->second.get_child(std::to_string(host_idx)).get_value<string>();
                }
                else {
                    throw std::runtime_error("Host " + std::to_string(host_idx) + " is not found in config.json");
                }
            }

            if(it->second.count(std::to_string(emp::TP)) > 0) {
                hosts_[N] = it->second.get_child(std::to_string(emp::TP)).get_value<std::string>();
            }
            else {
                throw std::runtime_error("No tp host found in config.json");
            }
        }

        port_ = pt.get_child("port").get_value<int32_t>();
        ctrl_port_ = pt.get_child("ctrl_port").get_value<int32_t>();
    }
    else {
        throw std::runtime_error("No hosts found in config.json");
    }
}

void loadTableInfoFromJson(string table_json_path) {
    stringstream ss;
    std::vector<std::string> json_lines = DataUtilities::readTextFile(table_json_path);
    for(vector<string>::iterator pos = json_lines.begin(); pos != json_lines.end(); ++pos)
        ss << *pos << endl;

    boost::property_tree::ptree pt;
    boost::property_tree::read_json(ss, pt);

    if(pt.count("tables") > 0) {
        boost::property_tree::ptree tables = pt.get_child("tables");

        for(ptree::const_iterator it = tables.begin(); it != tables.end(); ++it) {
            string table_name = "";

            if(it->second.count("table_name") > 0) {
                table_name = it->second.get_child("table_name").get_value<string>();
            }
            else {
                throw std::runtime_error("No table_name found in table_config.json");
            }

            if(it->second.count("query") > 0) {
                table_to_query[table_name] = it->second.get_child("query").get_value<string>();
            }
            else {
                throw std::runtime_error("No query found in table_config.json");
            }

            if(it->second.count("schema") > 0) {
                table_to_schema[table_name] = it->second.get_child("schema").get_value<string>();
            }
            else {
                throw std::runtime_error("No schema found in table_config.json");
            }

            if(it->second.count("tuple_count") > 0) {
                table_to_tuple_count[table_name] = it->second.get_child("tuple_count").get_value<int>();
            }
            else {
                throw std::runtime_error("No tuple_count found in table_config.json");
            }

            vector<ColumnSort> sort_def;

            if(it->second.count("collations") > 0) {
                boost::property_tree::ptree collations = it->second.get_child("collations");

                for(ptree::const_iterator collation_it = collations.begin(); collation_it != collations.end(); ++collation_it) {
                    int column = -1;
                    if(collation_it->second.count("column") > 0) {
                        column = collation_it->second.get_child("column").get_value<int>();
                    }
                    else {
                        throw std::runtime_error("No column found in table_config.json");
                    }

                    SortDirection direction = SortDirection::INVALID;
                    if(collation_it->second.count("direction") > 0) {
                        if(collation_it->second.get_child("direction").get_value<string>() == "ASCENDING") {
                            direction = SortDirection::ASCENDING;
                        }
                        else if(collation_it->second.get_child("direction").get_value<string>() == "DESCENDING") {
                            direction = SortDirection::DESCENDING;
                        }
                        else {
                            throw std::runtime_error("Invalid direction found in table_config.json");
                        }
                    }
                    else {
                        throw std::runtime_error("No direction found in table_config.json");
                    }

                    sort_def.push_back(ColumnSort(column, direction));
                }

                table_to_collation_[table_name] = DataUtilities::printSortDefinition(sort_def);
            }
            else {
                throw std::runtime_error("No collation found in table_config.json");
            }
        }
    }
}

void secret_share_table(string table_name) {
    PlainTable *table = DataUtilities::getQueryResults(db_name_, table_to_query[table_name], false);

    vector<vector<int8_t> > shares = table->generateSecretShares(party_count_);

    for(int i = 0; i < party_count_; i++) {
        string postfix;
        if(i == 0) {
            postfix = "10086";
        }
        else {
            postfix = to_string(i);
        }
        string share_filename = table_name + "."  + postfix;
        DataUtilities::writeFile(dst_root_ + "/" + share_filename, shares[i]);
    }
    cout << "Wrote " << party_count_ << " shares to " << dst_root_ << "/" << table_name << ".[0-" << party_count_ - 1 << "]" << endl;

    string metadata_filename = dst_root_ + "/" + table_name + ".metadata";
    string metadata = QuerySchema::toPlain(table_to_schema.at(table_name)).prettyPrint() + "\n"
                      + table_to_collation_.at(table_name) + "\n"
                      + std::to_string(table_to_tuple_count.at(table_name));
    DataUtilities::writeFile(metadata_filename, metadata);
    cout << "Wrote metadata for " << table_name << " to " << metadata_filename << '\n';

    delete table;
}

int main(int argc, char **argv) {

    // paths are relative to local $VAULTDB_ROOT, the src/main/cpp within your vaultdb directory
    // usage: ./secret_share_tpch_instance <db name> <destination root> <party count> <party>
    // e.g., ./secret_share_tpch_instance tpch_unioned_150 shares 4 1
    if (argc < 5) {
        std::cout << "usage: ./secret_share_tpch_instance <db name> <destination root> <party count> <party>" << std::endl;
        exit(-1);
    }

    string current_dir = Utilities::getCurrentWorkingDirectory();

    db_name_ = argv[1];

    dst_root_ = current_dir + "/" + argv[2] + "/" + argv[1];
    string dst_dir = dst_root_.substr(0, dst_root_.find_last_of("/"));
    Utilities::mkdir(dst_dir);
    Utilities::mkdir(dst_root_);

    party_count_ = atoi(argv[3]);

    party_ = atoi(argv[4]);

    loadTableInfoFromJson(current_dir + "/table_config.json");

    // set up OMPC
    // to enable wire packing set storage model to StorageModel::PACKED_COLUMN_STORE
    parseIPsFromJson(current_dir + "/config.json");
    EmpManager *manager = new OutsourcedMpcManager(hosts_.data(), party_, port_, ctrl_port_);

    conf_.emp_manager_ = manager;
    conf_.setEmptyDbName(empty_db_);
    BitPackingMetadata md = FieldUtilities::getBitPackingMetadata(argv[1]);
    conf_.initialize(db_name_, md, StorageModel::COLUMN_STORE);
    // TODO: disable bit packing for secret sharing loading
    conf_.clearBitPacking();

    for(auto const& entry : table_to_query) {
        secret_share_table(entry.first);
    }
}