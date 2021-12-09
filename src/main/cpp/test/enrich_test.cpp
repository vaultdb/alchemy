#include <test/support/enrich_test_support.h>
#include <data/psql_data_provider.h>
#include <operators/sql_input.h>
#include <gflags/gflags.h>
#include <expression/comparator_expression_nodes.h>
#include <expression/function_expression.h>
#include <expression/generic_expression.h>
#include <expression/bool_expression.h>

#include "enrich_test.h"




shared_ptr<PlainTable> EnrichTest::getAgeStrataProjection(shared_ptr<PlainTable> input, const bool & isEncrypted) const {
    FieldType ageStrataType = isEncrypted ? FieldType::SECURE_INT : FieldType::INT;

    ExpressionMapBuilder<bool> builder(*(input->getSchema()));

    shared_ptr<Expression<bool>> ageStrataExpression(new FunctionExpression<bool>(&EnrichTestSupport<bool>::projectAgeStrata, "age_strata", ageStrataType));
    builder.addExpression(ageStrataExpression, 2);

    for(uint32_t i = 0; i < 7; ++i)
        if(i != 2)
            builder.addMapping(i, i);


    Project project(input, builder.getExprs());

    return project.run();
}




// roll up one group-by col at a time
// input schema:
// zip_marker (0), age_strata (1), sex (2), ethnicity (3) , race (4), numerator_cnt (5), denominator_cnt (6), numerator_multisite (7), denominator_multisite (8)
std::shared_ptr<SecureTable> EnrichTest::rollUpAggregate(const int &ordinal, shared_ptr<SecureTable> src) {
    SortDefinition sortDefinition{ColumnSort(ordinal, SortDirection::ASCENDING)};
    Sort sort(src, sortDefinition);

    std::vector<int32_t> groupByCols{ordinal};
    // ordinals 0...4 are group-by cols in input schema
    std::vector<ScalarAggregateDefinition> aggregators {
            ScalarAggregateDefinition(5, AggregateId::SUM, "numerator_cnt"),
            ScalarAggregateDefinition(6, AggregateId::SUM, "denominator_cnt"),
            ScalarAggregateDefinition(7, AggregateId::SUM, "numerator_multisite"),
            ScalarAggregateDefinition(8, AggregateId::SUM, "denominator_multisite")
    };

    GroupByAggregate rollupStrata(&sort, groupByCols, aggregators );
    std::shared_ptr<SecureTable> rollupResult =  rollupStrata.run();
    return rollupResult;
}

// returns the data loaded from a single DB with age projection
// plaintext only
shared_ptr<PlainTable> EnrichTest::loadAndProjectPatientData(const std::string &dbName) const {

    SortDefinition patientSortDef{ColumnSort(0, SortDirection::ASCENDING)};
    string inputPatientSql = "SELECT * FROM patient ORDER BY patid";


    // *** Input on union of Alice and Bob's tables
    SqlInput patientInput(dbName, inputPatientSql, false, patientSortDef);


    // *** project on patient table
    shared_ptr<PlainTable> ageStrataProjection = getAgeStrataProjection(patientInput.run(), false);
    return ageStrataProjection;
}


// returns the data loaded from a single DB with age projection
// plaintext only
shared_ptr<PlainTable> EnrichTest::loadPatientExclusionData(const std::string &dbName) const {

    SortDefinition patientExclusionSortDef{ColumnSort(0, SortDirection::ASCENDING), ColumnSort (2, SortDirection::ASCENDING)};
    string inputPatientExclusionSql = "SELECT * FROM patient_exclusion ORDER BY patid, denom_excl";
    SqlInput input(dbName, inputPatientExclusionSql, false, patientExclusionSortDef);
    return input.run();
}

shared_ptr<PlainTable> EnrichTest::loadAndJoinLocalData(const std::string & dbName) const {
    shared_ptr<PlainTable> patientInput = loadAndProjectPatientData(dbName);
    shared_ptr<PlainTable> patientExclusionInput =  loadPatientExclusionData(dbName);

    //patientJoinPredicate.push_back(EqualityPredicate (0, 0)); //  patid = patid
    // mapping from (lhs_tuple[0], rhs_tuple[0])
    // lhs has 7 fields,
    uint32_t rhs_offset = patientInput->getSchema()->getFieldCount();
    BoolExpression<bool> predicate = Utilities::getEqualityPredicate<bool>(0, rhs_offset);
    KeyedJoin<bool> join(patientInput, patientExclusionInput, predicate);
    return join.run();
}

