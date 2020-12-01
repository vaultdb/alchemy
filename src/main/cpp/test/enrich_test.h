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
#include "support/EmpBaseTest.h"

DEFINE_int32(party, 1, "party for EMP execution");
DEFINE_int32(port, 54321, "port for EMP execution");
DEFINE_string(alice_host, "127.0.0.1", "alice hostname for execution");

using namespace vaultdb;
using namespace vaultdb::types;
using namespace emp;

class EnrichTest : public EmpBaseTest {

protected:
    void SetUp() override {

        std::string currentWorkingDirectory = DataUtilities::getCurrentWorkingDirectory();
        std::string partyStr = (FLAGS_party == ALICE) ? "alice" : "bob";

        srcPatientFile = currentWorkingDirectory + "/test/support/enrich-input/" + partyStr + "-patient.csv";
        srcPatientExclusionFile = currentWorkingDirectory + "/test/support/enrich-input/" + partyStr + "-patient-inclusion.csv";
    };
    void TearDown() override{};
    QuerySchema getPatientSchema();
    QuerySchema getPatientInclusionSchema();
    std::string srcPatientFile;
    std::string srcPatientExclusionFile;


    std::shared_ptr<Operator>
    getPatientCohort(const string &srcPatientFile, const string &srcPatientInclusionFile, bool isEncrypted);

    std::shared_ptr<QueryTable>
    rollUpAggregate(const int & ordinal, std::shared_ptr<Operator> src);

};


#endif _ENRICH_TEST_H
