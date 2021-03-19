#ifndef GROUP_BY_AGGREGATE_IMPL_H
#define GROUP_BY_AGGREGATE_IMPL_H


#include "query_table/field/bool_field.h"
#include "query_table/field/int_field.h"
#include "query_table/field/secure_long_field.h"
#include "query_table/field/long_field.h"
#include "query_table/field/float_field.h"

#include <query_table/query_tuple.h>
#include <query_table/field/field_factory.h>
#include <climits>
#include <cfloat>
#include <cstdint>



namespace vaultdb {

    // virtual class, wrapper for GroupByAggregateImpl below
    template<typename B>
    class GroupByAggregator {

    public:
        virtual ~GroupByAggregator() = default;
        GroupByAggregator(const int32_t &ordinal, const FieldType & fieldType) : aggregateOrdinal(ordinal), aggregateType(fieldType) {}
        virtual void initialize(const QueryTuple & tuple, const B & isDummy) = 0;
        virtual void accumulate(const QueryTuple & tuple, const B & isDummy) = 0;
        virtual Field getResult() const = 0;
        virtual inline void updateGroupByBinBoundary(const B & isNewBin, B & nonDummyBinFlag) {
            assert(isNewBin->getType() == nonDummyBinFlag->getType() && ( nonDummyBinFlag->getType() == FieldType::BOOL || nonDummyBinFlag.getType() == FieldType::SECURE_BOOL));

           // B choice = static_cast<B &>(*isNewBin);
            B f(false);
            Field::compareAndSwap(isNewBin, nonDummyBinFlag, f);

        }


        virtual inline  B getDummyTag(const B & isLastEntry, const B & nonDummyBin)  const {
            assert((isLastEntry->getType() == FieldType::BOOL || isLastEntry->getType() == FieldType::SECURE_BOOL)
                   && ((nonDummyBin->getType() == FieldType::BOOL || nonDummyBin->getType() == FieldType::SECURE_BOOL)));
            B nDummyBin = !nonDummyBin;
            B other(true);

            // if last entry, then return true nonDummyBin status, otherwise mark as dummy
            // ! nonDummy bin b/c if it is a real bin, we want dummyTag = false

            B res = nDummyBin.select(isLastEntry, other);
            return res;
        }

         FieldType getType() const { return aggregateType; }
         int32_t getOrdinal() const { return aggregateOrdinal; }

    protected:
        FieldType aggregateType;
        int32_t aggregateOrdinal;
        QueryTuple lastRealTuple;


    };

/*
    // T = field type (e.g., BoolField), B = bool type (BoolField || SecureBoolField)
    template<typename T, typename B>
    class GroupByAggregateImpl {
    public:
        explicit GroupByAggregateImpl(const int32_t &ordinal, const FieldType & fieldType) :
                                                            zero(FieldFactory::getZero(fieldType)), one(FieldFactory::getOne(fieldType)){ };

        virtual ~GroupByAggregateImpl() = default;


    protected:

        // signed int because -1 denotes *, as in COUNT(*)
        T zero;
        T one;

    };
*/


   // always a long / emp::Int for count
    template<typename T, typename B>
    class GroupByCountImpl :   public GroupByAggregator<B> {
    public:
        explicit GroupByCountImpl(const int32_t & ordinal, const FieldType & aggType)
        : GroupByAggregator<B>(ordinal, aggType) {
            assert(aggType == FieldType::LONG || aggType == FieldType::SECURE_LONG);
            zero =  FieldFactory::getZero(aggType);
            one =  FieldFactory::getZero(aggType);

            runningCount = zero;
        };

         void initialize(const QueryTuple & tuple, const B & isDummy) override;
         void accumulate(const QueryTuple & tuple, const B  & isDummy) override;
         Field getResult() const override;
        ~GroupByCountImpl() = default; //{ delete runningCount; }

    private:
        T zero;
        T one;
        T runningCount;

    };

    /*
    template<typename T, typename B>
    class GroupBySumImpl : public  GroupByAggregateImpl<T,B>,   public GroupByAggregator<B> {
    public:
        explicit GroupBySumImpl(const int32_t & ordinal, const FieldType & aggType) : GroupByAggregateImpl<T,B>(ordinal, aggType) {
            if(GroupByAggregateImpl<T,B>::aggregateType == FieldType::INT) {
                GroupByAggregateImpl<T,B>::aggregateType = FieldType::LONG; // accommodate psql handling of sum for validation
                delete GroupByAggregateImpl<T,B>::zero;
                delete GroupByAggregateImpl<T,B>::one;
                GroupByAggregateImpl<T,B>::zero = FieldFactory::getZero(GroupByAggregateImpl<T,B>::aggregateType);
                GroupByAggregateImpl<T,B>::one = FieldFactory::getOne(GroupByAggregateImpl<T,B>::aggregateType);
            }
        };
        void initialize(const QueryTuple & tuple, const Field *isDummy) override;
        void accumulate(const QueryTuple & tuple, const Field *isDummy) override;
         Field getResult() const override;
        ~GroupBySumImpl() {
            delete runningSum;
        }

    private:
        T runningSum;
        T readVal(const QueryTuple & tuple) const;

    };

    template<typename T, typename B>
    class GroupByAvgImpl :  public  GroupByAggregateImpl<T,B>,  public GroupByAggregator<B> {
    public:
        GroupByAvgImpl(const int32_t & ordinal, const FieldType & aggType);
        void initialize(const QueryTuple & tuple, const Field *isDummy) override;
        void accumulate(const QueryTuple & tuple, const Field *isDummy) override;
        Field getResult() const override;
        ~GroupByAvgImpl() = default;

    private:
        T runningSum;
        T runningCount;

    };

    template<typename T, typename B>
    class GroupByMinImpl : public  GroupByAggregateImpl<T,B>,  public GroupByAggregator<B> {
    public:
        explicit GroupByMinImpl(const int32_t & ordinal, const FieldType & aggType);
        void initialize(const QueryTuple & tuple, const Field *isDummy) override;
        void accumulate(const QueryTuple & tuple, const Field *isDummy) override;
        Field getResult() const override;
        ~GroupByMinImpl() { delete runningMin; }

    private:
        T runningMin;
        void resetRunningMin();


    };

    template<typename T, typename B>
    class GroupByMaxImpl : public  GroupByAggregateImpl<T,B>,  public GroupByAggregator<B> {
    public:
         GroupByMaxImpl(const int32_t & ordinal, const FieldType & aggType);
        void initialize(const QueryTuple & tuple, const Field *isDummy) override;
        void accumulate(const QueryTuple & tuple, const Field *isDummy) override;
         Field getResult() const override;
        ~GroupByMaxImpl() { delete runningMax; }

    private:
        T runningMax;
        void resetRunningMax();


    };
          */

}

#endif //_GROUP_BY_AGGREGATE_IMPL_H
