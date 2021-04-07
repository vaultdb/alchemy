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
        UnionHybridData(const QuerySchema & srcSchema, NetIO *netio, const int & party);
        static shared_ptr<SecureTable>
        readLocalInput(const string &localInputFile, const QuerySchema &src_schema, NetIO *netio,
                       const int &party);

        static shared_ptr<SecureTable>
        readSecretSharedInput(const string &secretSharesFile, const QuerySchema &plain_schema,
                              const int &party);

        std::shared_ptr<SecureTable> getInputTable();

        static shared_ptr<SecureTable> unionHybridData(const QuerySchema & schema, const std::string & localInputFile,  const std::string & secretSharesFile, NetIO *aNetIO, const int & party);

    private:
        int party;
        NetIO *netio;
        bool inputTableInit = false;

        std::shared_ptr<SecureTable> inputTable;

        Integer readEncrypted(int8_t *secretSharedBits, const size_t & sizeBytes, // size in bytes
                              const int & dstParty);

        // add on to inputTable
        void resizeAndAppend(std::shared_ptr<SecureTable> toAdd);

        string printFirstBytes(vector<int8_t> &bytes, const int &byteCount);
    };

}

#endif //UNION_HYBRID_DATA_H
