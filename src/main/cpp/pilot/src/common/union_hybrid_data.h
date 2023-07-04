#ifndef UNION_HYBRID_DATA_H
#define UNION_HYBRID_DATA_H


#include <string>
#include <query_table/query_table.h>
#include <emp-tool/io/net_io_channel.h>

using namespace std;
using namespace vaultdb;
using namespace emp;

namespace vaultdb {
    class UnionHybridData {
    public:
        UnionHybridData(const QuerySchema & src_schema);

        static SecureTable *
        readLocalInput(const string &local_input_file, const QuerySchema &src_schema);

        static SecureTable *
        readSecretSharedInput(const string &secret_shares_file, const QuerySchema &plain_schema);

        SecureTable *getInputTable();

        static SecureTable *unionHybridData(const QuerySchema & schema, const std::string & localInputFile,  const std::string & secretSharesFile);
        static SecureTable *
        unionHybridData(const string &dbName, const string &inputQuery, const string &secretSharesFile);

        ~UnionHybridData() {
            if(input_table_) {
                delete input_table_;
            }
        }
    private:
//        int party;
//        NetIO *netio;
//        bool inputTableInit = false;

        SecureTable *input_table_;

        Integer readEncrypted(int8_t *secret_shared_bits, const size_t & size_bytes,
                              const int & dst_party);

        // convert all instances of bool and dummy tag to be 1 bool instead of 8 from previous serialization
        static void
        plain_to_secure_bits(bool *src, bool *dst, const QuerySchema &plain_schema,
                             const QuerySchema &secure_schema, const size_t &tuple_cnt);

        // add on to input_table_
        void resizeAndAppend(SecureTable *to_add);

        string printFirstBytes(vector<int8_t> &bytes, const int &byteCount);
    };

}

#endif //UNION_HYBRID_DATA_H
