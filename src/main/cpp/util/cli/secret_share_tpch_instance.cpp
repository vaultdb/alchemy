#include "util/data_utilities.h"
#include "query_table/query_table.h"
#include "util/emp_manager/outsourced_mpc_manager.h"
#include "util/field_utilities.h"
#include "query_table/column_table.h"
#include "util/table_manager.h"
#include "operators/stored_table_scan.h"

#include <boost/property_tree/json_parser.hpp>
#include "util/parsing_utilities.h"

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
    PlainTable *plain_table = DataUtilities::getQueryResults(db_name_, table_to_query_.at(table_name), false);

    auto md = table_to_metadata_.at(table_name);
    // metadata file schema, collation, and tuple count
    if(conf_.inputParty()) {
        string metadata_filename = dst_root_ + "/" + table_name + ".metadata";
        plain_table->order_by_ = md.collation_;
        string metadata = md.schema_.prettyPrint() + "\n"
                          + DataUtilities::printSortDefinition(md.collation_) + "\n"
                          + std::to_string(md.tuple_cnt_);
        DataUtilities::writeFile(metadata_filename, metadata);
        cout << "Wrote metadata for " << table_name << " to " << metadata_filename << '\n';
    }

    // Store secret shares (emp::Bits) to disk
    SecureTable *secure_table = plain_table->secretShare();
    QuerySchema secure_schema = secure_table->getSchema();

    std::string secret_shares_file = dst_root_ + "/" + table_name + "." + std::to_string(party_);

    // Serialize SecureTable
    vector<int8_t> serialized = secure_table->serialize();

    int dst_bit_cnt = secure_schema.size() * md.tuple_cnt_;

    // Memcpy column data into Integer (array of emp::Bits)
    Integer dst_int(dst_bit_cnt, 0L, emp::PUBLIC);
    memcpy(dst_int.bits.data(), serialized.data(), dst_bit_cnt * empBitSizesInPhysicalBytes::bit_ram_size_);

    vector<int8_t> write_buffer(dst_bit_cnt * (party_ == 1 ? empBitSizesInPhysicalBytes::evaluator_disk_size_ : (party_ == 10086 ? 1 : empBitSizesInPhysicalBytes::garbler_disk_size_)), 0);
    int8_t *write_cursor = write_buffer.data();

    // Serialize bit by bit to write buffer
    for(int i = 0; i < dst_bit_cnt; ++i) {
        emp::Bit cur_bit = dst_int.bits[i];

        // only non-tp need macs and keys in the auth shares
        if(party_ != 10086) {
            memcpy(write_cursor, (int8_t *) &cur_bit.bit.auth.mac, emp::N * sizeof(emp::block));
            write_cursor += emp::N * sizeof(emp::block);

            memcpy(write_cursor, (int8_t *) &cur_bit.bit.auth.key, emp::N * sizeof(emp::block));
            write_cursor += emp::N * sizeof(emp::block);
        }

        // TP only needs lambdas in auth shares (those lambdas are generated and distributed by TP)
        memcpy(write_cursor, (int8_t *) &cur_bit.bit.auth.lambda, 1);
        ++write_cursor;

        // write maske value for evaluator
        if (party_ == 1) {
            memcpy(write_cursor, (int8_t *) &cur_bit.bit.masked_value, 1);
            ++write_cursor;
        }
    }

    // write to disk
    DataUtilities::writeFile(secret_shares_file,write_buffer);
    cout << "Wrote secret shares for " << table_name << " to " << secret_shares_file << '\n';

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
    ConnectionInfo c = ParsingUtilities::parseIPsFromJson(current_dir + "/conf/config.json");

    cout << "Party: " << party_ << endl;
    cout << "DB Name: " << db_name_ << endl;
    cout << "Destination dir: " << dst_root_ << endl;
    cout << "Ports: " << c.port_ << ", " << c.ctrl_port_ << endl;

    // attempt to create target dir
    string dst_dir = dst_root_.substr(0, dst_root_.find_last_of("/"));
    Utilities::mkdir(dst_dir);
    Utilities::mkdir(dst_root_);

    auto table_specs = ParsingUtilities::loadTableInfoFromJson(current_dir + "/conf/table_config.json");
    table_to_query_ = table_specs.first;
    table_to_metadata_ = table_specs.second;

    // set up OMPC

    EmpManager *manager = new OutsourcedMpcManager(hosts_.data(), party_, c.port_, c.ctrl_port_);

    conf_.emp_manager_ = manager;
    conf_.setEmptyDbName(empty_db_);
    //BitPackingMetadata md = FieldUtilities::getBitPackingMetadata(argv[1]);
    conf_.initialize(db_name_, BitPackingMetadata(), StorageModel::COLUMN_STORE);
    conf_.stored_db_path_ = dst_root_;

    // just encode the table
    for(auto const &entry : table_to_query_) {
        encodeTable(entry.first);
    }

    OMPCBackend<N> *protocol = (OMPCBackend<N> *) emp::backend;
    string backend_state_file = dst_root_ + "/backend_state." + to_string(party_);
    protocol->dump_backend_state(backend_state_file);

    // validate it
    if(VALIDATE) {
        conf_.initializeOutsourcedSecretShareDb(dst_root_);
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
