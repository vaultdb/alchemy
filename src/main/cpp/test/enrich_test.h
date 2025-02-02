#ifndef _ENRICH_TEST_H
#define _ENRICH_TEST_H

#include <util/data_utilities.h>
#include <operators/keyed_join.h>
#include <operators/csv_input.h>
#include <operators/project.h>
#include <operators/sort.h>
#include <operators/sort_merge_aggregate.h>
#include <operators/filter.h>
#include <test/mpc/emp_base_test.h>

using namespace vaultdb;
using namespace emp;


// to load test data, run from src/main/cpp the following:
// bash test/support/load-generated-data.sh <tuple count>


DEFINE_int32(party, 1, "party for EMP execution");
DEFINE_int32(port, 7654, "port for EMP execution");
DEFINE_string(alice_host, "127.0.0.1", "hostname for execution");
DEFINE_string(unioned_db, "tpch_unioned_150", "unioned db name");
DEFINE_string(alice_db, "tpch_alice_150", "alice db name");
DEFINE_string(bob_db, "tpch_bob_150", "bob db name");
DEFINE_int32(ctrl_port, 65486, "port for managing EMP control flow by passing public values");
DEFINE_string(filter, "*", "run only the tests passing this filter");
DEFINE_string(storage, "column", "storage model for columns (column, wire_packed or compressed)");



class EnrichTest : public EmpBaseTest  {

protected:


    const string unioned_enrich_db_ = "enrich_htn_unioned";
    const string alice_enrich_db_ = "enrich_htn_test_alice";
    const string bob_enrich_db_ = "enrich_htn_test_bob";

    PlainTable *getAgeStrataProjection(PlainTable *input, const bool & isEncrypted) const;

    PlainTable *loadAndProjectPatientData(const string &dbName) const;

    PlainTable *loadPatientExclusionData(const string &dbName) const;

    PlainTable *loadAndJoinLocalData(const string &dbName) const;

    SecureTable *getPatientCohort();

    void validateTable(const string &db_name, const string &sql, const SortDefinition  & expectedSortDefinition, PlainTable *observedTable) const;


    static SecureTable *rollUpAggregate(const int & ordinal, SecureTable *src);

    SecureTable *loadUnionAndDeduplicateData() const;

    void validateUnion(Operator<emp::Bit> &sortOp, const SortDefinition &expected_sort) const;

    SecureTable *aggregatePatientData();


    SecureTable *filterPatients();

    std::string getRollupExpectedResultsSql(const std::string &group_by_col);
};


#endif //  _ENRICH_TEST_H
