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
#include <_deps/googletest-src/googletest/include/gtest/gtest.h>



using namespace vaultdb;
using namespace vaultdb::types;
using namespace emp;

// plaintext only test
// to load test data, run from src/main/cpp the following:
// bash test/support/load-generated-data.sh

class EnrichTest :  public ::testing::Test  {

protected:
    void SetUp() override {
    };
    void TearDown() override{};



    std::shared_ptr<Operator>
    getPatientCohort(const string &srcPatientFile, const string &srcPatientInclusionFile, bool isEncrypted);

    static std::shared_ptr<QueryTable>
    rollUpAggregate(const int & ordinal, std::shared_ptr<Operator> src);

    const string unionedDbName = "enrich_htn_unioned";
    const string aliceDbName = "enrich_htn_alice";
    const string bobDbName = "enrich_htn_bob";

    std::shared_ptr<Operator> loadAndProjectPatientData(const string &dbName) const;

    std::shared_ptr<Operator> loadPatientExclusionData(const string &dbName) const;

    std::shared_ptr<Operator> loadAndJoinLocalData(const string &dbName) const;

    void validateTable(const string &dbName, const string &sql, const SortDefinition  & expectedSortDefinition, const std::shared_ptr<QueryTable> &observedTable) const;

};


#endif _ENRICH_TEST_H
