#include <test/support/enrich_test_support.h>
#include <data/PsqlDataProvider.h>
#include <operators/sql_input.h>
#include <gflags/gflags.h>
#include <operators/common_table_expression.h>
#include "enrich_test.h"




std::shared_ptr<Operator> getAgeStrataProjection(std::shared_ptr<Operator> input, const bool & isEncrypted) {
    Project *projectOp = new Project(input);
    TypeId ageStrataType = isEncrypted ? TypeId::ENCRYPTED_INTEGER32 : TypeId::INTEGER32;

    Expression ageStrataExpression(&EnrichTestSupport::projectPlainAgeStrata, "age_strata", ageStrataType);
    ProjectionMappingSet mappingSet{
            ProjectionMapping(0, 0),
            ProjectionMapping(1, 1),
            ProjectionMapping(3, 3),
            ProjectionMapping(4, 4),
            ProjectionMapping(5, 5),
            ProjectionMapping(6, 6)
    };


   projectOp->addColumnMappings(mappingSet);
   projectOp->addExpression(ageStrataExpression, 2);


    return projectOp->getPtr();
}



/*
std::shared_ptr<Operator> EnrichTest::getPatientCohort(const string & srcPatientFile, const string & srcPatientInclusionFile, bool isEncrypted) {

    // *** Input on union of Alice and Bob's tables
    std::shared_ptr<Operator> patientInput(new CsvInput(srcPatientFile, EnrichTestSupport::getPatientSchema()));
    std::shared_ptr<Operator> patientInclusionInput(new CsvInput(srcPatientInclusionFile, EnrichTestSupport::getPatientInclusionSchema()));

    // sort on patid
    Sort *sortPatients = new Sort(DataUtilities::getDefaultSortDefinition(1), patientInput);
    Sort *sortPatientInclusion = new Sort(DataUtilities::getDefaultSortDefinition(1), patientInclusionInput);

    // *** project on unioned patient table
    std::shared_ptr<Operator> ageStrataProjection = getAgeStrataProjection(sortPatients->getPtr(), isEncrypted);

    // *** Join patients and patient_incl

    ConjunctiveEqualityPredicate patientJoinPredicate;
    patientJoinPredicate.push_back(EqualityPredicate (0, 0)); //  patid = patid
    std::shared_ptr<BinaryPredicate> patientJoinCriteria(new JoinEqualityPredicate(patientJoinPredicate, isEncrypted));
    KeyedJoin *joinOp = new KeyedJoin(patientJoinCriteria, ageStrataProjection, sortPatientInclusion->getPtr());




    // aggregate to complete the union
    //      p.patid, zip_marker, age_strata, sex, ethnicity, race, max(p.numerator) numerator, COUNT(*), max(denom_excl)
    // GROUP BY   p.patid, zip_marker, age_strata, sex, ethnicity, race
    std::vector<int32_t> groupByCols{0, 1, 2, 3, 4, 5};
    std::vector<ScalarAggregateDefinition> unionAggregators {
            ScalarAggregateDefinition(6, AggregateId::MAX, "numerator"),
            ScalarAggregateDefinition(-1, AggregateId::COUNT, "site_count"),
            ScalarAggregateDefinition(8, AggregateId::MAX, "denom_excl")
    };

    GroupByAggregate *unionedPatients = new GroupByAggregate(joinOp->getPtr(), groupByCols, unionAggregators );



    // *** Project on aggregate outputs:
    //     CASE WHEN count(*) > 1 THEN 1 else 0 END AS multisite
    //    CASE WHEN MAX(numerator)=1 ^ COUNT(*) > 1 THEN 1 ELSE 0 END AS numerator_multisite
    // output schema:
    // p.patid, zip_marker, age_strata, sex, ethnicity, race, max(p.numerator) numerator, COUNT(*) > 1, max(denom_excl) = 0
    std::shared_ptr<Operator> unionedProjection = getUnionedProjection(unionedPatients->getPtr(), isEncrypted);


    // *** Filter
    // HAVING max(denom_excl) = 0
    std::shared_ptr<Predicate> predicateClass(new FilterExcludedPatients(isEncrypted));
    Filter *exclusionCriteria = new Filter(predicateClass, unionedProjection->getPtr());

    return exclusionCriteria->getPtr();

}*/


