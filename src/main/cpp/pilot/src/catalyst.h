#ifndef _CATALYST_
#define _CATALYST_

#include <string>
#include "common/study_parser.h"
#include "util/table_manager.h"

using namespace std;
using namespace vaultdb;


namespace catalyst {
    class Catalyst {
    public:
        Catalyst(int party, const std::string json_config_filename);

        void loadStudyData();
        void parseAndRunQueries();

        // redact in place
        static void redactCellCounts(SecureTable *table, const int & col, const int & min_cnt) {
            int col_width = table->getSchema().getField(col).size();
            FieldType col_type = table->getSchema().getField(col).getType();

            emp::Integer cutoff(col_width, min_cnt);
            Integer null(col_width, -1);

            // null out the ones with cell count below threshold
            for(int i = 0; i < table->tuple_cnt_; ++i) {
                Integer v = table->getField(i, col).getInt();
                v = emp::If(v < cutoff, null, v);
               table->setField(i, col, Field<Bit>(col_type, v));
            }

        }
        CatalystStudy<Bit> getStudy() const { return study_; }

    private:
        bool data_loaded_ = false;
        CatalystStudy<Bit> study_;

        void importSecretShares(const string & table_name, const int & src_party);

    };
}
#endif
