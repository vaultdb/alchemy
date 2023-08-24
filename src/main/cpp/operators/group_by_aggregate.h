#ifndef _GROUP_BY_AGGREGATE_H_
#define _GROUP_BY_AGGREGATE_H_

#include "operator.h"
#include <expression/generic_expression.h>
#include <operators/support/aggregate_id.h>
#include "common/defs.h"



namespace vaultdb {
    template<typename B>
    class GroupByAggregate : public Operator<B> {

    public:
        std::vector<ScalarAggregateDefinition> aggregate_definitions_;
        std::vector<int32_t> group_by_;
        SortDefinition effective_sort_;


        GroupByAggregate(const GroupByAggregate<B> & src) : Operator<B>(src), aggregate_definitions_(src.aggregate_definitions_), group_by_(src.group_by_), cardinality_bound_(src.cardinality_bound_), effective_sort_(src.effective_sort_) {}

        GroupByAggregate(Operator<B> *child, const vector<int32_t> &group_bys, const vector<ScalarAggregateDefinition> &aggregates, const SortDefinition & effective_sort = SortDefinition(), const int & cardinality_bound = -1)
            : Operator<B>(child),  aggregate_definitions_(aggregates), group_by_(group_bys), cardinality_bound_(cardinality_bound), effective_sort_(effective_sort) {

        }

        GroupByAggregate(QueryTable<B> *child, const vector<int32_t> &group_bys, const vector<ScalarAggregateDefinition> &aggregates, const SortDefinition & effective_sort = SortDefinition(), const int & cardinality_bound = -1)
                : Operator<B>(child),  aggregate_definitions_(aggregates), group_by_(group_bys), cardinality_bound_(cardinality_bound), effective_sort_(effective_sort)
        { }


        size_t getCardinalityBound() const { return cardinality_bound_; }

        // this checks if a given input sort will enable SMA to produce correct results
        // including this in parent class so NLA input can check for this too
        bool sortCompatible(const SortDefinition & sorted_on) {

            return sortCompatible(sorted_on, this->group_by_, this->effective_sort_);
        }

        // for use in plan parser before GroupByAggregate object constructed
        static bool sortCompatible(const SortDefinition & input_sort, const vector<int> & group_by, const SortDefinition & effective_sort = SortDefinition()) {
            int col_cnt = (effective_sort.empty()) ? group_by.size() : effective_sort.size();

            if(input_sort.size() < col_cnt)
                return false;

            if(!effective_sort.empty()) {
                for(size_t idx = 0; idx < col_cnt; ++idx) {
                    // ASC || DESC does not matter here
                    // effective_sort is output sort
                    int out_col_idx = effective_sort[idx].first;
                    int in_col_idx = -1;
                    for(int i = 0; i < group_by.size(); ++i) {
                        if(out_col_idx == group_by[i]) {
                            in_col_idx = group_by[i];
                            break;
                        }
                    }

                    if(in_col_idx == -1) return false; // sort col must be in group_by

                    if(in_col_idx != input_sort[idx].first) {
                        return false;
                    }
                }
                return true;
            } // end effective sort case

            for(int i = 0; i < col_cnt; ++i) {
                if(group_by[i] != input_sort[i].first) {
                    return false;
                }
            }

            return true;
        }

    protected:
        size_t cardinality_bound_  = 0; // stored in SMA for planning purposes, only used in NLA


        inline string getParameters() const override {
            stringstream  ss;
            ss << "group-by: (" << group_by_[0];
            for(uint32_t i = 1; i < group_by_.size(); ++i)
                ss << ", " << group_by_[i];

            ss << ") aggs: (" << aggregate_definitions_[0].toString();

            for(uint32_t i = 1; i < aggregate_definitions_.size(); ++i) {
                ss << ", " << aggregate_definitions_[i].toString();
            }

            ss << ")";
            return ss.str();
        }

        QuerySchema generateOutputSchema(const QuerySchema & input_schema) const {
            QuerySchema output_schema;
            size_t i;
            bool bit_packing = SystemConfiguration::getInstance().bitPackingEnabled();
            int input_row_cnt = this->getChild(0)->getOutputCardinality();

            for(i = 0; i < group_by_.size(); ++i) {
                QueryFieldDesc src = input_schema.getField(group_by_[i]);
                QueryFieldDesc dst(i, src.getName(), src.getTableName(), src.getType());
                dst.setStringLength(src.getStringLength());
                output_schema.putField(dst);
            }


            for(i = 0; i < aggregate_definitions_.size(); ++i) {
                ScalarAggregateDefinition agg = aggregate_definitions_[i];
                FieldType agg_type = (agg.ordinal >= 0) ?
                                     input_schema.getField(agg.ordinal).getType() :
                                     (std::is_same_v<B, emp::Bit> ? FieldType::SECURE_LONG : FieldType::LONG);
                QueryFieldDesc f;
                if((agg.type == AggregateId::MIN || agg.type == AggregateId::MAX) && std::is_same_v<B, Bit> && bit_packing) {
                    f = QueryFieldDesc(input_schema.getField(agg.ordinal), i + group_by_.size()); // copy out bit packing info
                }
                else if (agg.type == AggregateId::COUNT && std::is_same_v<B, Bit> && bit_packing) {
                    f = QueryFieldDesc(i + group_by_.size(), aggregate_definitions_[i].alias, "", agg_type);
                    f.initializeFieldSizeWithCardinality(input_row_cnt);
                }
                else { // sum, avg
                    f = QueryFieldDesc(i + group_by_.size(), aggregate_definitions_[i].alias, "", agg_type);
                }
                output_schema.putField(f);
            }

            output_schema.initializeFieldOffsets();
            return output_schema;
        }



    };
}

#endif //_GROUP_BY_AGGREGATE_H