// roll up one group-by col at a time
// input schema:
// zip_marker (0), age_strata (1), sex (2), ethnicity (3) , race (4), numerator_cnt (5), denominator_cnt (6), numerator_multisite (7), denominator_multisite (8)
std::shared_ptr<QueryTable> EnrichTest::rollUpAggregate(const int &ordinal, std::shared_ptr<Operator> src) {
    SortDefinition sortDefinition{ColumnSort(ordinal, SortDirection::ASCENDING)};
    Sort *sort = new Sort(sortDefinition, src);

    std::vector<int32_t> groupByCols{ordinal};
    // ordinals 0...4 are group-by cols in input schema
    std::vector<ScalarAggregateDefinition> aggregators {
            ScalarAggregateDefinition(5, AggregateId::SUM, "numerator_cnt"),
            ScalarAggregateDefinition(6, AggregateId::SUM, "denominator_cnt"),
            ScalarAggregateDefinition(7, AggregateId::SUM, "numerator_multisite"),
            ScalarAggregateDefinition(8, AggregateId::SUM, "denominator_multisite")
    };

    GroupByAggregate *rollupStrata = new GroupByAggregate(sort->getPtr(), groupByCols, aggregators );
    std::shared_ptr<QueryTable> rollupResult =  rollupStrata->getPtr()->run();
    return rollupResult;
}

// returns the data loaded from a single DB with age projection
// plaintext only
std::shared_ptr<Operator> EnrichTest::loadAndProjectPatientData(const std::string &dbName) const {

    SortDefinition patientSortDef{ColumnSort(0, SortDirection::ASCENDING)};
    string inputPatientSql = "SELECT * FROM patient ORDER BY patid";


    // *** Input on union of Alice and Bob's tables
    std::shared_ptr<Operator> patientInput(new SqlInput(dbName, inputPatientSql, false, patientSortDef));


    // *** project on patient table
    std::shared_ptr<Operator> ageStrataProjection = getAgeStrataProjection(patientInput, false);
    return ageStrataProjection;
}


// returns the data loaded from a single DB with age projection
// plaintext only
std::shared_ptr<Operator> EnrichTest::loadPatientExclusionData(const std::string &dbName) const {

    SortDefinition patientExclusionSortDef{ColumnSort(0, SortDirection::ASCENDING), ColumnSort (2, SortDirection::ASCENDING)};
    string inputPatientExclusionSql = "SELECT * FROM patient_exclusion ORDER BY patid, denom_excl";
    SqlInput *input = new SqlInput(dbName, inputPatientExclusionSql, false, patientExclusionSortDef);
    return input->getPtr();
}

std::shared_ptr<Operator> EnrichTest::loadAndJoinLocalData(const std::string & dbName) const {
    std::shared_ptr<Operator> patientInput = loadAndProjectPatientData(dbName);
    std::shared_ptr<Operator> patientExclusionInput =  loadPatientExclusionData(dbName);

    ConjunctiveEqualityPredicate patientJoinPredicate;
    patientJoinPredicate.push_back(EqualityPredicate (0, 0)); //  patid = patid
    std::shared_ptr<BinaryPredicate> patientJoinCriteria(new JoinEqualityPredicate(patientJoinPredicate, false));
    KeyedJoin *joinOp = new KeyedJoin(patientJoinCriteria, patientInput, patientExclusionInput);
    return joinOp->getPtr();
}

