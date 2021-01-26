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
        void readLocalInput(const string & localInputFile);
        void readSecretSharedInput(const string & secretSharesFile);
        std::shared_ptr<QueryTable> getInputTable();

        static shared_ptr<QueryTable> unionHybridData(const QuerySchema & schema, const std::string & localInputFile,  const std::string & secretSharesFile, NetIO *aNetIO, const int & party);

    private:
        int party;
        NetIO *netio;
        bool inputTableInit = false;

        std::shared_ptr<QueryTable> inputTable;

        Integer readEncrypted(int8_t *secretSharedBits, const size_t & sizeBytes, // size in bytes
                              const int & dstParty);

        // add on to inputTable
        void resizeAndAppend(std::shared_ptr<QueryTable> toAdd);

        string printFirstBytes(vector<int8_t> &bytes, const int &byteCount);
    };

}

#endif //UNION_HYBRID_DATA_H
