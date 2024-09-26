#include "operator_utilities.h"
#include "data/csv_reader.h"

using namespace vaultdb;

vector<int> OperatorUtilities::getOrdinalsFromColNames(const QuerySchema & src_schema, const string & spec) {
    if(spec.empty()) return vector<int>();

    vector<string> col_names = CsvReader::split(spec);

    vector<int> ordinals;
    for(int i = 0; i < col_names.size(); ++i) {
        auto col = col_names.at(i);

        // chop leading and trailing whitespace
        auto col_begin = col.find_first_not_of(' ');
        auto col_end = col.find_last_not_of(' ');
        col = col.substr(col_begin, col_end - col_begin + 1);

        int ordinal = src_schema.getField(col).getOrdinal();
        ordinals.emplace_back(ordinal);
    }

    return ordinals;
}
