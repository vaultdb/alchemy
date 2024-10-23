#include <pilot/src/common/shared_schema.h>
#include <data/csv_reader.h>
#include "union_hybrid_data.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <util/data_utilities.h>
#include <query_table/plain_tuple.h>
#include <query_table/secure_tuple.h>
#include <operators/union.h>
#include <operators/sort.h>
#include <query_table/query_table.h>

UnionHybridData::UnionHybridData(const QuerySchema & src_schema)  {
    // placeholder, retains schema
    input_table_ =  SecureTable::getTable(0, src_schema);

}

// based on EmpManager::encryptedVarchar
Integer UnionHybridData::readEncrypted(int8_t *secret_shared_bits, const size_t &size_bytes, const int & dst_party) {


        bool *bools = Utilities::bytesToBool(secret_shared_bits, size_bytes);

        Integer result(size_bytes * 8, 0L, dst_party);

        int party = SystemConfiguration::getInstance().party_;
        EmpManager *manager = SystemConfiguration::getInstance().emp_manager_;
        if(party == dst_party) {
            //         virtual void feed(Bit *labels, int party, const bool *b, int bit_cnt) = 0;
            manager->feed(result.bits.data(), dst_party, bools, size_bytes * 8);
        }
        else {
            manager->feed(result.bits.data(), dst_party, nullptr, size_bytes * 8);
        }


        delete [] bools;

        return result;

    }

SecureTable *
UnionHybridData::readLocalInput(const string &localInputFile, const QuerySchema &src_schema) {
    PlainTable *local = CsvReader::readCsv(localInputFile, src_schema);
    SecureTable *secret_shared = local->secretShare();
    delete local;

    return secret_shared;

}





SecureTable *UnionHybridData::getInputTable() {
    return input_table_;
}

void UnionHybridData::resizeAndAppend(SecureTable *to_add) {
    size_t old_tuple_cnt = input_table_->tuple_cnt_;
    size_t new_tuple_cnt = old_tuple_cnt + to_add->tuple_cnt_;
    input_table_->resize(new_tuple_cnt);

    int write_idx = old_tuple_cnt;

    for(size_t i = 0; i < to_add->tuple_cnt_; ++i) {
        input_table_->cloneRow(write_idx, 0, to_add, i);
        ++write_idx;
    }

}

SecureTable *UnionHybridData::unionHybridData(const QuerySchema &schema, const string &localInputFile,
                                                        const string &secretSharesFile) {

    SecureTable *local = UnionHybridData::readLocalInput(localInputFile, schema);

    if(!secretSharesFile.empty()) {
     SecureTable *remote = DataUtilities::readSecretSharedInput(secretSharesFile, schema);
      Union<emp::Bit> union_op(local, remote);
      return union_op.run();
    }

    return local;

}


SecureTable *
UnionHybridData::unionHybridData(const string &db_name, const string &input_query, const string &secret_shares_file) {


    PlainTable *local_plain = DataUtilities::getQueryResults(db_name, input_query, false);
    size_t total_tuples = local_plain->tuple_cnt_;

    cout << "Reading in " << local_plain->tuple_cnt_ << " tuples from local db." << endl;

    SecureTable *local = local_plain->secretShare();
    int party = SystemConfiguration::getInstance().party_;

    string other_party = (party == ALICE) ? "Bob" : "Alice";
    cout <<  other_party << " read in " << local->tuple_cnt_ - local_plain->tuple_cnt_ << " tuples." << endl;

    total_tuples += local->tuple_cnt_ - local_plain->tuple_cnt_;

    QuerySchema local_plain_schema = local_plain->getSchema();
    delete local_plain;

    if(!secret_shares_file.empty()) {
        SecureTable *remote = DataUtilities::readSecretSharedInput(secret_shares_file, local_plain_schema);
        cout << "Reading in " << remote->tuple_cnt_ << " secret-shared records from " << secret_shares_file << endl;
        total_tuples += remote->tuple_cnt_;
        Union<emp::Bit> union_op(local, remote);
        cout << "Total tuples read: " << total_tuples << endl;
        // deep copy so it isn't deleted with union_op
        SecureTable *res = union_op.run()->clone();
        return res;
    }
    
    cout << "Total tuples read: " << total_tuples << endl;
    return local;


}

