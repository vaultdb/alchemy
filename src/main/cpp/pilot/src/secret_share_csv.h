#ifndef  _SECRET_SHARE_CSV_H
#define  _SECRET_SHARE_CSV_H

#include <iostream>
#include <query_table/query_schema.h>


// output the data for testing at NM with the following cmd in psql:
// \copy (SELECT * FROM patient WHERE study_year=2020) TO 'charlie.csv' CSV HEADER;
// alice = 2018, bob = 2019, charlie = 2020
using namespace std;
using namespace vaultdb;

class SecretShareCsv {
public:

    static void SecretShareTable(const string & srcCsvFile, const QuerySchema & srcSchema, const string & dstRoot);


    static void writeFile(const string &  fileName, const std::vector<int8_t> & contents);
};





#endif //_SECRET_SHARE_CSV_H
