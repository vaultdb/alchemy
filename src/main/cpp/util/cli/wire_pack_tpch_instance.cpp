#include "util/data_utilities.h"
#include "query_table/query_table.h"
#include "util/emp_manager/outsourced_mpc_manager.h"
#include "util/field_utilities.h"
#include "query_table/packed_column_table.h"
#include "util/table_manager.h"

#if __has_include("emp-rescu/emp-rescu.h")

#define VALIDATE 0
using namespace std;
using namespace vaultdb;
// customize this as needed
const string empty_db_ = "tpch_empty";
const int port_ = 55370;
const int ctrl_port_ = 55380;
string dst_root_;
string db_name_;
int party_ = -1;
SystemConfiguration & conf_ = SystemConfiguration::getInstance();

// encode whole db at once to preserve same delta for all of them
map<string, string> table_to_query = {
        {"lineitem", "SELECT l_orderkey, l_orderkey, l_partkey, l_suppkey, l_linenumber, l_quantity, l_extendedprice, l_discount, l_tax, l_returnflag, l_linestatus, l_shipdate, l_commitdate, l_receiptdate, l_shipinstruct, l_shipmode, l_comment FROM lineitem ORDER BY l_orderkey, l_linenumber"},
        {"orders", "SELECT o_orderkey, o_orderkey, o_custkey, o_orderstatus, o_totalprice, o_orderdate, o_orderpriority, o_clerk, o_shippriority, o_comment, o_orderyear FROM orders ORDER BY o_orderkey"},
        {"customer", "SELECT c_custkey, c_name, c_address, c_nationkey, c_phone, c_acctbal, c_mktsegment, c_comment FROM customer ORDER BY c_custkey"},
        {"part", "SELECT p_partkey, p_name, p_mfgr, p_brand, p_type, p_size, p_container, p_retailprice, p_comment FROM part ORDER BY p_partkey"},
        {"partsupp", "SELECT ps_partkey, ps_suppkey, ps_availqty, ps_supplycost, ps_comment FROM partsupp ORDER BY ps_partkey, ps_suppkey"},
        {"supplier", "SELECT  s_suppkey, s_name, s_address, s_nationkey, s_phone, s_acctbal, s_comment FROM supplier ORDER BY s_suppkey"},
        {"nation", "SELECT n_nationkey, n_name, n_regionkey, n_comment FROM nation ORDER BY n_nationkey"},
        {"region", "SELECT r_regionkey, r_name, r_comment  FROM region ORDER BY r_regionkey"}
};

// initialized below
map<string, string> table_to_collation_;

void initializeCollations() {
    table_to_collation_["lineitem"] = DataUtilities::printSortDefinition({ColumnSort(0, SortDirection::ASCENDING), ColumnSort(4, SortDirection::ASCENDING)}),
        table_to_collation_["orders"] = DataUtilities::printSortDefinition({ColumnSort(0, SortDirection::ASCENDING)});
        table_to_collation_["customer"] = DataUtilities::printSortDefinition({ColumnSort(0, SortDirection::ASCENDING)});
        table_to_collation_["part"]= DataUtilities::printSortDefinition({ColumnSort(0, SortDirection::ASCENDING)});
        table_to_collation_["partsupp"] = DataUtilities::printSortDefinition({ColumnSort(0, SortDirection::ASCENDING), ColumnSort(1, SortDirection::ASCENDING)});
        table_to_collation_["supplier"] = DataUtilities::printSortDefinition({ColumnSort(0, SortDirection::ASCENDING)});
        table_to_collation_["nation"] = DataUtilities::printSortDefinition({ColumnSort(0, SortDirection::ASCENDING)});
        table_to_collation_["region"] = DataUtilities::printSortDefinition({ColumnSort(0, SortDirection::ASCENDING)});
}

void encodeTable(string table_name) {
    // metadata consists of schema and tuple count
    PlainTable *table = DataUtilities::getQueryResults(db_name_, table_to_query.at(table_name), false);


    // pack the table
    // this still uses BPM under the hood, but it's less onerous than serializing one wire at a time as before
    PackedColumnTable *packed_table = (PackedColumnTable *) table->secretShare();
    auto serialized = packed_table->serialize();




    if(party_ == conf_.input_party_) {
        // metadata file schema, collation, and tuple count
        string metadata_filename = dst_root_ + "/" + table_name + ".metadata";
        string metadata = packed_table->getSchema().prettyPrint() + "\n"
                + table_to_collation_.at(table_name) + "\n"
               + std::to_string(packed_table->tuple_cnt_);
        DataUtilities::writeFile(metadata_filename, metadata);
        cout << "Wrote metadata for " << table_name << " to " << metadata_filename << '\n';
    }
    else {    // write out shares
        string secret_shares_file = dst_root_ + "/" + table_name + "." + std::to_string(party_);
        DataUtilities::writeFile(secret_shares_file, serialized);
        cout << "Wrote secret shares to " << secret_shares_file << endl;

    }
    delete table;
    delete packed_table;
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
     dst_root_ = Utilities::getCurrentWorkingDirectory() + "/" + argv[2];

    cout << "Party: " << party_ << endl;
    cout << "DB Name: " << db_name_ << endl;
    cout << "Destination dir: " << dst_root_ << endl;
    cout << "Ports: " << port_ << ", " << ctrl_port_ << endl;

    // attempt to create target dir
    string dst_dir = dst_root_.substr(0, dst_root_.find_last_of("/"));
    Utilities::mkdir(dst_dir);

    initializeCollations();

    // set up OMPC
    string hosts[] = {"127.0.0.1", "127.0.0.1", "127.0.0.1", "127.0.0.1"};
    // to enable wire packing set storage model to StorageModel::PACKED_COLUMN_STORE
    EmpManager *manager = new OutsourcedMpcManager(hosts, party_, port_, ctrl_port_);
    SystemConfiguration & conf = SystemConfiguration::getInstance();

    conf.emp_manager_ = manager;
    conf.setEmptyDbName(empty_db_);
    BitPackingMetadata md = FieldUtilities::getBitPackingMetadata(argv[1]);
    conf.initialize(db_name_, md, StorageModel::PACKED_COLUMN_STORE);


    for(auto const &entry : table_to_query) {
        encodeTable(entry.first);
    }


    // TP instance responsible for writing metadata
    // everything is localhost so it does not matter which one does this
    if(party_ == conf.input_party_) {

        OMPCBackend<N> *protocol = (OMPCBackend<N> *) emp::backend;
        string delta_file = dst_root_ + "/" + "delta";
        vector<int8_t> delta;
        delta.resize(sizeof(block));
        memcpy(delta.data(), &protocol->multi_pack_delta, sizeof(block));
        DataUtilities::writeFile(delta_file, delta);
    }

    // validate it
    if(VALIDATE) {
        TableManager::getInstance().initializePackedWires(dst_root_, party_);
        for (auto table_entry: table_to_query) {
            string table_name = table_entry.first;
            string query = table_entry.second;
            PlainTable *expected = DataUtilities::getQueryResults(argv[1], query, false);
            SecureTable *recvd = TableManager::getInstance().getSecureTable(table_name);
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