std::shared_ptr<Operator> EnrichTest::loadUnionAndDeduplicateData() const{
    string dbName = (FLAGS_party == ALICE) ? aliceDbName : bobDbName;
    std::shared_ptr<Operator> inputData = loadAndJoinLocalData(dbName);
    std::shared_ptr<QueryTable> localData = inputData->run();
    std::shared_ptr<QueryTable> unionedAndEncryptedData = localData->secretShare(netio, FLAGS_party);

    CommonTableExpression *unionedData = new CommonTableExpression(unionedAndEncryptedData);

    // TODO: do bitonic merge instead of full-fledged sort here.  Inputs are sorted locally and each side makes up half of a bitonic sequence
    // sort it on group-by cols to prepare for aggregate
    SortDefinition unionSortDefinition = DataUtilities::getDefaultSortDefinition(6);
    unionSortDefinition.push_back(ColumnSort(9, SortDirection::ASCENDING)); // last sort makes it verifiable
    Sort *sortUnioned = new Sort(unionSortDefinition, unionedData->getPtr());
    validateUnion(sortUnioned->getPtr(), unionSortDefinition);


    // aggregate to complete the union
    //      p.patid, zip_marker, age_strata, sex, ethnicity, race, max(p.numerator) numerator, COUNT(*), max(denom_excl)
    // GROUP BY   p.patid, zip_marker, age_strata, sex, ethnicity, race
    std::vector<int32_t> groupByCols{0, 1, 2, 3, 4, 5};
    std::vector<ScalarAggregateDefinition> unionAggregators {
            ScalarAggregateDefinition(6, AggregateId::MAX, "numerator"),
            ScalarAggregateDefinition(-1, AggregateId::COUNT, "site_count"),
            ScalarAggregateDefinition(9, AggregateId::MAX, "denom_excl")
    };

    GroupByAggregate *unionedPatients = new GroupByAggregate(sortUnioned->getPtr(), groupByCols, unionAggregators );
    return unionedPatients->getPtr();

}


std::shared_ptr<Operator> EnrichTest::filterPatients() {

    std::shared_ptr<Operator> deduplicatedPatients = loadUnionAndDeduplicateData();

    // *** Filter
    // HAVING max(denom_excl) = 0
    std::shared_ptr<Predicate> predicateClass(new FilterExcludedPatients(true));
    Filter *inclusionCohort = new Filter(predicateClass, deduplicatedPatients->getPtr());

    return  inclusionCohort->getPtr();



}
std::shared_ptr<Operator> EnrichTest::getPatientCohort() {

    std::shared_ptr<Operator> inclusionCohort = filterPatients();

    // *** Project on aggregate outputs:
    //     CASE WHEN count(*) > 1 THEN 1 else 0 END AS multisite
    //    CASE WHEN MAX(numerator)=1 ^ COUNT(*) > 1 THEN 1 ELSE 0 END AS numerator_multisite
    // output schema:
    // zip_marker, age_strata, sex, ethnicity, race, max(p.numerator) numerator, COUNT(*) > 1, COUNT(*) > 1 ^ numerator
    Project *projectOp = new Project(inclusionCohort);
    TypeId intType = TypeId::ENCRYPTED_INTEGER32;

    ProjectionMappingSet mappingSet{
            // zip_marker
            ProjectionMapping(1, 0),
            // age_strata
            ProjectionMapping(2, 1),
            // sex
            ProjectionMapping(3, 2),
            // ethnicity
            ProjectionMapping(4, 3),
            // race
            ProjectionMapping(5, 4),
            // numerator
            ProjectionMapping(6, 5),


            // 7: multisite int
            // 8: multisite ^ numerator

    };

    Expression multisiteExpression(&EnrichTestSupport::projectMultisite, "multisite", intType);
    Expression multisiteNumeratorExpression(&EnrichTestSupport::projectNumeratorMultisite, "numerator_multisite", intType);

    projectOp->addColumnMappings(mappingSet);
    projectOp->addExpression(multisiteExpression, 6);
    projectOp->addExpression(multisiteNumeratorExpression, 7);

    return projectOp->getPtr();

}

