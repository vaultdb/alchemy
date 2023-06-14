#ifndef _GROUP_BY_AGGREGATE_IMPL_H
#define _GROUP_BY_AGGREGATE_IMPL_H


#include <cstdint>
#include <query_table/query_tuple.h>
#include <query_table/field/field_factory.h>
#include <query_table/query_table.h>
#include <cfloat>

#include <util/field_utilities.h>

namespace vaultdb {
    template<typename B>
    class GroupByAggregateImpl {
    public:
        explicit GroupByAggregateImpl(const int32_t & ordinal, const FieldType & agg_type) :
                aggregate_ordinal_(ordinal), aggregate_type_(agg_type),
                zero_(FieldFactory<B>::getZero(aggregate_type_)), one_(FieldFactory<B>::getOne(aggregate_type_)){};

        virtual ~GroupByAggregateImpl() = default;

        virtual void initialize(const QueryTable<B> *input) = 0; // for use on the first row only of an aggregate
        virtual void accumulate(const QueryTable<B> *table, const int & row,  const B &group_by_match) = 0;

        virtual Field<B> getResult() = 0;
        virtual FieldType getType() const { return aggregate_type_; }
        virtual bool packedFields() const = 0; // is the aggregator emitting a packed field?

    protected:

        // signed int because -1 denotes *, as in COUNT(*)
        int32_t aggregate_ordinal_;
        FieldType aggregate_type_;
        Field<B> zero_;
        Field<B> one_;

    };

    template<typename B> class GroupByCountImpl;

    template<>
    class GroupByCountImpl<emp::Bit> : public  GroupByAggregateImpl<emp::Bit> {
    public:
         GroupByCountImpl(const int32_t & ordinal, const FieldType & agg_type)  : GroupByAggregateImpl<Bit>(ordinal, agg_type)
        {
            GroupByAggregateImpl<Bit>::aggregate_type_ = FieldType::SECURE_LONG;
            running_count_ = emp::Integer(64, 0, PUBLIC); // publicly initialize to zero
        }

        // run on first row alone for a GroupByAggregate
         inline void initialize(const SecureTable *table) override {
            Bit dummy_tag = table->getDummyTag(0);
            running_count_.bits[0] = !dummy_tag;
        }

        inline void accumulate(const SecureTable *table, const int & row,  const Bit &group_by_match) override {
            Bit dummy_tag = table->getDummyTag(row);
            emp::Integer incr = zero_i_;
            incr.bits[0] = !dummy_tag;

            running_count_ = emp::If(group_by_match, running_count_, zero_i_);
            running_count_ = running_count_ + incr;
        }

        inline Field<Bit> getResult() override {
            return SecureField(FieldType::SECURE_LONG, running_count_, 0);
        }

        inline FieldType getType() const override {
                return FieldType::SECURE_LONG;
        }

        bool packedFields() const override { return false; }
       ~GroupByCountImpl() = default;


   private:
        emp::Integer running_count_;
        const emp::Integer zero_i_ = Integer(64, 0, PUBLIC);
        const emp::Integer one_i_ = Integer(64, 1, PUBLIC);

    };

    template<>
    class GroupByCountImpl<bool> : public  GroupByAggregateImpl<bool> {
    public:
        explicit GroupByCountImpl(const int32_t & ordinal, const FieldType & agg_type)  : GroupByAggregateImpl<bool>(ordinal, agg_type)
        {
            GroupByAggregateImpl<bool>::aggregate_type_ = FieldType::LONG;
            running_count_ = 0L;

        }

        // run on first row alone for a GroupByAggregate
        inline void initialize(const PlainTable *table) override {
            running_count_ = table->getDummyTag(0) ? 0L : 1L;
        }

        inline void accumulate(const PlainTable *table, const int & row,  const bool &group_by_match) override {
            long incr = table->getDummyTag(row) ? 0L : 1L;
            if(!group_by_match)
                running_count_ = 0L;

            running_count_ = running_count_ + incr;
        }

