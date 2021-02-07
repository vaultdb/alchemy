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

        static Value projectMultisite(const QueryTuple &aTuple);
        static Value projectNumeratorMultisite(const QueryTuple &aTuple);
        static Value projectSecureAgeStrata(const QueryTuple &aTuple);

        // emp Integers for age cutoffs in age_strata projection:
        //static const vector<Integer> ageStrata;
        //static const vector<Integer> ageCutoff;
    };


    class FilterExcludedPatients : public Predicate {

        Value cmp;
    public:
        explicit FilterExcludedPatients(const bool & isEncrypted) {
            cmp = isEncrypted ? TypeUtilities::getZero(TypeId::ENCRYPTED_INTEGER32) : TypeUtilities::getZero(TypeId::INTEGER32);
        }

        ~FilterExcludedPatients() = default;
        [[nodiscard]] types::Value predicateCall(const QueryTuple & aTuple) const override {

            QueryTuple decrypted = aTuple.reveal();
            Value field = aTuple.getFieldPtr(8)->getValue();
            Value res = (field == cmp);
            return  res;
        }


    };

}


#endif //ENRICH_HTN_QUERY_H