std::shared_ptr<Operator> EnrichTest::aggregatePatientData() {
    std::shared_ptr<Operator> includedPatients = getPatientCohort();

    // sort it on cols [0,5)
    Sort *sortOp = new Sort(DataUtilities::getDefaultSortDefinition(5), includedPatients);

    std::vector<int32_t> groupByCols{0, 1, 2, 3, 4};
    std::vector<ScalarAggregateDefinition> aggregators {
            ScalarAggregateDefinition(5, AggregateId::SUM, "numerator_cnt"),
            ScalarAggregateDefinition(-1, AggregateId::COUNT, "denominator_cnt"),
            ScalarAggregateDefinition(7, AggregateId::SUM, "numerator_multisite"),
            ScalarAggregateDefinition(6, AggregateId::SUM, "denominator_multisite")
    };


    // output schema:
    // zip_marker (0), age_strata (1), sex (2), ethnicity (3) , race (4), numerator_cnt (5), denominator_cnt (6), numerator_multisite (7), denominator_multisite (8)
    GroupByAggregate *aggregatedPatients = new GroupByAggregate(sortOp->getPtr(), groupByCols, aggregators );
    return aggregatedPatients->getPtr();

}


/**** verification methods ****/

std::string EnrichTest::getRollupExpectedResultsSql(const std::string &groupByColName) {
    std::string expectedResultSql = "    WITH labeled as (\n"
                                    "        SELECT patid, zip_marker, CASE WHEN age_days <= 28*365 THEN 0\n"
                                    "                WHEN age_days > 28*365 AND age_days <= 39*365 THEN 1\n"
                                    "              WHEN age_days > 39*365  AND age_days <= 50*365 THEN 2\n"
                                    "              WHEN age_days > 50*365 AND age_days <= 61*365 THEN 3\n"
                                    "              WHEN age_days > 61*365 AND age_days <= 72*365 THEN 4\n"
                                    "                WHEN age_days > 72*365  AND age_days <= 83*365 THEN 5\n"
                                    "                ELSE 6 END age_strata,\n"
                                    "            sex, ethnicity, race, numerator, site_id \n"
                                    "        FROM patient\n"
                                    "        ORDER BY patid), \n"
                                    "  deduplicated AS ("
                                    "    SELECT p.patid, zip_marker, age_strata, sex, ethnicity, race, MAX(p.numerator) numerator, COUNT(*) cnt\n"
                                    "    FROM labeled p JOIN patient_exclusion pe ON p.patid = pe.patid AND p.site_id = pe.site_id\n"
                                    "    GROUP BY p.patid, zip_marker, age_strata, sex, ethnicity, race \n"
                                    "    HAVING MAX(denom_excl) = 0 \n"
                                    "    ORDER BY p.patid, zip_marker, age_strata, sex, ethnicity, race ),  \n"
                                    "  data_cube AS (SELECT  zip_marker, age_strata, sex, ethnicity, race, SUM(numerator) numerator, COUNT(*) denominator, SUM((numerator = 1 AND cnt> 1)::INT) numerator_multisite, SUM(CASE WHEN cnt > 1 THEN 1 else 0 END)  denominator_multisite \n"
                                    "  FROM deduplicated \n"
                                    "  GROUP BY zip_marker, age_strata, sex, ethnicity, race \n"
                                    "  ORDER BY zip_marker, age_strata, sex, ethnicity, race ) \n";

    // JMR: not clear why we need to cast to BIGINT here, comes out as float by default despite inputs being BIGINT
    expectedResultSql += "SELECT " + groupByColName + ", SUM(numerator)::BIGINT numerator, SUM(denominator)::BIGINT denominator, SUM(numerator_multisite)::BIGINT numerator_multisite, SUM(denominator_multisite)::BIGINT denominator_multisite \n";
    expectedResultSql += " FROM data_cube \n"
                         " GROUP BY " + groupByColName + " \n"
                                                         " ORDER BY " + groupByColName;

    return expectedResultSql;

}

