#ifndef PILOT_UTILITIES_H
#define PILOT_UTILITIES_H

#include <string>
#include "query_table/query_table.h"


namespace  vaultdb {
    class PilotUtilities {
    public:
        static std::string getRollupExpectedResultsSql(const std::string &groupByColName, const std::string & selection);
        static SecureTable *rollUpAggregate( SecureTable *input, const int &ordinal);
        static void validateInputTable(const std::string & dbName, const std::string & sql, const SortDefinition  & expectedSortDefinition,  PlainTable *testTable);
        static void secretShareFromCsv(const string &src_file, const QuerySchema &plain_schema, const string &dst_root);
        static void secretShareFromQuery(const std::string & db_name, const std::string & query, const std::string & dst_root);
        static std::string appendToConjunctivePredicate(const std::string & base, const std::string & to_append);
        static std::string replaceSubstring(const std::string & base, const std::string & to_find, const std::string replacement);
        static std::string replaceSelection(const std::string & base, const std::string replacement);
        static std::pair<std::string, std::string>  parseRange(const std::string & range);
        static std::string getRangePredicate(const std::string & range);
        static std::string parseYearSelection(const std::string & input_year);
        // study length in years
        static int getStudyLength(const std::string study_year);

        static const std::string data_cube_sql_,  data_cube_sql_no_dummies_, unioned_db_name_;
        static void redactCellCounts(SecureTable *input, const int & min_cell_cnt);
        static void setupSystemConfiguration(int & party, string & host, int & port);
        static void setupSystemConfiguration();
    };

}
#endif //PILOT_UTILITIES_H
