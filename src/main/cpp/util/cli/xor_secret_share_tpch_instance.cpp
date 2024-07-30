#include "util/data_utilities.h"
#include "query_table/query_table.h"
#include "util/emp_manager/outsourced_mpc_manager.h"
#include "util/field_utilities.h"
#include "operators/stored_table_scan.h"
#include "util/parsing_utilities.h"

#include <boost/property_tree/json_parser.hpp>

#if __has_include("emp-rescu/emp-rescu.h")

#define VALIDATE 1
using namespace std;
using namespace vaultdb;
// customize this as needed
const string empty_db_ = "tpch_empty";
vector<string> hosts_ = vector<string>(N + 1, "127.0.0.1");
string dst_root_;
string db_name_;
SystemConfiguration & conf_ = SystemConfiguration::getInstance();

// encode whole db at once to preserve same delta for all of them
map<string, string> table_to_query_;
map<string, TableMetadata> table_to_metadata_;


void encodeTable(string table_name) {
    // metadata consists of schema and tuple count
    PlainTable *table = DataUtilities::getQueryResults(db_name_, table_to_query_.at(table_name), false);
    auto shares = table->generateSecretShares(N);

    // metadata file schema, collation, and tuple count
    string metadata_filename = dst_root_ + "/" + table_name + ".metadata";
    auto md = table_to_metadata_.at(table_name);
    table->order_by_ = md.collation_;
    string metadata = md.schema_.prettyPrint() + "\n"
                + DataUtilities::printSortDefinition(md.collation_) + "\n"
               + std::to_string(md.tuple_cnt_);
    DataUtilities::writeFile(metadata_filename, metadata);
    cout << "Wrote metadata for " << table_name << " to " << metadata_filename << '\n';

    cout << "First rows encoded: " << table->toString(5, true);

    for(int i = 0; i < N; ++i) {
        string secret_shares_file = dst_root_ + "/" + table_name + "." + std::to_string(i+1);
        cout << "Writing xor shares starting with " << DataUtilities::printByteArray(shares[i].data(), 10) << endl;
        DataUtilities::writeFile(secret_shares_file, shares[i]);
    }

    delete table;
}

int main(int argc, char **argv) {

    // paths are relative to local $VAULTDB_ROOT, the src/main/cpp within your vaultdb directory
    // usage: ./xor_secret_share_tpch_instance <db name> <destination root>
    // e.g., ./xor_secret_share_tpch_instance tpch_unioned_600 wires/tpch_unioned_600
    if (argc < 3) {
        std::cout << "usage: ./xor_secret_share_tpch_instance <db name> <destination root>" << std::endl;
        exit(-1);
    }


     db_name_ =  argv[1];
     string current_dir = Utilities::getCurrentWorkingDirectory();
     dst_root_ = current_dir + "/" + argv[2];

    cout << "DB Name: " << db_name_ << endl;
    cout << "Destination dir: " << dst_root_ << endl;

    // attempt to create target dir
    string dst_dir = dst_root_.substr(0, dst_root_.find_last_of("/"));
    Utilities::mkdir(dst_dir);
    Utilities::mkdir(dst_root_);

    auto table_specs = ParsingUtilities::loadTableInfoFromJson(current_dir + "/table_config.json");
    table_to_query_ = table_specs.first;
    table_to_metadata_ = table_specs.second;

    // plaintext mode
    conf_.emp_manager_ = new OutsourcedMpcManager();
    conf_.setEmptyDbName(empty_db_);
    BitPackingMetadata md = FieldUtilities::getBitPackingMetadata(argv[1]);
    conf_.initialize(db_name_, md, StorageModel::COLUMN_STORE);
    conf_.stored_db_path_ = dst_root_;

    for(auto const &entry : table_to_query_) {
            encodeTable(entry.first);
    }

    // validate it
    if(VALIDATE) {
        for (auto table_entry: table_to_query_) {
            string table_name = table_entry.first;
            string query = table_entry.second;
            PlainTable *expected = DataUtilities::getQueryResults(argv[1], query, false);

                vector<int8_t> src_data = DataUtilities::readFile(dst_root_ + "/" + table_name + ".1");
                for (int i = 2; i <= N; ++i) {
                    auto tmp = DataUtilities::readFile(dst_root_ + "/" + table_name + "." + std::to_string(i));
                    cout << "Reading shares starting with " << DataUtilities::printByteArray(tmp.data(), 10) << endl;
                    for (int j = 0; j < src_data.size(); ++j) {
                        src_data[j] ^= tmp[j];
                    }
                }
                cout << "Revealed bytes: " << DataUtilities::printByteArray(src_data.data(), 10) << endl;
                PlainTable *recvd = QueryTable<bool>::deserialize(expected->getSchema(), src_data);
                cout << "First rows recv'd: " << recvd->toString(5, true);
                expected->order_by_ = recvd->order_by_;
                assert(*expected == *recvd);

                delete recvd;
                delete expected;
            }

    }

}
#else
int main(int argc, char **argv) {
    std::cout << "emp-rescu backend not found!" << std::endl;
}

#endif