void EnrichTest::validateUnion(std::shared_ptr<Operator> sortOp, const SortDefinition &expectedSortOrder) const {
    std::string expectedResultSql = "    WITH labeled as (\n"
                                    "        SELECT patid, zip_marker, CASE WHEN age_days <= 28*365 THEN 0\n"
                                    "                WHEN age_days > 28*365 AND age_days <= 39*365 THEN 1\n"
                                    "              WHEN age_days > 39*365  AND age_days <= 50*365 THEN 2\n"
                                    "              WHEN age_days > 50*365 AND age_days <= 61*365 THEN 3\n"
                                    "              WHEN age_days > 61*365 AND age_days <= 72*365 THEN 4\n"
                                    "                WHEN age_days > 72*365  AND age_days <= 83*365 THEN 5\n"
                                    "                ELSE 6 END age_strata,\n"
                                    "            sex, ethnicity, race, numerator, site_id\n"
                                    "        FROM patient\n"
                                    "        ORDER BY patid)\n"
                                    "    SELECT p.patid, zip_marker, age_strata, sex, ethnicity, race, p.numerator, pe.patid, pe.numerator n2, pe.denom_excl \n"
                                    "    FROM labeled p JOIN patient_exclusion pe ON p.patid = pe.patid AND p.site_id = pe.site_id"
                                    "    ORDER BY p.patid, zip_marker, age_strata, sex, ethnicity, race, denom_excl";

    std::shared_ptr<QueryTable> observedTable = sortOp->run()->reveal();
    validateTable(unionedDbName, expectedResultSql, expectedSortOrder, observedTable);

}


void EnrichTest::validateTable(const std::string & dbName, const std::string & sql, const SortDefinition  & expectedSortDefinition, const std::shared_ptr<QueryTable> & observedTable) const {

    PsqlDataProvider dataProvider;
    std::shared_ptr<QueryTable> expectedTable = dataProvider.getQueryTable(dbName, sql);
    expectedTable->setSortOrder(expectedSortDefinition);

    ASSERT_EQ(*expectedTable, *observedTable);

}

/**** end verification methods ****/



TEST_F(EnrichTest, loadAndPrepData) {
    string expectedResultPatientSql = "       SELECT patid, zip_marker, CASE WHEN age_days <= 28*365 THEN 0\n"
                                      "                WHEN age_days > 28*365 AND age_days <= 39*365 THEN 1\n"
                                      "              WHEN age_days > 39*365  AND age_days <= 50*365 THEN 2\n"
                                      "              WHEN age_days > 50*365 AND age_days <= 61*365 THEN 3\n"
                                      "              WHEN age_days > 61*365 AND age_days <= 72*365 THEN 4\n"
                                      "                WHEN age_days > 72*365  AND age_days <= 83*365 THEN 5\n"
                                      "                ELSE 6 END age_strata,\n"
                                      "            sex, ethnicity, race, numerator \n"
                                      "FROM patient \n"
                                      "ORDER BY patid";

    string expectedResultPatientExclusionSql = "SELECT * FROM patient_exclusion ORDER BY patid, denom_excl";
    SortDefinition patientSortDef{ColumnSort(0, SortDirection::ASCENDING)};
    SortDefinition patientExclusionSortDef{ColumnSort(0, SortDirection::ASCENDING), ColumnSort(2, SortDirection::ASCENDING)};

    // *** alice test
    std::shared_ptr<Operator> alicePatient = loadAndProjectPatientData(aliceDbName);
    // this is more of a smoke test to make sure the setup is aligned correctly, not testing query reading facility
    std::shared_ptr<Operator> alicePatientExclusion = loadPatientExclusionData(aliceDbName);

    validateTable(aliceDbName, expectedResultPatientSql, patientSortDef, alicePatient->run());
    validateTable(aliceDbName, expectedResultPatientExclusionSql, patientExclusionSortDef, alicePatientExclusion->run());


    // *** bob test
    std::shared_ptr<Operator> bobPatient = loadAndProjectPatientData(bobDbName);
    std::shared_ptr<Operator> bobPatientExclusion = loadPatientExclusionData(bobDbName);

    validateTable(bobDbName, expectedResultPatientSql, patientSortDef, bobPatient->run());
    validateTable(bobDbName, expectedResultPatientExclusionSql, patientExclusionSortDef, bobPatientExclusion->run());

}



