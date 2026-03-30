#ifndef _GROUP_BY_AGGREGATE_H_
#define _GROUP_BY_AGGREGATE_H_

#include "operators/operator.h"
#include <expression/generic_expression.h>
#include <operators/support/aggregate_id.h>
#include "common/defs.h"
#include <string>
#include <vector>


namespace vaultdb {
template<typename B>
class GroupByAggregate : public Operator<B> {

    public:
        std::vector<ScalarAggregateDefinition> aggregate_definitions_;
        std::vector<int32_t> group_by_;
        SortDefinition effective_sort_;
        size_t cardinality_bound_  = 0; // stored in SMA for planning purposes, only used in NLA


    GroupByAggregate(const GroupByAggregate<B> & src) : Operator<B>(src), aggregate_definitions_(src.aggregate_definitions_), group_by_(src.group_by_), effective_sort_(src.effective_sort_), cardinality_bound_(src.cardinality_bound_) {}

    GroupByAggregate(Operator<B> *child, const vector<int32_t> &group_bys, const vector<ScalarAggregateDefinition> &aggregates, const SortDefinition & effective_sort = SortDefinition(), const int & cardinality_bound = -1)
        : Operator<B>(child),  aggregate_definitions_(aggregates), group_by_(group_bys), effective_sort_(effective_sort), cardinality_bound_(cardinality_bound) {
        if(cardinality_bound == -1) {
            cardinality_bound_ = child->getOutputCardinality();
        }

    }

    GroupByAggregate(QueryTable<B> *child, const vector<int32_t> &group_bys, const vector<ScalarAggregateDefinition> &aggregates, const SortDefinition & effective_sort = SortDefinition(), const int & cardinality_bound = -1)
            : Operator<B>(child),  aggregate_definitions_(aggregates), group_by_(group_bys), effective_sort_(effective_sort), cardinality_bound_(cardinality_bound)
    {
        if(cardinality_bound == -1) {
            cardinality_bound_ = child->tuple_cnt_;
        }
    }


    size_t getCardinalityBound() const { return cardinality_bound_; }

    // this checks if a given input sort will enable SMA to produce correct results
    // including this in parent class so NLA input can check for this too
    bool sortCompatible(const SortDefinition & sorted_on) {

        return sortCompatible(sorted_on, this->group_by_, this->effective_sort_);
    }

    // for use in plan parser before GroupByAggregate object constructed
    static bool sortCompatible(const SortDefinition & input_sort, const vector<int> & group_by, const SortDefinition & effective_sort = SortDefinition()) {
        size_t col_cnt = (effective_sort.empty()) ? group_by.size() : effective_sort.size();

        if(input_sort.size() < col_cnt)
            return false;

        if(!effective_sort.empty()) {
            // Case that child sort has -1 dummy tag in its collation
            size_t input_idx = 0;
            int start_idx = 0;

            if(input_sort[input_idx].first == -1)
                input_idx++;

            if (effective_sort[0].first == -1)
                ++start_idx;

            // need at least one group-by col.  If it's just -1, then return false
            if(input_idx >= input_sort.size()) {
                return false;
            }

            vector<int> effective_sort_ordinals;
            for (int i = start_idx; i < effective_sort.size(); ++i) {
                effective_sort_ordinals.push_back(effective_sort.at(i).first);
            }
            std::sort(effective_sort_ordinals.begin(), effective_sort_ordinals.end(), std::less<int>());

            int subvec_size = effective_sort.size() - start_idx;

            vector<int> input_sort_ordinals;
            for (int i = input_idx; i < (input_idx + subvec_size); ++i) {
                input_sort_ordinals.push_back(input_sort.at(i).first);
            }

            std::sort(input_sort_ordinals.begin(), input_sort_ordinals.end(), std::less<int>());
            if (std::equal(input_sort_ordinals.begin(), input_sort_ordinals.end(), effective_sort_ordinals.begin())) {
                return true;
            }
            return false;
        } // end effective sort case


        vector<int> group_bys_sorted = group_by;
        std::sort(group_bys_sorted.begin(), group_bys_sorted.end(), std::less<int>());
        vector<int> input_cols_sorted;
        // two cases: (-1, group by cols) or (group by cols)
        if (input_sort[0].first == -1 && input_sort.size() >= group_by.size() + 1) {
            for (int i = 0; i < group_by.size(); ++i) {
                input_cols_sorted.push_back(input_sort[i+1].first);
            }
            std::sort(input_cols_sorted.begin(), input_cols_sorted.end(), std::less<int>());
        }


        if (input_sort.size() >= group_by.size()) {
            // order of cols does not matter as long as all are in the first GB cols of input sort to ensure "binned alike" rows  are adjacent
            for (int i = 0; i < group_by.size(); ++i) {
                input_cols_sorted.push_back(input_sort[i].first);
            }
        }

        std::sort(input_cols_sorted.begin(), input_cols_sorted.end(), std::less<int>());

        if (std::equal(input_cols_sorted.begin(), input_cols_sorted.end(), group_bys_sorted.begin())) {
            return true;
        }

       return false;
    }