        inline PlainField getResult() override {
            return PlainField(FieldType::LONG, running_count_, 0);
        }

        inline FieldType getType() const override {
            return FieldType::LONG;  // count always returns a long
        }

        bool packedFields() const override { return false; }

        ~GroupByCountImpl() = default;


    private:
        int64_t running_count_;

    };


    template<typename B>
    class GroupBySumImpl : public  GroupByAggregateImpl<B> {
    public:
        explicit GroupBySumImpl(const int32_t & ordinal, const FieldType & agg_type) : GroupByAggregateImpl<B>(ordinal, agg_type) {
            running_sum_ = GroupByAggregateImpl<B>::zero_;
            assert(agg_type != FieldType::STRING && agg_type != FieldType::SECURE_STRING);
        }

         inline void initialize(const QueryTable<B> *table) override{
            running_sum_ = Field<B>::If(table->getDummyTag(0), running_sum_, table->getField(0, GroupByAggregateImpl<B>::aggregate_ordinal_));
        }

        //virtual void accumulate(const QueryTuple<B> & tuple, const B &group_by_match) override;
         inline void accumulate(const QueryTable<B> *table, const int & row,  const B &group_by_match) override {

            Field<B> incr = Field<B>::If(table->getDummyTag(row), GroupByAggregateImpl<B>::zero_, table->getField(row, GroupByAggregateImpl<B>::aggregate_ordinal_));

            running_sum_ = Field<B>::If(group_by_match, running_sum_, GroupByAggregateImpl<B>::zero_);

            running_sum_ = running_sum_ + incr;
        }
        inline Field<B> getResult() override {
            return running_sum_;
        }
        inline FieldType getType() const override { return GroupByAggregateImpl<B>::aggregate_type_; }
        bool packedFields() const override { return false; }

        ~GroupBySumImpl() = default;

    private:
        Field<B> running_sum_;

    };

    template<typename B>
    class GroupByAvgImpl : public  GroupByAggregateImpl<B> {
    public:
        GroupByAvgImpl(const int32_t & ordinal, const FieldType & agg_type) : GroupByAggregateImpl<B>(ordinal, agg_type)  {
            running_sum_ = GroupByAggregateImpl<B>::zero_;
            running_count_ = GroupByAggregateImpl<B>::zero_;
        }

         inline void initialize(const QueryTable<B> *table) override {
           running_count_ = Field<B>::If(table->getDummyTag(0),  GroupByAggregateImpl<B>::zero_,  GroupByAggregateImpl<B>::one_);
           running_sum_ = Field<B>::If(table->getDummyTag(0), running_sum_, table->getField(0, GroupByAggregateImpl<B>::aggregate_ordinal_));
        }

         inline void accumulate(const QueryTable<B> *table, const int & row,  const B &group_by_match) override {
            // count
            Field<B> cnt_incr = Field<B>::If(table->getDummyTag(row), GroupByAggregateImpl<B>::zero_, GroupByAggregateImpl<B>::one_);
            running_count_ = Field<B>::If(group_by_match, running_count_, GroupByAggregateImpl<B>::zero_);
            running_count_ = running_count_ + cnt_incr;


            // sum
            Field<B> sum_incr = Field<B>::If(table->getDummyTag(row), GroupByAggregateImpl<B>::zero_, table->getField(row, GroupByAggregateImpl<B>::aggregate_ordinal_));

            running_sum_ = Field<B>::If(group_by_match, running_sum_, GroupByAggregateImpl<B>::zero_);

            running_sum_ = running_sum_ + sum_incr;

        }
        inline Field<B> getResult() override {
            return running_sum_ / running_count_;
        }

        bool packedFields() const override { return false; }
        ~GroupByAvgImpl() = default;

    private:
        Field<B> running_sum_;
        Field<B> running_count_;

    };

