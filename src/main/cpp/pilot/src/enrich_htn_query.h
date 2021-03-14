#ifndef ENRICH_HTN_QUERY_H
#define ENRICH_HTN_QUERY_H

#include <query_table/query_table.h>
#include <operators/group_by_aggregate.h>
#include <filter.h>

using namespace std;

namespace  vaultdb {
    class EnrichHtnQuery {


        shared_ptr<QueryTable> inputTable;
        shared_ptr<QueryTable> dataCube;
        shared_ptr<GroupByAggregate> aggregator;

    public:
        // compute initial data cube to prepare for rollups
        EnrichHtnQuery(shared_ptr<QueryTable> & input);
        shared_ptr<QueryTable> rollUpAggregate(const int &ordinal) const;

    private:

        //  breaking this up into smaller pieces to make it easier to debug

        // filter out patients that match exclusion criteria in at least one site
        // then deduplicate the patients
        shared_ptr<QueryTable> filterPatients();


        // project it into the right format
        shared_ptr<QueryTable> projectPatients(shared_ptr<QueryTable> src);

        // aggregate the data cube
        void aggregatePatients(shared_ptr<QueryTable> src);

        static Field projectMultisite(const QueryTuple &aTuple);
        static Field projectNumeratorMultisite(const QueryTuple &aTuple);
        static Field projectSecureAgeStrata(const QueryTuple &aTuple);


        // emp Integers for age cutoffs in age_strata projection:
        //static const vector<Integer> ageStrata;
        //static const vector<Integer> ageCutoff;
    };


    class FilterExcludedPatients : public Predicate {

        Field cmp;
    public:
        explicit FilterExcludedPatients(const bool & isEncrypted) {
            cmp = isEncrypted ? TypeUtilities::getZero(FieldType::SECURE_INT) : TypeUtilities::getZero(FieldType::INT);
        }

        ~FilterExcludedPatients() = default;
        [[nodiscard]] Field predicateCall(const QueryTuple & aTuple) const override {

            QueryTuple decrypted = aTuple.reveal();
            Field field = aTuple.getFieldPtr(8)->getValue();
            Field res = (field == cmp);
            return  res;
        }


    };

}


#endif //ENRICH_HTN_QUERY_H
