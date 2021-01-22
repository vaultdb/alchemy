#ifndef UNION_HYBRID_DATA_H
#define UNION_HYBRID_DATA_H


#include <string>
#include <query_table/query_table.h>
#include <emp-tool/io/net_io_channel.h>

using namespace std;
using namespace vaultdb;
using namespace emp;

class UnionHybridData {
public:
    UnionHybridData(const string & localInputFile, const string & secretSharesFile,
                    NetIO *netio, const int & party);

private:
    int party;
    NetIO *netio;

    Integer readEncrypted(int8_t *secretSharedBits, const size_t & size, // size in bytes
                          const int & dstParty);


};


#endif //UNION_HYBRID_DATA_H
