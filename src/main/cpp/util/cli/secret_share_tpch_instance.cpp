#include "util/data_utilities.h"
#include "query_table/query_table.h"
#include "util/emp_manager/outsourced_mpc_manager.h"
#include "query_table/buffered_column_table.h"
#include "operators/stored_table_scan.h"

#include <boost/property_tree/json_parser.hpp>
#include "util/parsing_utilities.h"

// #if __has_include("emp-rescu/emp-rescu.h")
// only make this if we are not using emp-sh2pc
#if !__has_include("emp-sh2pc/emp-sh2pc.h")

#define VALIDATE 1
using namespace std;
using namespace vaultdb;
// customize this as needed
const string empty_db_ = "tpch_empty";
vector<string> hosts_ = vector<string>(N + 1, "127.0.0.1");
string dst_root_;
string temp_data_path_;
string db_name_;
int party_ = -1;
SystemConfiguration & conf_ = SystemConfiguration::getInstance();

// encode whole db at once to preserve same delta for all of them
map<string, string> table_to_query_;
map<string, TableMetadata> table_to_metadata_;


void encodeTable(string table_name) {
    // metadata consists of schema and tuple count
    PlainTable *plain_table = DataUtilities::getQueryResults(db_name_, table_to_query_.at(table_name), false);

    auto md = table_to_metadata_.at(table_name);
    // metadata file schema, collation, and tuple count
    //if(conf_.inputParty()) {
    // Every party needs to read metadata in ompc base test
    string metadata_filename = dst_root_ + "/" + table_name + ".metadata";
    plain_table->order_by_ = md.collation_;
    string metadata = md.schema_.prettyPrint() + "\n"
                      + DataUtilities::printSortDefinition(md.collation_) + "\n"
                      + std::to_string(md.tuple_cnt_);
    DataUtilities::writeFile(metadata_filename, metadata);
    cout << "Wrote metadata for " << table_name << " to " << metadata_filename << '\n';
    //}

    std::string secret_shares_file = Utilities::getFilenameForTable(table_name);

    BufferedColumnTable * secure_table = (BufferedColumnTable *) plain_table->secretShare();

    std::this_thread::sleep_for(std::chrono::seconds(1));

    conf_.bpm_.flushTable<Bit>(secure_table->table_id_);
    secure_table->table_shares_file_.flush();
    secure_table->table_shares_file_.close();

    std::this_thread::sleep_for(std::chrono::seconds(1));

    // rename the file to table name
    std::filesystem::copy_file(secure_table->table_file_name_.c_str(), secret_shares_file.c_str(), std::filesystem::copy_options::overwrite_existing);

    std::this_thread::sleep_for(std::chrono::seconds(1));

    cout << "Wrote secret shares for " << table_name << " to " << secret_shares_file << '\n';

    secure_table->table_shares_file_.open(secure_table->table_file_name_.c_str(), std::ios::in | std::ios::out | std::ios::binary);

    delete plain_table;
    delete secure_table;
}

int main(int argc, char **argv) {

    // paths are relative to local $VAULTDB_ROOT, the src/main/cpp within your vaultdb directory
    // usage: ./secret_share_tpch_instance <db name> <destination root> <party>
    // e.g., ./secret_share_tpch_instance tpch_unioned_600 wires/tpch_unioned_600 10086
    if (argc < 4) {
        std::cout << "usage: ./secret_share_tpch_instance <db name> <destination root> <party>" << std::endl;
        exit(-1);
    }


    party_ = atoi(argv[3]);
    db_name_ = (party_ == emp::TP) ? argv[1] : empty_db_;
    string current_dir = Utilities::getCurrentWorkingDirectory();
    dst_root_ = current_dir + "/" + argv[2];
    temp_data_path_ = current_dir + "/shares/temp";
    ConnectionInfo c = ParsingUtilities::parseIPsFromJson(current_dir + "/conf/config.json");

    cout << "Party: " << party_ << endl;
    cout << "DB Name: " << db_name_ << endl;
    cout << "Destination dir: " << dst_root_ << endl;
    cout << "Ports: " << c.port_ << ", " << c.ctrl_port_ << endl;

    // attempt to create target dir
    string dst_dir = dst_root_.substr(0, dst_root_.find_last_of("/"));
    Utilities::mkdir(dst_dir);
    Utilities::mkdir(dst_root_);

    string conf_file = current_dir + "/conf/" + argv[1] + "_config.json";
    auto table_specs = ParsingUtilities::loadTableInfoFromJson(conf_file);
    table_to_query_ = table_specs.first;
    table_to_metadata_ = table_specs.second;

    // set up OMPC

    EmpManager *manager = new OutsourcedMpcManager(hosts_.data(), party_, c.port_, c.ctrl_port_, 40);

    conf_.emp_manager_ = manager;
    conf_.setEmptyDbName(empty_db_);
    conf_.buffer_pool_enabled_ = true;
    conf_.bp_page_cnt_ = 50;
    //BitPackingMetadata md = FieldUtilities::getBitPackingMetadata(argv[1]);
    conf_.initialize(db_name_, BitPackingMetadata(), StorageModel::COLUMN_STORE);
    conf_.stored_db_path_ = dst_root_;
    conf_.temp_db_path_ = temp_data_path_;

    // just encode the table
    for(auto const &entry : table_to_query_) {
        encodeTable(entry.first);
    }

    std::this_thread::sleep_for(std::chrono::seconds(1));

    OMPCBackend<N> *protocol = (OMPCBackend<N> *) emp::backend;
    string backend_state_file = dst_root_ + "/backend_state." + to_string(party_);
    protocol->dump_backend_state(backend_state_file);

    // validate it
    if(VALIDATE) {
        conf_.initializeSecretShares(dst_root_, temp_data_path_);
        for (auto table_entry: table_to_query_) {
            string table_name = table_entry.first;
            string query = table_entry.second;
            PlainTable *expected = DataUtilities::getQueryResults(argv[1], query, false);
            SecureTable *recvd = StoredTableScan<Bit>::readStoredTable(table_name, vector<int>());
            PlainTable *recvd_plain = recvd->revealInsecure();

            expected->order_by_ = recvd_plain->order_by_;
            assert(*expected == *recvd_plain);

            delete recvd_plain;
            delete expected;
        }
    }

}
#else
int main(int argc, char **argv) {
    std::cout << "emp-rescu backend not found!" << std::endl;
}

#endif
