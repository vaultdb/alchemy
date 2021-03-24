#ifndef  _SECRET_SHARE_CSV_H
#define  _SECRET_SHARE_CSV_H

#include <iostream>
#include <query_table/query_schema.h>


using namespace std;
using namespace vaultdb;

class SecretShareCsv {
public:

    static void SecretShareTable(const string & srcCsvFile, const QuerySchema & srcSchema, const string & dstRoot);


    static void writeFile(const string &  fileName, const std::vector<int8_t> & contents);
};





#endif //_SECRET_SHARE_CSV_H
