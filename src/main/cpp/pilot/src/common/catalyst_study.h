#ifndef _CATALYST_STUDY_
#define _CATALYST_STUDY_

#include "operators/operator.h"

using namespace vaultdb;

namespace catalyst {
    typedef struct input_table {
        string name_;
        QuerySchema schema_;
        vector<int> data_contributors_;
    } InputTable;


// simple wrapper for a study like PHAME.
    template<typename B>
    class CatalystStudy {
    public:
        CatalystStudy() = default;


        string study_name_;
        // query name, root operator in tree
        map<string, Operator<B> *> queries_;
        vector<InputTable> input_tables_;
        string db_name_; // psql DB name
        // all paths  relative to $VAULTDB_HOME/src/main/cpp
        string secret_shares_root_; // root directory for secret shares,
        string query_path_; // where JSON files for queries are stored
        string dst_path_; // where to write secret shares of query output
    };
}
#endif