    bool operator==(const Operator<B> &other) const override {
        if(other.getType() != this->getType()) return false;

        auto rhs = dynamic_cast<const GroupByAggregate<B> *>(&other);
        if(!Utilities::vectorEquality(this->group_by_, rhs->group_by_)) return false;
        if(!Utilities::vectorEquality(this->aggregate_definitions_, rhs->aggregate_definitions_)) return false;
        if(!Utilities::vectorEquality(this->effective_sort_, rhs->effective_sort_)) return false;

        if(this->cardinality_bound_ != rhs->cardinality_bound_) return false;

        return this->operatorEquality(other);
    }

    int getSourceOrdinal(const int & output_ordinal) const override {
       if(output_ordinal < group_by_.size()) {
           return group_by_[output_ordinal];
       } else {
          // see what aggregator associated with this ordinal is computing over
          return aggregate_definitions_[output_ordinal - group_by_.size()].ordinal;
       }
    }

    int getDestOrdinal(Operator<B> *src, const int & src_ordinal) const override {

        auto pos = std::find(group_by_.begin(), group_by_.end(), src_ordinal);
        if(pos != group_by_.end()) {
            return std::distance(group_by_.begin(), pos);
        } else {
          // return first aggregator that references src_ordinal
          for(int i = 0; i < aggregate_definitions_.size(); ++i) {
              if(aggregate_definitions_[i].ordinal == src_ordinal) {
                  return group_by_.size() + i;
              }
          }
        }
        throw std::runtime_error("Source ordinal " + std::to_string(src_ordinal) + " has no 1:1 mapping in output relation!");
    }


    protected:
        inline string getParameters() const override {
            stringstream  ss;
            ss << "group-by: (" << group_by_[0];
            for(size_t i = 1; i < group_by_.size(); ++i)
                ss << ", " << group_by_[i];

        ss << ") aggs: (" << aggregate_definitions_[0].toString();

        for(size_t i = 1; i < aggregate_definitions_.size(); ++i) {
            ss << ", " << aggregate_definitions_[i].toString();
        }

        ss << ")";
        return ss.str();
    }

    QuerySchema generateOutputSchema(const QuerySchema & input_schema) const {
        QuerySchema output_schema;
        size_t i;
        int input_row_cnt = this->getChild(0)->getOutputCardinality();

        for(i = 0; i < group_by_.size(); ++i) {
            QueryFieldDesc src = input_schema.getField(group_by_[i]);
            QueryFieldDesc dst(src, i);
            output_schema.putField(dst);
        }

        for(i = 0; i < aggregate_definitions_.size(); ++i) {
            ScalarAggregateDefinition agg = aggregate_definitions_[i];
            FieldType agg_type = (agg.ordinal >= 0) ?
                                 input_schema.getField(agg.ordinal).getType() :
                                 (std::is_same_v<B, emp::Bit> ? FieldType::SECURE_LONG : FieldType::LONG);
            QueryFieldDesc f;
            if((agg.type == AggregateId::MIN || agg.type == AggregateId::MAX) && std::is_same_v<B, Bit>) {
                f = QueryFieldDesc(input_schema.getField(agg.ordinal), i + group_by_.size()); // copy out bit packing info
                f.setName("", agg.alias);
            } else if (agg.type == AggregateId::COUNT && std::is_same_v<B, Bit>) {
                f = QueryFieldDesc(i + group_by_.size(), aggregate_definitions_[i].alias, "", agg_type);
              if(input_row_cnt > 0) {
                  f.initializeFieldSizeWithCardinality(input_row_cnt);
              }
            } else { // sum, avg
                f = QueryFieldDesc(i + group_by_.size(), aggregate_definitions_[i].alias, "", agg_type);
            }
            output_schema.putField(f);
        }

        output_schema.initializeFieldOffsets();
        return output_schema;
    }



};
} // namespace vaultdb

#endif //_GROUP_BY_AGGREGATE_H