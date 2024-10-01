#include "util/data_utilities.h"
#include "query_table/query_table.h"
#include "util/emp_manager/outsourced_mpc_manager.h"
#include "util/field_utilities.h"
#include "query_table/packed_column_table.h"
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
int party_ = -1;
SystemConfiguration & conf_ = SystemConfiguration::getInstance();

// encode whole db at once to preserve same delta for all of them
map<string, string> table_to_query_;
map<string, TableMetadata> table_to_metadata_;


void encodeTable(string table_name) {
    // metadata consists of schema and tuple count
    PlainTable *table = DataUtilities::getQueryResults(db_name_, table_to_query_.at(table_name), false);
    // all parties need packed table (even TP) to complete protocol, same for serialize because it flushes table from BP
    PackedColumnTable *packed_table = (PackedColumnTable *) table->secretShare();
    // this still uses BPM under the hood, but it's less onerous than serializing one wire at a time as before
    auto serialized = packed_table->serialize();

    if(conf_.inputParty()) {
        // metadata file schema, collation, and tuple count
        string metadata_filename = dst_root_ + "/" + table_name + ".metadata";
        auto md = table_to_metadata_.at(table_name);
        string metadata = md.schema_.prettyPrint() + "\n"
                + DataUtilities::printSortDefinition(md.collation_) + "\n"
               + std::to_string(md.tuple_cnt_);
        DataUtilities::writeFile(metadata_filename, metadata);
        cout << "Wrote metadata for " << table_name << " to " << metadata_filename << '\n';
    }
    else {
        // write out shares
        string secret_shares_file = Utilities::getFilenameForTable(table_name);
        DataUtilities::writeFile(secret_shares_file, serialized);
        cout << "Wrote secret shares to " << secret_shares_file << endl;
    }

    delete packed_table;
    delete table;
}

int main(int argc, char **argv) {

    // paths are relative to local $VAULTDB_ROOT, the src/main/cpp within your vaultdb directory
    // usage: ./wire_pack_table_from_query <db name> <destination root> <party>
    // e.g., ./wire_pack_table_from_query tpch_unioned_600 wires/tpch_unioned_600 10086
    if (argc < 4) {
        std::cout << "usage: ./wire_pack_tpch_instance <db name> <destination root> <party>" << std::endl;
        exit(-1);
    }


     party_ = atoi(argv[3]);
     db_name_ = (party_ == emp::TP) ? argv[1] : empty_db_;
     string current_dir = Utilities::getCurrentWorkingDirectory();
     dst_root_ = current_dir + "/" + argv[2];

    cout << "Party: " << party_ << endl;
    cout << "DB Name: " << db_name_ << endl;
    cout << "Destination dir: " << dst_root_ << endl;

    // attempt to create target dir
    string dst_dir = dst_root_.substr(0, dst_root_.find_last_of("/"));
    Utilities::mkdir(dst_dir);
    Utilities::mkdir(dst_root_);

    auto table_specs = ParsingUtilities::loadTableInfoFromJson(current_dir + "/conf/table_config.json");
    table_to_query_ = table_specs.first;
    table_to_metadata_ = table_specs.second;


    // set up OMPC
    // to enable wire packing set storage model to StorageModel::PACKED_COLUMN_STORE
    ConnectionInfo c = ParsingUtilities::parseIPsFromJson(current_dir + "/conf/config.json");
    EmpManager *manager = new OutsourcedMpcManager(hosts_.data(), party_, c.port_, c.ctrl_port_);
    cout << "Ports: " << c.port_ << ", " << c.ctrl_port_ << endl;

    conf_.emp_manager_ = manager;
    conf_.setEmptyDbName(empty_db_);
//    BitPackingMetadata md = FieldUtilities::getBitPackingMetadata(argv[1]);
    conf_.initialize(db_name_, BitPackingMetadata(), StorageModel::PACKED_COLUMN_STORE);
    conf_.stored_db_path_ = dst_root_;

    // just encode the first table
    for(auto const &entry : table_to_query_) {
        encodeTable(entry.first);
    }

    OMPCBackend<N> *protocol = (OMPCBackend<N> *) emp::backend;
    string backend_state_file = dst_root_ + "/backend_state." + to_string(party_);
    protocol->dump_backend_state(backend_state_file);

    // validate it
    if(VALIDATE) {
        conf_.initializeWirePackedDb(dst_root_);
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