shared_ptr<SecureTable> EnrichTest::loadUnionAndDeduplicateData() const{
    string dbName = (FLAGS_party == ALICE) ? aliceDbName : bobDbName;
    shared_ptr<PlainTable>  localData = loadAndJoinLocalData(dbName);
    std::shared_ptr<SecureTable> unionedAndEncryptedData = PlainTable::secretShare(*localData, netio_, FLAGS_party);


    // TODO: do bitonic merge instead of full-fledged sort here.  Inputs are sorted locally and each side makes up half of a bitonic sequence
    // sort it on group-by cols to prepare for aggregate
    SortDefinition unionSortDefinition = DataUtilities::getDefaultSortDefinition(6);

    unionSortDefinition.push_back(ColumnSort(9, SortDirection::ASCENDING)); // last sort makes it verifiable

    Sort<emp::Bit> sortUnioned(unionedAndEncryptedData, unionSortDefinition);
    shared_ptr<PlainTable> result = sortUnioned.run()->reveal();


    validateUnion(sortUnioned, unionSortDefinition);

    // aggregate to complete the union
    //      p.patid, zip_marker, age_strata, sex, ethnicity, race, max(p.numerator) numerator, COUNT(*), max(denom_excl)
    // GROUP BY   p.patid, zip_marker, age_strata, sex, ethnicity, race
    std::vector<int32_t> groupByCols{0, 1, 2, 3, 4, 5};
    std::vector<ScalarAggregateDefinition> unionAggregators {
            ScalarAggregateDefinition(6, AggregateId::MAX, "numerator"),
            ScalarAggregateDefinition(-1, AggregateId::COUNT, "site_count"),
            ScalarAggregateDefinition(9, AggregateId::MAX, "denom_excl")
    };

    GroupByAggregate<emp::Bit> unionedPatients(&sortUnioned, groupByCols, unionAggregators );
    return unionedPatients.run();

}


shared_ptr<SecureTable> EnrichTest::filterPatients() {

    std::shared_ptr<SecureTable> deduplicatedPatients = loadUnionAndDeduplicateData();

    // *** Filter
    // HAVING max(denom_excl) = 0
    shared_ptr<ExpressionNode<emp::Bit> > zero(new LiteralNode<emp::Bit>(Field<emp::Bit>(FieldType::SECURE_INT, emp::Integer(32, 0))));;
    shared_ptr<ExpressionNode<emp::Bit> > input(new InputReferenceNode<emp::Bit>(8));
    shared_ptr<ExpressionNode<emp::Bit> > equality(new EqualNode<emp::Bit>(input, zero));

    BoolExpression<emp::Bit> equality_expr(equality);

    //std::shared_ptr<Predicate<emp::Bit> > predicateClass(new FilterExcludedPatients<emp::Bit>(true));
    Filter inclusionCohort(deduplicatedPatients, equality_expr);

    return  inclusionCohort.run();

}

shared_ptr<SecureTable> EnrichTest::getPatientCohort() {

    std::shared_ptr<SecureTable> inclusionCohort = filterPatients();

    // *** Project on aggregate outputs:
    //     CASE WHEN count(*) > 1 THEN 1 else 0 END AS multisite
    //    CASE WHEN MAX(numerator)=1 ^ COUNT(*) > 1 THEN 1 ELSE 0 END AS numerator_multisite
    // output schema:
    // zip_marker, age_strata, sex, ethnicity, race, max(p.numerator) numerator, COUNT(*) > 1, COUNT(*) > 1 ^ numerator
    ExpressionMapBuilder<emp::Bit> builder( *(inclusionCohort->getSchema()));
    for(uint32_t i = 1; i < 7; ++i)
        builder.addMapping(i, i-1);



/*    ProjectionMappingSet mappingSet{
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

    };*/

    shared_ptr<Expression<emp::Bit> > multisiteExpression(new FunctionExpression<emp::Bit>(&(EnrichTestSupport<emp::Bit>::projectMultisite), "multisite", FieldType::SECURE_INT));
    shared_ptr<Expression<emp::Bit> > multisiteNumeratorExpression(new FunctionExpression<emp::Bit>(&(EnrichTestSupport<emp::Bit>::projectNumeratorMultisite), "numerator_multisite", FieldType::SECURE_INT));
    //Expression multisiteExpression(&(EnrichTestSupport<emp::Bit>::projectMultisite), "multisite", FieldType::SECURE_INT);
    //Expression multisiteNumeratorExpression(&(EnrichTestSupport<emp::Bit>::projectNumeratorMultisite), "numerator_multisite", FieldType::SECURE_INT);

    builder.addExpression(multisiteExpression, 6);
    builder.addExpression(multisiteNumeratorExpression, 7);

    Project project(inclusionCohort, builder.getExprs());




    return project.run();

}

