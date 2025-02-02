#ifndef ENRICH_HTN_QUERY_H
#define ENRICH_HTN_QUERY_H

#include <query_table/query_table.h>
#include <operators/sort_merge_aggregate.h>
#include <operators/filter.h>

using namespace std;

namespace  vaultdb {
    class EnrichHtnQuery {


        SecureTable *input_table_;
        size_t cardinality_bound_;

    public:
        SecureTable *data_cube_;

        EnrichHtnQuery() {}; // placeholder
        ~EnrichHtnQuery() {
            if(data_cube_ != nullptr)
                delete data_cube_;
        }

        // compute initial data cube to prepare for rollups
        EnrichHtnQuery(SecureTable *input, const size_t & cardinality = 0);
        // after constructor, can add partial counts from other sites for semi-join optimization
        void unionWithPartialAggregates(vector<SecureTable *> partials);
        static  SecureTable *addPartialAggregates(vector<SecureTable *> partials);


        // initialize a new enrich object with list of partial counts from batches and/or semijoin optimization
        EnrichHtnQuery(vector<SecureTable *> input);
        static SecureTable *aggregatePartialPatientCounts( SecureTable *src, const size_t & cardinality_bound);

    private:

        //  breaking this up into smaller pieces to make it easier to debug

        // filter out patients that match exclusion criteria in at least one site
        // then deduplicate the patients
        SecureTable *filterPatients();


        // project it into the right format
        SecureTable *projectPatients(SecureTable *src);

        // aggregate the data cube
        void aggregatePatients(SecureTable *src);



// Project #1
// CASE WHEN age_days <= 28*365 THEN 1
//                WHEN age_days > 28*365 AND age_days <= 39*365 THEN 2
//              WHEN age_days > 39*365  AND age_days <= 50*365 THEN 3
//              WHEN age_days > 50*365 AND age_days <= 61*365 THEN 4
//              WHEN age_days > 61*365 AND age_days <= 72*365 THEN 5
//                WHEN age_days > 72*365  AND age_days <= 83*365 THEN 6
//                ELSE 7 END age_strata

/*        template<typename B>
        static inline Field<B> projectAgeStrata(const QueryTuple<B> & aTuple) {

            Field<B> ageDays = aTuple[2]; // age_days
            // either an IntField or SecureIntField

            Field<B> ageStrata = Field<B>::If(ageDays <= FieldFactory<B>::getInt(28*365), FieldFactory<B>::getInt(0),
                                              Field<B>::If(ageDays <= FieldFactory<B>::getInt(39*365), FieldFactory<B>::getInt(1),
                                                           Field<B>::If(ageDays <= FieldFactory<B>::getInt(50*365), FieldFactory<B>::getInt(2),
                                                                        Field<B>::If(ageDays <= FieldFactory<B>::getInt(61*365), FieldFactory<B>::getInt(3),
                                                                                     Field<B>::If(ageDays <= FieldFactory<B>::getInt(72*365), FieldFactory<B>::getInt(4),
                                                                                                  Field<B>::If(ageDays <= FieldFactory<B>::getInt(83*365), FieldFactory<B>::getInt(5), FieldFactory<B>::getInt(6)))))));

            return ageStrata;
        }

*/
      
    // Project #2
    //     CASE WHEN count(*) > 1 THEN 1 else 0 END AS multisite
        template<typename B>
        static Field<B> projectMultisite(const QueryTuple<B> & aTuple) {
            // int32_t
            Field<B> siteCount = aTuple[6];
            Field<B> zero =  FieldFactory<B>::getInt(0);
            Field<B> one =  FieldFactory<B>::getInt(1);

            B cmp = siteCount > FieldFactory<B>::getOne(siteCount.getType());

            return Field<B>::If(cmp, one, zero);

        }

        //    CASE WHEN MAX(numerator)=1 ^ COUNT(*) > 1 THEN 1 ELSE 0 END AS numerator_multisite
        template<typename B>
        static inline Field<B> projectNumeratorMultisite(const QueryTuple<B> & aTuple) {

            Field<B> inNumerator = aTuple[6];
            Field<B> siteCount = aTuple[8];
            Field<B> zero = FieldFactory<B>::getInt(0);
            Field<B> one = FieldFactory<B>::getInt(1);


            B multisite = (siteCount > FieldFactory<B>::getOne(siteCount.getType()));
            // only 0 || 1
            B numeratorTrue = inNumerator > FieldFactory<B>::getZero(inNumerator.getType());
            B condition = multisite & numeratorTrue;

            return Field<B>::If(condition, one, zero);

        }


        template<typename B>
        static inline Field<B> projectNumeratorMultisiteTable(const QueryTable<B> * src, const int & row) {
            // bool for numerator
            Field<B> inNumerator = src->getField(row, 6);
            Field<B> siteCount = src->getField(row, 8);
            auto desc = src->getSchema().getField(8);
            Field<B> zero = FieldFactory<B>::getInt(0, desc.size() + desc.bitPacked());
            Field<B> one = FieldFactory<B>::getInt(1, desc.size() + desc.bitPacked());


            B multisite = (siteCount > one);
            // only 0 || 1
            auto in_num_desc = src->getSchema().getField(6);
            B numeratorTrue = inNumerator.template getValue<B>(); //inNumerator > FieldFactory<B>::getInt(0, in_num_desc.size() + in_num_desc.bitPacked());
            B condition = multisite & numeratorTrue;

            return Field<B>::If(condition, FieldFactory<B>::getInt(1), FieldFactory<B>::getInt(0));

        }

        //    CASE WHEN MAX(numerator)=1 ^ COUNT(*) > 1 THEN 1 ELSE 0 END AS numerator_multisite
        template<typename B>
        static inline Field<B> projectDenominatorMultisite(const QueryTuple<B> & aTuple) {

            Field<B> inDenominator = aTuple[7];
            Field<B> siteCount = aTuple[8];
            Field<B> zero = FieldFactory<B>::getInt(0);
            Field<B> one = FieldFactory<B>::getInt(1);


            B multisite = (siteCount > FieldFactory<B>::getOne(siteCount.getType()));
            // only 0 || 1
            B denominatorTrue = inDenominator.template getValue<B>(); //inDenominator > FieldFactory<B>::getZero(inDenominator.getType());
            B condition = multisite & denominatorTrue;

            return Field<B>::If(condition, one, zero);

        }

        template<typename B>
        static inline Field<B> projectDenominatorMultisiteTable(const QueryTable<B> *src, const int & row) {
            Field<B> inDenominator = src->getField(row, 7);
            Field<B> siteCount = src->getField(row, 8);
            auto desc = src->getSchema().getField(8);
            Field<B> zero = FieldFactory<B>::getInt(0, desc.size() + desc.bitPacked());
            Field<B> one = FieldFactory<B>::getInt(1, desc.size() + desc.bitPacked());


            B multisite = (siteCount > one);
            // only 0 || 1
            auto in_denom_desc = src->getSchema().getField(7);
            B denominatorTrue = inDenominator.template getValue<B>(); //inDenominator > FieldFactory<B>::getZero(inDenominator.getType());
            B condition = multisite & denominatorTrue;

            return Field<B>::If(condition, FieldFactory<B>::getInt(1), FieldFactory<B>::getInt(0));

        }
    };


}


#endif //ENRICH_HTN_QUERY_H
