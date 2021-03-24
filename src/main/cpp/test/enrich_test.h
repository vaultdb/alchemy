#ifndef _ENRICH_TEST_H
#define _ENRICH_TEST_H

#include <util/data_utilities.h>
#include <operators/support/join_equality_predicate.h>
#include <operators/fkey_pkey_join.h>
#include <operators/csv_input.h>
#include <operators/project.h>
#include <operators/sort.h>
#include <operators/group_by_aggregate.h>
#include <operators/filter.h>
#include <test/support/EmpBaseTest.h>


using namespace vaultdb;
using namespace emp;


// to load test data, run from src/main/cpp the following:
// bash test/support/load-generated-data.sh <tuple count>


DEFINE_int32(party, 1, "party for EMP execution");
DEFINE_int32(port, 43439, "port for EMP execution");
DEFINE_string(alice_host, "127.0.0.1", "hostname for execution");



class EnrichTest : public EmpBaseTest  {

protected:


    const string unionedDbName = "enrich_htn_unioned";
    const string aliceDbName = "enrich_htn_alice";
    const string bobDbName = "enrich_htn_bob";

    shared_ptr<PlainTable> getAgeStrataProjection(shared_ptr<PlainTable> input, const bool & isEncrypted) const;

    shared_ptr<PlainTable> loadAndProjectPatientData(const string &dbName) const;

    shared_ptr<PlainTable> loadPatientExclusionData(const string &dbName) const;

    shared_ptr<PlainTable> loadAndJoinLocalData(const string &dbName) const;

    shared_ptr<SecureTable> getPatientCohort();

    void validateTable(const string &dbName, const string &sql, const SortDefinition  & expectedSortDefinition, const std::shared_ptr<PlainTable> &observedTable) const;


    static std::shared_ptr<SecureTable> rollUpAggregate(const int & ordinal, shared_ptr<SecureTable> src);

    shared_ptr<SecureTable> loadUnionAndDeduplicateData() const;

    void validateUnion(Operator<SecureBoolField> &sortOp, const SortDefinition &expectedSortOrder) const;

    shared_ptr<SecureTable> aggregatePatientData();


    shared_ptr<SecureTable> filterPatients();

    std::string getRollupExpectedResultsSql(const std::string &groupByColName);
};


#endif //  _ENRICH_TEST_H