shared_ptr<SecureTable> EnrichTest::aggregatePatientData() {
    std::shared_ptr<SecureTable> includedPatients = getPatientCohort();

    // sort it on cols [0,5)
    Sort sort(includedPatients, DataUtilities::getDefaultSortDefinition(5));

    std::vector<int32_t> groupByCols{0, 1, 2, 3, 4};
    std::vector<ScalarAggregateDefinition> aggregators {
            ScalarAggregateDefinition(5, AggregateId::SUM, "numerator_cnt"),
            ScalarAggregateDefinition(-1, AggregateId::COUNT, "denominator_cnt"),
            ScalarAggregateDefinition(7, AggregateId::SUM, "numerator_multisite"),
            ScalarAggregateDefinition(6, AggregateId::SUM, "denominator_multisite")
    };


    // output schema:
    // zip_marker (0), age_strata (1), sex (2), ethnicity (3) , race (4), numerator_cnt (5), denominator_cnt (6), numerator_multisite (7), denominator_multisite (8)
    GroupByAggregate aggregatedPatients(&sort, groupByCols, aggregators );
    return aggregatedPatients.run();

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

void EnrichTest::validateUnion(Operator<emp::Bit> &sortOp, const SortDefinition &expectedSortOrder) const {
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

    std::shared_ptr<PlainTable> observedTable = sortOp.run()->reveal();

    validateTable(unionedDbName, expectedResultSql, expectedSortOrder, observedTable);

}


void EnrichTest::validateTable(const std::string & dbName, const std::string & sql, const SortDefinition  & expectedSortDefinition, const std::shared_ptr<PlainTable> & observedTable) const {

    PsqlDataProvider dataProvider;
    std::shared_ptr<PlainTable> expectedTable = dataProvider.getQueryTable(dbName, sql);
    expectedTable->setSortOrder(expectedSortDefinition);

    ASSERT_EQ(*expectedTable->getSchema(), *observedTable->getSchema());

    // check that the types are faithfully aligned
    for(size_t i = 0; i < observedTable->getSchema()->getFieldCount(); ++i) {
        FieldType schemaType = observedTable->getSchema()->getField(i).getType();
        FieldType instanceType = (*observedTable)[0][i].getType();
        FieldType expectedType = (*expectedTable)[0][i].getType();
        /*if(schemaType != instanceType) {
            std::cout << "Instance type not aligned! " << observedTable->get_schema().getField(i) << " expected: " << TypeUtilities::getTypeString(expectedType) << " received " << TypeUtilities::getTypeString(instanceType) << std::endl;
        }*/

        ASSERT_EQ(schemaType, instanceType);
        ASSERT_EQ(schemaType, expectedType);

    }

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
    std::shared_ptr<PlainTable> alicePatient = loadAndProjectPatientData(aliceDbName);
    // this is more of a smoke test to make sure the setup is aligned correctly, not testing query reading facility
    std::shared_ptr<PlainTable> alicePatientExclusion = loadPatientExclusionData(aliceDbName);

    validateTable(aliceDbName, expectedResultPatientSql, patientSortDef, alicePatient);
    validateTable(aliceDbName, expectedResultPatientExclusionSql, patientExclusionSortDef, alicePatientExclusion);


    // *** bob test
    std::shared_ptr<PlainTable> bobPatient = loadAndProjectPatientData(bobDbName);
    std::shared_ptr<PlainTable> bobPatientExclusion = loadPatientExclusionData(bobDbName);

    validateTable(bobDbName, expectedResultPatientSql, patientSortDef, bobPatient);
    validateTable(bobDbName, expectedResultPatientExclusionSql, patientExclusionSortDef, bobPatientExclusion);

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


    SortDefinition  expectedSort{ColumnSort(0, SortDirection::ASCENDING)};
    std::shared_ptr<PlainTable> aliceJoined = loadAndJoinLocalData(aliceDbName);
    validateTable(aliceDbName, expectedResultSql, expectedSort, aliceJoined);

    std::shared_ptr<PlainTable> bobJoined = loadAndJoinLocalData(bobDbName);
    validateTable(bobDbName, expectedResultSql, expectedSort, bobJoined);

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

    std::shared_ptr<SecureTable> deduplicated = loadUnionAndDeduplicateData();
    std::shared_ptr<PlainTable> observedTable = deduplicated->reveal();
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


    std::shared_ptr<SecureTable> filter = filterPatients();
    std::shared_ptr<PlainTable> observedTable = filter->reveal();
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


    std::shared_ptr<SecureTable> patientCohort = getPatientCohort();
    std::shared_ptr<PlainTable> observedTable = patientCohort->reveal();


    observedTable = DataUtilities::removeDummies(observedTable);
    //SortDefinition  sortDefinition = DataUtilities::getDefaultSortDefinition(5);
    SortDefinition empty;

    validateTable(unionedDbName, expectedResultSql, empty, observedTable);

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

  std::shared_ptr<SecureTable> aggregator = aggregatePatientData();
    std::shared_ptr<PlainTable> observedTable = aggregator->reveal();
    observedTable = DataUtilities::removeDummies(observedTable);


    validateTable(unionedDbName, expectedResultSql, DataUtilities::getDefaultSortDefinition(5), observedTable);

}



TEST_F(EnrichTest, testRollups) {
    std::shared_ptr<SecureTable> aggregator = aggregatePatientData();
    SortDefinition orderBy = DataUtilities::getDefaultSortDefinition(1);


    // TODO: make this more efficient by doing a single scan for multiple rollups
    // roll-ups:
    // age_strata (1)
    std::shared_ptr<PlainTable> ageStratified = rollUpAggregate(1, aggregator)->reveal();
    ageStratified = DataUtilities::removeDummies(ageStratified);
    std::string expectedOutputSql = getRollupExpectedResultsSql("age_strata");
    validateTable(unionedDbName, expectedOutputSql, orderBy, ageStratified);
    //std::cout << "Validated age strata" <<  *ageStratified << std::endl;

    // gender (2)
    std::shared_ptr<PlainTable> genderStratified = rollUpAggregate(2, aggregator)->reveal();
    genderStratified = DataUtilities::removeDummies(genderStratified);
    expectedOutputSql = getRollupExpectedResultsSql("sex");
    validateTable(unionedDbName, expectedOutputSql, orderBy, genderStratified);

    //std::cout << "Validated gender strata " << *genderStratified << std::endl;

    // ethnicity (3)
    std::shared_ptr<PlainTable> ethnicityStratified = rollUpAggregate(3, aggregator)->reveal();
    ethnicityStratified = DataUtilities::removeDummies(ethnicityStratified);
    expectedOutputSql = getRollupExpectedResultsSql("ethnicity");
    validateTable(unionedDbName, expectedOutputSql, orderBy, ethnicityStratified);
    //std::cout << "Validated ethnicity strata " << *ethnicityStratified << std::endl;

    // race (4)
    std::shared_ptr<PlainTable> raceStratified = rollUpAggregate(4, aggregator)->reveal();
    raceStratified = DataUtilities::removeDummies(raceStratified);
    expectedOutputSql = getRollupExpectedResultsSql("race");
    validateTable(unionedDbName, expectedOutputSql, orderBy, raceStratified);
    //std::cout << "Validated race strata " << *raceStratified << std::endl;


    // zip marker (0)
    std::shared_ptr<PlainTable> zipMarkerStratified = rollUpAggregate(0, aggregator)->reveal();
    zipMarkerStratified = DataUtilities::removeDummies(zipMarkerStratified);
    expectedOutputSql = getRollupExpectedResultsSql("zip_marker");
    validateTable(unionedDbName, expectedOutputSql, orderBy, zipMarkerStratified);
    //std::cout << "Validated zip marker stratified " << *zipMarkerStratified << std::endl;
}

// set up with 100 tuples:
// bash test/support/load-generated-data.sh 100
int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    gflags::ParseCommandLineFlags(&argc, &argv, false);

    return RUN_ALL_TESTS();
}