    template<typename B>
    class GroupByMinImpl : public  GroupByAggregateImpl<B> {
    public:
        explicit GroupByMinImpl(const int32_t & ordinal, const QueryFieldDesc & input_schema) : GroupByAggregateImpl<B>(ordinal, input_schema.getType()) {
            if(input_schema.bitPacked()) {
                // generate max
                assert(this->aggregate_type_ == FieldType::SECURE_LONG || this->aggregate_type_ == FieldType::SECURE_INT);
                Integer max = emp::Integer(input_schema.size() + 1, 0);
                Bit one(true);
                Bit *write_cursor = max.bits.data();
                for(int i = 0; i < input_schema.size(); ++i) {
                  *write_cursor = one;
                  ++write_cursor;
                }
                running_min_ = Field<B>(this->aggregate_type_, max, 0);
                packed_fields_ = true;
            }
            else
                running_min_ = FieldFactory<B>::getMax(input_schema.getType());
        }

        inline void initialize(const QueryTable<B> *table) override {
            running_min_ = Field<B>::If(table->getDummyTag(0), running_min_, table->getPackedField(0, GroupByAggregateImpl<B>::aggregate_ordinal_));
        }

        inline void accumulate(const QueryTable<B> *table, const int & row,  const B &group_by_match) override {
            // if a match and not a dummy
            Field<B> agg_input = table->getPackedField(row, GroupByAggregateImpl<B>::aggregate_ordinal_);


            Field<B> new_min = Field<B>::If(agg_input < running_min_, agg_input, running_min_);
            B input_dummy_tag = table->getDummyTag(row);

            running_min_ = Field<B>::If(group_by_match & !input_dummy_tag, new_min, running_min_);
            // new bin
            running_min_ = Field<B>::If(!group_by_match & !input_dummy_tag, agg_input, running_min_);

        }
        inline Field<B> getResult() override {
            return running_min_;
        }

        bool packedFields() const override { return packed_fields_; }
        ~GroupByMinImpl() = default;

    private:
        Field<B> running_min_;
        bool packed_fields_ = false;

    };

    template<typename B>
    class GroupByMaxImpl : public  GroupByAggregateImpl<B> {
    public:
        GroupByMaxImpl(const int32_t & ordinal, const QueryFieldDesc & input_schema)  : GroupByAggregateImpl<B>(ordinal, input_schema.getType()) {

            if(input_schema.bitPacked()) {
                // generate max
                assert(this->aggregate_type_ == FieldType::SECURE_LONG || this->aggregate_type_ == FieldType::SECURE_INT);
                Integer min = emp::Integer(input_schema.size() + 1, input_schema.getFieldMin()); // +1 for 2's complement
                running_max_ = Field<B>(this->aggregate_type_, min, 0);
                packed_fields_ = true;
            }
            else
                running_max_ = FieldFactory<B>::getMin(input_schema.getType());
        }

        inline void initialize(const QueryTable<B> *table) override {
            running_max_ = Field<B>::If(table->getDummyTag(0), running_max_, table->getPackedField(0, GroupByAggregateImpl<B>::aggregate_ordinal_));
        }


        inline void accumulate(const QueryTable<B> *table, const int & row,  const B &group_by_match) override {
            // if a match and not a dummy
            Field<B> agg_input = table->getPackedField(row, GroupByAggregateImpl<B>::aggregate_ordinal_);
            Field<B> new_min = Field<B>::If(agg_input > running_max_, agg_input, running_max_);
            B input_dummy_tag = table->getDummyTag(row);

            running_max_ = Field<B>::If(group_by_match & !input_dummy_tag, new_min, running_max_);
            // new bin
            running_max_ = Field<B>::If(!group_by_match & !input_dummy_tag, agg_input, running_max_);


        }

        inline Field<B> getResult() override { return running_max_; }
        bool packedFields() const override { return packed_fields_; }
        ~GroupByMaxImpl() = default;

    private:
        Field<B> running_max_;
        bool packed_fields_ = false;


    };
}
#endif //_GROUP_BY_AGGREGATE_IMPL_H
