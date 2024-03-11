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
        map<int, Operator<B> *> queries_;
        vector<InputTable> input_tables_;

    };
}
#endif