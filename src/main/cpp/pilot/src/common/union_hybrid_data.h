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

        static SecureTable * readSecretSharedInput(const string &secret_shares_input, const QuerySchema &plain_schema);

        SecureTable *getInputTable();

        static SecureTable *unionHybridData(const QuerySchema & schema, const std::string & localInputFile,  const std::string & secretSharesFile);
        static SecureTable *
        unionHybridData(const string &db_name, const string &input_query, const string &secret_shares_file);

        ~UnionHybridData() {
            if(input_table_) {
                delete input_table_;
            }
        }
    private:

        SecureTable *input_table_;

        Integer readEncrypted(int8_t *secret_shared_bits, const size_t & size_bytes,
                              const int & dst_party);


        // add on to input_table_
        void resizeAndAppend(SecureTable *to_add);

        string printFirstBytes(vector<int8_t> &bytes, const int &byteCount);
    };

}

#endif //UNION_HYBRID_DATA_H
