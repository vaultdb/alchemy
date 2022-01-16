#ifndef  _SECRET_SHARE_QUERY_ANSWER_H
#define  _SECRET_SHARE_QUERY_ANSWER_H
#include <iostream>
#include <query_table/query_schema.h>


// output the data for testing at NM with the following cmd in psql:
// \copy (SELECT * FROM patient WHERE study_year=2020) TO 'charlie.csv' CSV HEADER;
// alice = 2018, bob = 2019, charlie = 2020
using namespace std;
using namespace vaultdb;

class SecretShareQueryAnswer {
public:

    static void SecretShareTable(const string & src_query, const string & db_name, const string & dst_root);


    static void writeFile(const string &  filename, const std::vector<int8_t> & contents);
};





#endif // _SECRET_SHARE_QUERY_ANSWER_H