// need to refactor this to do the join before unioning Alice and Bob's inputs
TEST_F(EnrichTest, loadAndJoinData) {

    string expectedResultSql = "       SELECT p.patid, zip_marker, CASE WHEN age_days <= 28*365 THEN 0\n"
                                      "                WHEN age_days > 28*365 AND age_days <= 39*365 THEN 1\n"
                                      "              WHEN age_days > 39*365  AND age_days <= 50*365 THEN 2\n"
                                      "              WHEN age_days > 50*365 AND age_days <= 61*365 THEN 3\n"
                                      "              WHEN age_days > 61*365 AND age_days <= 72*365 THEN 4\n"
                                      "                WHEN age_days > 72*365  AND age_days <= 83*365 THEN 5\n"
                                      "                ELSE 6 END age_strata,\n"
                                      "            sex, ethnicity, race, p.numerator, pe.patid, pe.numerator n2, denom_excl \n"
                                      "FROM patient p JOIN patient_exclusion pe ON p.patid = pe.patid \n"
                                      "ORDER BY p.patid, denom_excl";


    SortDefinition emptySort; // leaving this empty for now, TODO: propagate sort trait through join op

    std::shared_ptr<Operator> aliceJoined = loadAndJoinLocalData(aliceDbName);
    validateTable(aliceDbName, expectedResultSql, emptySort, aliceJoined->run());

    std::shared_ptr<Operator> bobJoined = loadAndJoinLocalData(bobDbName);
    validateTable(bobDbName, expectedResultSql, emptySort, bobJoined->run());

}



TEST_F(EnrichTest, loadUnionAndDeduplicateData) {

    std::string expectedResultSql = "    WITH labeled as (\n"
                                    "        SELECT patid, zip_marker, CASE WHEN age_days <= 28*365 THEN 0\n"
                                    "                WHEN age_days > 28*365 AND age_days <= 39*365 THEN 1\n"
                                    "              WHEN age_days > 39*365  AND age_days <= 50*365 THEN 2\n"
                                    "              WHEN age_days > 50*365 AND age_days <= 61*365 THEN 3\n"
                                    "              WHEN age_days > 61*365 AND age_days <= 72*365 THEN 4\n"
                                    "                WHEN age_days > 72*365  AND age_days <= 83*365 THEN 5\n"
                                    "                ELSE 6 END age_strata,\n"
                                    "            sex, ethnicity, race, numerator, site_id\n"
                                    "        FROM patient\n"
                                    "        ORDER BY patid)\n"
                                    "    SELECT p.patid, zip_marker, age_strata, sex, ethnicity, race, max(p.numerator) numerator, COUNT(*) cnt, max(denom_excl) denom_excl\n"
                                    "    FROM labeled p JOIN patient_exclusion pe ON p.patid = pe.patid AND p.site_id = pe.site_id\n"
                                    "    GROUP BY p.patid, zip_marker, age_strata, sex, ethnicity, race "
                                    "    ORDER BY p.patid, zip_marker, age_strata, sex, ethnicity, race ";

    std::shared_ptr<Operator> deduplicator = loadUnionAndDeduplicateData();
    std::shared_ptr<QueryTable> observedTable = deduplicator->run()->reveal();
    std::cout << "Observed table: " << observedTable->toString(true) << std::endl;
    observedTable = DataUtilities::removeDummies(observedTable);

    validateTable(unionedDbName, expectedResultSql, DataUtilities::getDefaultSortDefinition(6), observedTable);


}

TEST_F(EnrichTest, testExclusionCriteria) {
    std::string expectedResultSql = "    WITH labeled as (\n"
                                    "        SELECT patid, zip_marker, CASE WHEN age_days <= 28*365 THEN 0\n"
                                    "                WHEN age_days > 28*365 AND age_days <= 39*365 THEN 1\n"
                                    "              WHEN age_days > 39*365  AND age_days <= 50*365 THEN 2\n"
                                    "              WHEN age_days > 50*365 AND age_days <= 61*365 THEN 3\n"
                                    "              WHEN age_days > 61*365 AND age_days <= 72*365 THEN 4\n"
                                    "                WHEN age_days > 72*365  AND age_days <= 83*365 THEN 5\n"
                                    "                ELSE 6 END age_strata,\n"
                                    "            sex, ethnicity, race, numerator, site_id \n"
                                    "        FROM patient\n"
                                    "        ORDER BY patid)\n"
                                    "    SELECT p.patid, zip_marker, age_strata, sex, ethnicity, race, MAX(p.numerator) numerator, COUNT(*) cnt, MAX(denom_excl) denom_excl\n"
                                    "    FROM labeled p JOIN patient_exclusion pe ON p.patid = pe.patid AND p.site_id = pe.site_id\n"
                                    "    GROUP BY p.patid, zip_marker, age_strata, sex, ethnicity, race \n"
                                    "    HAVING MAX(denom_excl) = 0 \n"
                                    "    ORDER BY p.patid, zip_marker, age_strata, sex, ethnicity, race ";


    std::shared_ptr<Operator> filter = filterPatients();
    std::shared_ptr<QueryTable> observedTable = filter->run()->reveal();
    observedTable = DataUtilities::removeDummies(observedTable);

    validateTable(unionedDbName, expectedResultSql, DataUtilities::getDefaultSortDefinition(6), observedTable);

}


