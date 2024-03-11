#include "catalyst.h"
#include "util/emp_manager/sh2pc_manager.h"
#include "util/system_configuration.h"
#include "common/union_hybrid_data.h"

using namespace catalyst;

Catalyst::Catalyst(int party, const std::string json_config_filename)  {
    StudyParser study_parser(json_config_filename);
    study_ = study_parser.study_;
    assert(study_parser.protocol_ == "sh2pc");
    emp_manager_ = new SH2PCManager(study_parser.alice_host_, party, study_parser.port_);
    SystemConfiguration & s = SystemConfiguration::getInstance();
    s.emp_mode_ = EmpMode::SH2PC;
    s.emp_manager_ = emp_manager_;
    BitPackingMetadata md; // empty set
    s.initialize(study_.db_name_, md, StorageModel::COLUMN_STORE);

    // load secret shares into TableManager
    for(auto &input_table : study_.input_tables_) {
        for(auto &contributor : input_table.data_contributors_) {
                importSecretShares(input_table.name_, contributor);
        }
    }

    cout << "Finished loading secret shares!" << endl;

}


void Catalyst::importSecretShares(const string & table_name, const int & src_party) {
    // read secret shares from file
    // import into table_manager
    string fq_table_file = study_.secret_shares_root_ + "/" + std::to_string(src_party) + "/" + table_name;
    SystemConfiguration & s = SystemConfiguration::getInstance();
    string suffix = (s.party_ == 1) ? "alice" : "bob";
    string secret_shares_file = fq_table_file + "." + suffix;
    QuerySchema schema = QuerySchema::toPlain(table_manager_.getSchema(table_name));
    SecureTable *secret_shares = UnionHybridData::readSecretSharedInput(secret_shares_file, schema);
    table_manager_.insertTable(table_name, secret_shares);
}

int main(int argc, char **argv) {
    // usage: ./bin/catalyst <json config file>
    if(argc < 3) {
        std::cout << "usage: ./bin/catalyst <party> <json config file>" << std::endl;
        exit(-1);
    }

    // Alice = 1, Bob = 2
    int party = atoi(argv[1]);
    assert(party == 1 || party == 2);

    Catalyst catalyst(party, argv[2]);

    // parse study
    // stand up TableManager
    // load tables according to study params
    // run queries
    // write out results to secret share files


}