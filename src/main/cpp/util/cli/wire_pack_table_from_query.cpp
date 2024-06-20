#include "util/data_utilities.h"
#include "query_table/query_table.h"
#include "util/emp_manager/outsourced_mpc_manager.h"
#include "util/field_utilities.h"
#include "query_table/packed_column_table.h"

#if __has_include("emp-rescu/emp-rescu.h")

using namespace std;
using namespace vaultdb;

// customize this as needed
const string empty_db_ = "tpch_empty";
const int port = 55370;
const int ctrl_port = 55380;


int main(int argc, char **argv) {

    // paths are relative to local $VAULTDB_ROOT, the src/main/cpp within your vaultdb directory
    // usage: ./wire_pack_table_from_query <db name> "sql statement" <destination root> <party>
    // e.g., ./wire_pack_table_from_query tpch_unioned_600 "select * from lineitem ORDER BY l_orderkey, l_linenumber"  wires/tpch_unioned_600/lineitem 10086
    if (argc < 5) {
        std::cout << "usage: ./wire_pack_table_from_query <db name> \"sql statement\" <destination root> <party>" << std::endl;
        exit(-1);
    }


    int party = atoi(argv[4]);
    string db_name = (party == emp::TP) ? argv[1] : empty_db_;
    string query = argv[2];
    string dst_root = Utilities::getCurrentWorkingDirectory() + "/" + argv[3];
//    int port = atoi(argv[5]);
//    int ctrl_port = atoi(argv[6]);

    cout << "Party: " << party << endl;
    cout << "DB Name: " << db_name << endl;
    cout << "Query: " << query << endl;
    cout << "Destination Root: " << dst_root << endl;
    cout << "Ports: " << port << ", " << ctrl_port << endl;

    string dst_dir = dst_root.substr(0, dst_root.find_last_of("/"));
    Utilities::mkdir(dst_dir);

    // set up OMPC
    string hosts[] = {"127.0.0.1", "127.0.0.1", "127.0.0.1", "127.0.0.1"};
    // to enable wire packing set storage model to StorageModel::PACKED_COLUMN_STORE
    EmpManager *manager = new OutsourcedMpcManager(hosts, party, port, ctrl_port);
    SystemConfiguration & conf = SystemConfiguration::getInstance();

    conf.emp_manager_ = manager;
    conf.setEmptyDbName(empty_db_);
    BitPackingMetadata md = FieldUtilities::getBitPackingMetadata(argv[1]);
    conf.initialize(db_name, md, StorageModel::PACKED_COLUMN_STORE);

    cout << "Test settings " << Utilities::getTestParameters() << endl;

    // metadata consists of schema and tuple count
    PlainTable *table = DataUtilities::getQueryResults(db_name, query, false);

    // pack the table
    // this still uses BPM under the hood, but it's less onerous than serializing one wire at a time as before
    PackedColumnTable *packed_table = (PackedColumnTable *) table->secretShare();

    auto serialized = packed_table->serialize();

    string secret_shares_file = dst_root + "." + std::to_string(party);
    DataUtilities::writeFile(secret_shares_file, serialized);

    // TP instance responsible for writing metadata
    // everything is localhost so it does not matter which one does this
    if(party == conf.input_party_) {
        string metadata_filename = dst_root + ".metadata";
        string metadata = packed_table->getSchema().prettyPrint() + "\n" + std::to_string(packed_table->tuple_cnt_);
        DataUtilities::writeFile(metadata_filename, metadata);

        OMPCBackend<N> *protocol = (OMPCBackend<N> *) emp::backend;
        string delta_file = dst_root + ".delta";
        string delta;
        delta.resize(sizeof(block));
        memcpy(delta.data(), &protocol->multi_pack_delta, sizeof(block));
        DataUtilities::writeFile(delta_file, delta);
    }

    delete table;
    delete packed_table;

    cout << "Wrote secret shares to " << secret_shares_file << endl;
}

#endif
