#include <util/data_utilities.h>
#include "util/emp_manager/sh2pc_manager.h"
#include "query_table/column_table.h"

using namespace std;
using namespace emp;
using namespace vaultdb;




// usage: ./repair_phame_rollup <party 1 || 2> <host> <port>  <secret shares file> <optional max row count>
// e.g.,
// ./bin/repair_phame_rollup 1 127.0.0.1 4444 pilot/secret_shares/output/1/PHAME_ROLLUP.alice <max row count>
// ./bin/repair_phame_rollup 2 127.0.0.1 4444 pilot/secret_shares/output/1/PHAME_ROLLUP.bob <max row count>

// phame_rollup schema: (age_cat:varchar(1), gender:varchar(1), race:varchar(1), ethnicity:varchar(1), zip:varchar(5), payer_primary:varchar(1), payer_secondary:varchar(1), patient_cnt:int64, diabetes_cnt:int64, hypertension_cnt:int64, cervical_cancer_cnt:int64, breast_cancer_cnt:int64, lung_cancer_cnt:int64, colorectal_cancer_cnt:int64)
// expect that none of the "condition counts (e.g., diabetes_cnt)" are greater than the patient count
// if they are > patient count, then the data is corrupted and we zero out these cells
// a given patient may contribute to > 1 of these counts, so we can't add them up to see if they are <= patient_cnt
int main(int argc, char **argv) {
    int port, party;
    if(argc < 4) {
        cout << "Usage: ./repair_phame_rollup <party 1 || 2> <host> <port> <secret shares file>\n";
        return 1;
    }

    party = atoi(argv[1]);
    string host = argv[2];
    port = atoi(argv[3]);
    string secret_shares_file(argv[4]);
    int limit = -1;
    if(argc > 5) {
        limit = atoi(argv[5]);
    }

    cout << "Setup: party=" << party << " host=" << host << " port=" << port << " input_shares=" << secret_shares_file << '\n';
    auto emp_manager = new SH2PCManager(host, party, port);
    SystemConfiguration & s = SystemConfiguration::getInstance();
    s.emp_mode_ = EmpMode::SH2PC;
    s.emp_manager_ = emp_manager;
    BitPackingMetadata md; // empty set
    s.initialize("", md, StorageModel::COLUMN_STORE);


    // get schema:
    int suffix_start = secret_shares_file.find_last_of(".");
    string schema_file = secret_shares_file.substr(0, suffix_start) + ".schema";
    string dst_file = secret_shares_file.substr(0, suffix_start) + "_repaired."  + (party == 1 ? "alice" : "bob");
    auto schema_spec = DataUtilities::readTextFileToString(schema_file);
    auto schema = QuerySchema(schema_spec);
    auto plain_schema = QuerySchema::toPlain(schema);

    auto secret_shared = DataUtilities::readSecretSharedInput(secret_shares_file, plain_schema, limit);

    SecureField zero(FieldType::SECURE_LONG, Integer(64, 0));
    for(int i = 0; i < secret_shared->tuple_cnt_; ++i) {
         auto patient_cnt = secret_shared->getField(i, 7);
         for(int j = 8; j < 14; ++j) {
             auto condition_cnt = secret_shared->getField(i, j);
             condition_cnt = Field<Bit>::If(condition_cnt > patient_cnt, zero, condition_cnt);
             secret_shared->setField(i, j, condition_cnt);
         }
    }

    cout << "writing revealed data to " << dst_file << '\n';
    std::vector<int8_t> results = secret_shared->reveal(emp::XOR)->serialize();
    DataUtilities::writeFile(dst_file, results);


    emp_manager->flush();
    delete emp_manager;
    s.emp_manager_ = nullptr;

}
