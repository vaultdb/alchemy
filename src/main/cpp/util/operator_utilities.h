#ifndef _OPERATOR_UTILITIES_
#define _OPERATOR_UTILITIES_

#include "operators/operator.h"
#include "data/csv_reader.h"
#include "operators/project.h"

namespace vaultdb {
    class OperatorUtilities {

    public:

        // e.g., given lineitem scan as `input` and spec of "l_orderkey, l_linenumber"
        // project to $0, $2
        static vector<int> getOrdinalsFromColNames(const QuerySchema & src_schema, const string & spec) {
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

        template<typename B>
        static Project<B> *getProjectionFromColNames(Operator<B> *input, const string & spec) {
            auto schema = input->getOutputSchema();
            auto ordinals  = getOrdinalsFromColNames(schema, spec);
            return getProjectionFromOrdinals(input, ordinals);
        }

        template<typename B>
        static Project<B> *getProjectionFromOrdinals(Operator<B> *input, const vector<int> & ordinals) {
            auto schema = input->getOutputSchema();

            ExpressionMapBuilder<B> builder(schema);

            for(int i = 0; i < ordinals.size(); ++i) {

                int ordinal = ordinals.at(i);
                builder.addMapping(ordinal, i);
            }

            return new Project<B>(input, builder.getExprs());
        }

    static SortDefinition deriveCollation(const SortDefinition & src_collation, const vector<int> & ordinals) {
        SortDefinition  dst_collation;
        // *** Check to see if order-by carries over
        for(ColumnSort sort : src_collation) {
            int32_t src_ordinal = sort.first;
            bool found = false;
            if(src_ordinal == -1) {
                found = true; // dummy tag automatically carries over
                dst_collation.push_back(sort);
                continue;
            }
            for(int i = 0; i < ordinals.size(); ++i) {
                int dst_ordinal = i;
                int src_ordinal = ordinals.at(i);

                if(dst_ordinal == src_ordinal) {
                    dst_collation.push_back(ColumnSort(dst_ordinal, sort.second));
                    found = true;
                    break;
                }
            } // end search for mapping
            if(!found) {
                break;
            } // broke the sequence of mappings
        }

        return dst_collation;
    }

    };
}


#endif