TEST_F(EnrichTest, testPatientCohort) {

    std::string expectedResultSql = "    WITH labeled as (\n"
                                    "        SELECT patid, zip_marker, CASE WHEN age_days <= 28*365 THEN 0\n"
                                    "                WHEN age_days > 28*365 AND age_days <= 39*365 THEN 1\n"
                                    "              WHEN age_days > 39*365  AND age_days <= 50*365 THEN 2\n"
                                    "              WHEN age_days > 50*365 AND age_days <= 61*365 THEN 3\n"
                                    "              WHEN age_days > 61*365 AND age_days <= 72*365 THEN 4\n"
                                    "                WHEN age_days > 72*365  AND age_days <= 83*365 THEN 5\n"
                                    "                ELSE 6 END age_strata,\n"
                                    "            sex, ethnicity, race, numerator, site_id \n"
                                    "        FROM patient\n"
                                    "        ORDER BY patid), \n"
                                    "  deduplicated AS ("
                                    "    SELECT p.patid, zip_marker, age_strata, sex, ethnicity, race, MAX(p.numerator) numerator, COUNT(*) cnt, MAX(denom_excl) denom_excl\n"
                                    "    FROM labeled p JOIN patient_exclusion pe ON p.patid = pe.patid AND p.site_id = pe.site_id\n"
                                    "    GROUP BY p.patid, zip_marker, age_strata, sex, ethnicity, race \n"
                                    "    HAVING MAX(denom_excl) = 0 \n"
                                    "    ORDER BY p.patid, zip_marker, age_strata, sex, ethnicity, race ) \n"
                                    "  SELECT  zip_marker, age_strata, sex, ethnicity, race, numerator, CASE WHEN cnt > 1 THEN 1 else 0 END AS multisite, (numerator = 1 AND cnt> 1)::INT numerator_multisite \n"
                                    "  FROM deduplicated";


    std::shared_ptr<Operator> patientCohortOp = getPatientCohort();
    std::shared_ptr<QueryTable> observedTable = patientCohortOp->run()->reveal();
    observedTable = DataUtilities::removeDummies(observedTable);
    // empty sort definition, first column in prior sort is no longer in play
    SortDefinition  emptySort;


    validateTable(unionedDbName, expectedResultSql, emptySort, observedTable);

}

