#ifndef _PARSING_UTILITIES_H_
#define _PARSING_UTILITIES_H_
#include "data_utilities.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

using boost::property_tree::ptree;


namespace vaultdb {
        class ParsingUtilities {
        public:
            static pair<map<string, string>, map<string, TableMetadata> > loadTableInfoFromJson(string json_path) {
                stringstream ss;
                map<string, TableMetadata> table_to_metadata;
                map<string, string> table_to_query;
                std::vector<std::string> json_lines = DataUtilities::readTextFile(json_path);
                for (vector<string>::iterator pos = json_lines.begin(); pos != json_lines.end(); ++pos)
                    ss << *pos << endl;

                boost::property_tree::ptree pt;
                boost::property_tree::read_json(ss, pt);

                if (pt.count("tables") > 0) {
                    boost::property_tree::ptree tables = pt.get_child("tables");

                    for (ptree::const_iterator it = tables.begin(); it != tables.end(); ++it) {
                        string table_name = "";
                        TableMetadata md;

                        if (it->second.count("table_name") > 0) {
                            table_name = it->second.get_child("table_name").get_value<string>();
                        } else {
                            throw std::runtime_error("No table_name found in table_config.json");
                        }

                        if (it->second.count("query") > 0) {
                            table_to_query[table_name] = it->second.get_child("query").get_value<string>();
                        } else {
                            throw std::runtime_error("No query found in table_config.json");
                        }

                        if (it->second.count("schema") > 0) {
                            md.schema_ = QuerySchema(it->second.get_child("schema").get_value<string>());

                        } else {
                            throw std::runtime_error("No schema found in table_config.json");
                        }

                        if (it->second.count("tuple_count") > 0) {
                            md.tuple_cnt_ = it->second.get_child("tuple_count").get_value<int>();
                        } else {
                            throw std::runtime_error("No tuple_count found in table_config.json");
                        }


                        if (it->second.count("collations") > 0) {
                            boost::property_tree::ptree collations = it->second.get_child("collations");

                            for (ptree::const_iterator collation_it = collations.begin();
                                 collation_it != collations.end(); ++collation_it) {
                                int column = -1;
                                if (collation_it->second.count("column") > 0) {
                                    column = collation_it->second.get_child("column").get_value<int>();
                                } else {
                                    throw std::runtime_error("No column found in table_config.json");
                                }

                                SortDirection direction = SortDirection::INVALID;
                                if (collation_it->second.count("direction") > 0) {
                                    if (collation_it->second.get_child("direction").get_value<string>() ==
                                        "ASCENDING") {
                                        direction = SortDirection::ASCENDING;
                                    } else if (collation_it->second.get_child("direction").get_value<string>() ==
                                               "DESCENDING") {
                                        direction = SortDirection::DESCENDING;
                                    } else {
                                        throw std::runtime_error("Invalid direction found in table_config.json");
                                    }
                                } else {
                                    throw std::runtime_error("No direction found in table_config.json");
                                }

                                md.collation_.push_back(ColumnSort(column, direction));
                            }
                        } else {
                            throw std::runtime_error("No collation found in table_config.json");
                        }
                        table_to_metadata[table_name] = md;
                    }

                }

                return std::pair(table_to_query, table_to_metadata);

            }

            static map<string, TableMetadata> parseTableMetadata(const string & db_path) {
                string all_tables = Utilities::runCommand("ls *.metadata", db_path);
                vector<string> tables = Utilities::splitStringByNewline(all_tables);
                map<string, TableMetadata> table_metadata;

                for(auto & metadata_file : tables) {
                    TableMetadata md;
                    md.name_ = metadata_file.substr(0, metadata_file.size() - 9);
                    auto metadata = DataUtilities::readTextFile(db_path + "/" + metadata_file);
                    // drop ".metadata" suffix
                    md.schema_ = QuerySchema(metadata.at(0));
                    md.collation_ = DataUtilities::parseCollation(metadata.at(1));
                    md.tuple_cnt_ = atoi(metadata.at(2).c_str());
                    table_metadata[md.name_] = md;
                }
                return table_metadata;
            }

            static ConnectionInfo parseIPsFromJson(const std::string &config_json_path) {
                stringstream ss;
                ConnectionInfo conn_info;
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
                                conn_info.hosts_[host_idx-1] = it->second.get_child(std::to_string(host_idx)).get_value<string>();
                            }
                            else {
                                throw std::runtime_error("Host " + std::to_string(host_idx) + " is not found in config.json");
                            }
                        }

                        if(it->second.count(std::to_string(emp::TP)) > 0) {
                            conn_info.hosts_[N] = it->second.get_child(std::to_string(emp::TP)).get_value<std::string>();
                        }
                        else {
                            throw std::runtime_error("No tp host found in config.json");
                        }
                    }

                    conn_info.port_ = pt.get_child("port").get_value<int32_t>();
                    conn_info.ctrl_port_ = pt.get_child("ctrl_port").get_value<int32_t>();
                }
                else {
                    throw std::runtime_error("No hosts found in config.json");
                }
                return conn_info;
            }


        };
}

#endif