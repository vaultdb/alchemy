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
        static vector<int> getOrdinalsFromColNames(const QuerySchema & src_schema, const string & spec);

        template<typename B>
        static Project<B> *getProjectionFromColNames(Operator<B> *input, const string & col_names_csv) {
            auto schema = input->getOutputSchema();
            auto ordinals  = getOrdinalsFromColNames(schema, col_names_csv);
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
    static QuerySchema deriveSchema(const QuerySchema & src_schema, const vector<int> & ordinals) {
        QuerySchema dst_schema;
        int cnt = 0;
        for(auto ord : ordinals) {
            auto desc = src_schema.getField(ord);
            desc.setOrdinal(cnt);
            dst_schema.putField(desc);
            ++cnt;
        }

        dst_schema.initializeFieldOffsets();
        return dst_schema;

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
                int dst_pos = i;
                int to_write = ordinals.at(i);

                if(to_write == src_ordinal) {
                    dst_collation.push_back(ColumnSort(dst_pos, sort.second));
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