TEST_F(EnrichTest, testPatientAgggregation) {
    std::string expectedResultSql = "    WITH labeled as (\n"
                                    "        SELECT patid, zip_marker, CASE WHEN age_days <= 28*365 THEN 0\n"
                                    "                WHEN age_days > 28*365 AND age_days <= 39*365 THEN 1\n"
                                    "              WHEN age_days > 39*365  AND age_days <= 50*365 THEN 2\n"
                                    "              WHEN age_days > 50*365 AND age_days <= 61*365 THEN 3\n"
                                    "              WHEN age_days > 61*365 AND age_days <= 72*365 THEN 4\n"
                                    "                WHEN age_days > 72*365  AND age_days <= 83*365 THEN 5\n"
                                    "                ELSE 6 END age_strata,\n"
                                    "            sex, ethnicity, race, numerator, site_id \n"
                                    "        FROM patient\n"
                                    "        ORDER BY patid), \n"
                                    "  deduplicated AS ("
                                    "    SELECT p.patid, zip_marker, age_strata, sex, ethnicity, race, MAX(p.numerator) numerator, COUNT(*) cnt\n"
                                    "    FROM labeled p JOIN patient_exclusion pe ON p.patid = pe.patid AND p.site_id = pe.site_id\n"
                                    "    GROUP BY p.patid, zip_marker, age_strata, sex, ethnicity, race \n"
                                    "    HAVING MAX(denom_excl) = 0 \n"
                                    "    ORDER BY p.patid, zip_marker, age_strata, sex, ethnicity, race ) \n"
                                    "  SELECT  zip_marker, age_strata, sex, ethnicity, race, SUM(numerator), COUNT(*) denominator, SUM((numerator = 1 AND cnt> 1)::INT) numerator_multisite, SUM(CASE WHEN cnt > 1 THEN 1 else 0 END)  multisite \n"
                                    "  FROM deduplicated \n"
                                    "  GROUP BY zip_marker, age_strata, sex, ethnicity, race \n"
                                    "  ORDER BY zip_marker, age_strata, sex, ethnicity, race ";

  std::shared_ptr<Operator> aggregator = aggregatePatientData();
    std::shared_ptr<QueryTable> observedTable = aggregator->run()->reveal();
    observedTable = DataUtilities::removeDummies(observedTable);


    validateTable(unionedDbName, expectedResultSql, DataUtilities::getDefaultSortDefinition(5), observedTable);

}



TEST_F(EnrichTest, testRollups) {
    std::shared_ptr<Operator> aggregator = aggregatePatientData();
    SortDefinition orderBy = DataUtilities::getDefaultSortDefinition(1);


    // TODO: make this more efficient by doing a single scan for multiple rollups
    // roll-ups:
    // age_strata (1)
    std::shared_ptr<QueryTable> ageStratified = rollUpAggregate(1, aggregator)->reveal();
    ageStratified = DataUtilities::removeDummies(ageStratified);
    std::string expectedOutputSql = getRollupExpectedResultsSql("age_strata");
    validateTable(unionedDbName, expectedOutputSql, orderBy, ageStratified);
    std::cout << "Validated age strata" <<  *ageStratified << std::endl;

    // gender (2)
    std::shared_ptr<QueryTable> genderStratified = rollUpAggregate(2, aggregator)->reveal();
    genderStratified = DataUtilities::removeDummies(genderStratified);
    expectedOutputSql = getRollupExpectedResultsSql("sex");
    validateTable(unionedDbName, expectedOutputSql, orderBy, genderStratified);

    std::cout << "Validated gender strata " << *genderStratified << std::endl;

    // ethnicity (3)
    std::shared_ptr<QueryTable> ethnicityStratified = rollUpAggregate(3, aggregator)->reveal();
    ethnicityStratified = DataUtilities::removeDummies(ethnicityStratified);
    expectedOutputSql = getRollupExpectedResultsSql("ethnicity");
    validateTable(unionedDbName, expectedOutputSql, orderBy, ethnicityStratified);
    std::cout << "Validated ethnicity strata " << *ethnicityStratified << std::endl;

    // race (4)
    std::shared_ptr<QueryTable> raceStratified = rollUpAggregate(4, aggregator)->reveal();
    raceStratified = DataUtilities::removeDummies(raceStratified);
    expectedOutputSql = getRollupExpectedResultsSql("race");
    validateTable(unionedDbName, expectedOutputSql, orderBy, raceStratified);
    std::cout << "Validated race strata " << *raceStratified << std::endl;


    // zip marker (0)
    std::shared_ptr<QueryTable> zipMarkerStratified = rollUpAggregate(0, aggregator)->reveal();
    zipMarkerStratified = DataUtilities::removeDummies(zipMarkerStratified);
    expectedOutputSql = getRollupExpectedResultsSql("zip_marker");
    validateTable(unionedDbName, expectedOutputSql, orderBy, zipMarkerStratified);
    std::cout << "Validated zip marker stratified " << *zipMarkerStratified << std::endl;
}


int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    gflags::ParseCommandLineFlags(&argc, &argv, false);

    return RUN_ALL_TESTS();
}
