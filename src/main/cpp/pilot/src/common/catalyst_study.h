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

    typedef struct query_spec {
        string name_;
        string plan_file_; // fully-qualified filename for JSON query plan
        vector<int> count_cols_; // columns with counts for redaction
    } QuerySpec;

// simple wrapper for a study like PHAME.
    template<typename B>
    class CatalystStudy {
    public:
        CatalystStudy() = default;


        string study_name_;
        int min_cell_cnt_ = -1; // minimum cell count for redaction
        map<string, QuerySpec> queries_;
        vector<InputTable> input_tables_;
        // all paths  relative to $VAULTDB_HOME/src/main/cpp
        string secret_shares_path_; // root directory for secret shares,
        string dst_path_; // where to write secret shares of query output

        string toString() const {
            stringstream ss;

            ss << "Study: " << study_name_ << endl;
            ss << "Min cell count: " << min_cell_cnt_ << endl;
            ss << "Secret shares path: " << secret_shares_path_ << endl;
            ss << "Destination path: " << dst_path_ << endl;
            ss << "Input tables: " << endl;
            for(auto t : input_tables_) {
                ss << "Table: " << t.name_ << endl;
                ss << "     Schema: " << t.schema_ << endl;
                ss << "     Data contributors: ";
                for(auto c : t.data_contributors_) {
                    ss << c << " ";
                }
                ss << endl;
            }

            ss << "Queries: " << endl;
            for(auto q : queries_) {
                ss << "Query: " << q.first << endl;
                ss << "     Plan file: " << q.second.plan_file_ << endl;
                ss << "     Count columns: ";
                for(auto c : q.second.count_cols_) {
                    ss << c << " ";
                }
                ss << endl;
            }
            return ss.str();
        }
    };
}
#endif