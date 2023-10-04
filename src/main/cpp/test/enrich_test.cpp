#include <test/support/enrich_test_support.h>
#include <data/psql_data_provider.h>
#include <operators/sql_input.h>
#include <gflags/gflags.h>
#include <expression/comparator_expression_nodes.h>
#include <expression/function_expression.h>
#include <expression/generic_expression.h>

#include "enrich_test.h"
#include "util/field_utilities.h"



PlainTable *EnrichTest::getAgeStrataProjection(PlainTable *input, const bool & is_secret_shared) const {
    FieldType age_strata_type = is_secret_shared ? FieldType::SECURE_INT : FieldType::INT;

    ExpressionMapBuilder<bool> builder(input->getSchema());

    Expression<bool> *age_strata_expr = new FunctionExpression<bool>(&EnrichTestSupport<bool>::projectAgeStrata, &EnrichTestSupport<bool>::projectAgeStrataTable, "age_strata", age_strata_type);
    builder.addExpression(age_strata_expr, 2);

    for(uint32_t i = 0; i < 7; ++i)
        if(i != 2)
            builder.addMapping(i, i);


    Project project(input, builder.getExprs());

    return project.run()->clone();
}




// roll up one group-by col at a time
// input schema:
// zip_marker (0), age_strata (1), sex (2), ethnicity (3) , race (4), numerator_cnt (5), denominator_cnt (6), numerator_multisite (7), denominator_multisite (8)
SecureTable *EnrichTest::rollUpAggregate(const int &ordinal, SecureTable *src) {
    SortDefinition sortDefinition{ColumnSort(ordinal, SortDirection::ASCENDING)};
    auto sort = new Sort(src, sortDefinition);

    std::vector<int32_t> group_bys{ordinal};
    // ordinals 0...4 are group-by cols in input schema
    std::vector<ScalarAggregateDefinition> aggregators {
            ScalarAggregateDefinition(5, AggregateId::SUM, "numerator_cnt"),
            ScalarAggregateDefinition(6, AggregateId::SUM, "denominator_cnt"),
            ScalarAggregateDefinition(7, AggregateId::SUM, "numerator_multisite"),
            ScalarAggregateDefinition(8, AggregateId::SUM, "denominator_multisite")
    };

    SortMergeAggregate rollup_strata(sort, group_bys, aggregators );
    return rollup_strata.run()->clone();
}

// returns the data loaded from a single DB with age projection
// plaintext only
PlainTable *EnrichTest::loadAndProjectPatientData(const std::string &db_name) const {

    SortDefinition pat_sort_def{ColumnSort(0, SortDirection::ASCENDING)};
    string pat_sql = "SELECT * FROM patient ORDER BY patid";


    // *** Input on union of Alice and Bob's tables
    SqlInput pat_input(db_name, pat_sql, false, pat_sort_def);
    PlainTable *plain = pat_input.run()->clone(); // freed by projection

    // *** project on patient table
    return getAgeStrataProjection(plain, false);

}


// returns the data loaded from a single DB with age projection
// plaintext only
PlainTable *EnrichTest::loadPatientExclusionData(const std::string &db_name) const {

    SortDefinition pat_excl_sort_def{ColumnSort(0, SortDirection::ASCENDING), ColumnSort (2, SortDirection::ASCENDING)};
    string pat_excl_sql = "SELECT * FROM patient_exclusion ORDER BY patid, denom_excl";
    SqlInput input(db_name, pat_excl_sql, false, pat_excl_sort_def);
    PlainTable  *plain = input.run(); // p is deleted when input goes out of scope
    return plain->clone();
}

PlainTable *EnrichTest::loadAndJoinLocalData(const std::string & db_name) const {
    PlainTable *pat = loadAndProjectPatientData(db_name);
    PlainTable *pat_excl =  loadPatientExclusionData(db_name);

    //patientJoinPredicate.push_back(EqualityPredicate (0, 0)); //  patid = patid
    // mapping from (lhs_tuple[0], rhs_tuple[0])
    // lhs has 7 fields,
    uint32_t rhs_offset = pat->getSchema().getFieldCount();
    Expression<bool> *predicate = FieldUtilities::getEqualityPredicate<bool>(0, pat->getSchema(), rhs_offset,
                                                                             pat_excl->getSchema());
    KeyedJoin<bool> join(pat, pat_excl, predicate);
    return join.run()->clone();
}

SecureTable *EnrichTest::loadUnionAndDeduplicateData() const{
    string db_name = (FLAGS_party == ALICE) ? alice_enrich_db_ : bob_enrich_db_;
    PlainTable *local_data = loadAndJoinLocalData(db_name);
    SecureTable *union_and_secret_share = local_data->secretShare();


    // TODO: do bitonic merge instead of full-fledged sort here.  Inputs are sorted locally and each side makes up half of a bitonic sequence
    // sort it on group-by cols to prepare for aggregate
    SortDefinition union_sort_def = DataUtilities::getDefaultSortDefinition(6);

    union_sort_def.push_back(ColumnSort(9, SortDirection::ASCENDING)); // last sort makes it verifiable

    auto sort_unioned = new Sort<emp::Bit>(union_and_secret_share, union_sort_def);


    validateUnion(*sort_unioned, union_sort_def);

    // aggregate to complete the union
    //      p.patid, zip_marker, age_strata, sex, ethnicity, race, max(p.numerator) numerator, COUNT(*), max(denom_excl)
    // GROUP BY   p.patid, zip_marker, age_strata, sex, ethnicity, race
    std::vector<int32_t> groupByCols{0, 1, 2, 3, 4, 5};
    std::vector<ScalarAggregateDefinition> unionAggregators {
            ScalarAggregateDefinition(6, AggregateId::MAX, "numerator"),
            ScalarAggregateDefinition(-1, AggregateId::COUNT, "site_count"),
            ScalarAggregateDefinition(9, AggregateId::MAX, "denom_excl")
    };

    SortMergeAggregate<emp::Bit> unioned_pats(sort_unioned, groupByCols, unionAggregators );
    return unioned_pats.run()->clone();

}


SecureTable *EnrichTest::filterPatients() {

    SecureTable *deduped = loadUnionAndDeduplicateData();

    // *** Filter
    // HAVING max(denom_excl) = 0
   auto zero  = new LiteralNode<emp::Bit>(Field<emp::Bit>(FieldType::SECURE_INT, emp::Integer(32, 0)));;
   ExpressionNode<emp::Bit> * input = new InputReference<emp::Bit>(8, deduped->getSchema());
   ExpressionNode<emp::Bit> *equality = new EqualNode<emp::Bit>(input, zero);
    Expression<emp::Bit> *equality_expr = new GenericExpression<emp::Bit>(equality, "predicate", FieldType::SECURE_BOOL);

    Filter inclusion_cohort(deduped, equality_expr);

    return  inclusion_cohort.run()->clone();



}

SecureTable *EnrichTest::getPatientCohort() {

    SecureTable *inclusionCohort = filterPatients();

    // *** Project on aggregate outputs:
    //     CASE WHEN count(*) > 1 THEN 1 else 0 END AS multisite
    //    CASE WHEN MAX(numerator)=1 ^ COUNT(*) > 1 THEN 1 ELSE 0 END AS numerator_multisite
    // output schema:
    // zip_marker, age_strata, sex, ethnicity, race, max(p.numerator) numerator, COUNT(*) > 1, COUNT(*) > 1 ^ numerator
    ExpressionMapBuilder<emp::Bit> builder(inclusionCohort->getSchema());
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

    Expression<emp::Bit>* multisiteExpression = new FunctionExpression<emp::Bit>(&(EnrichTestSupport<emp::Bit>::projectMultisite), &(EnrichTestSupport<emp::Bit>::projectMultisiteTable), "multisite", FieldType::SECURE_INT);
    Expression<emp::Bit> * multisiteNumeratorExpression = new FunctionExpression<emp::Bit>(&(EnrichTestSupport<emp::Bit>::projectNumeratorMultisite),  &(EnrichTestSupport<emp::Bit>::projectNumeratorMultisiteTable), "numerator_multisite", FieldType::SECURE_INT);
    //Expression multisiteExpression(&(EnrichTestSupport<emp::Bit>::projectMultisite), "multisite", FieldType::SECURE_INT);
    //Expression multisiteNumeratorExpression(&(EnrichTestSupport<emp::Bit>::projectNumeratorMultisite), "numerator_multisite", FieldType::SECURE_INT);

    builder.addExpression(multisiteExpression, 6);
    builder.addExpression(multisiteNumeratorExpression, 7);

    Project project(inclusionCohort, builder.getExprs());




    return project.run()->clone();

}

SecureTable *EnrichTest::aggregatePatientData() {
    SecureTable *pat_incl = getPatientCohort();

    // sort it on cols [0,5)
    auto sort = new Sort(pat_incl, DataUtilities::getDefaultSortDefinition(5));

    std::vector<int32_t> group_bys{0, 1, 2, 3, 4};
    std::vector<ScalarAggregateDefinition> aggregators {
            ScalarAggregateDefinition(5, AggregateId::SUM, "numerator_cnt"),
            ScalarAggregateDefinition(-1, AggregateId::COUNT, "denominator_cnt"),
            ScalarAggregateDefinition(7, AggregateId::SUM, "numerator_multisite"),
            ScalarAggregateDefinition(6, AggregateId::SUM, "denominator_multisite")
    };


    // output schema:
    // zip_marker (0), age_strata (1), sex (2), ethnicity (3) , race (4), numerator_cnt (5), denominator_cnt (6), numerator_multisite (7), denominator_multisite (8)
    SortMergeAggregate aggregated_pats(sort, group_bys, aggregators );
    return aggregated_pats.run()->clone();

}


/**** verification methods ****/

std::string EnrichTest::getRollupExpectedResultsSql(const std::string &group_by_col) {
    std::string expected_sql = "    WITH labeled as (\n"
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
                                    "    SELECT p.patid, zip_marker, age_strata, sex, ethnicity, race, MAX(p.numerator) numerator, COUNT(*)::BIGINT cnt\n"
                                    "    FROM labeled p JOIN patient_exclusion pe ON p.patid = pe.patid AND p.site_id = pe.site_id\n"
                                    "    GROUP BY p.patid, zip_marker, age_strata, sex, ethnicity, race \n"
                                    "    HAVING MAX(denom_excl) = 0 \n"
                                    "    ORDER BY p.patid, zip_marker, age_strata, sex, ethnicity, race ),  \n"
                                    "  data_cube AS (SELECT  zip_marker, age_strata, sex, ethnicity, race, SUM(numerator) numerator, COUNT(*)::BIGINT denominator, SUM((numerator = 1 AND cnt> 1)::INT) numerator_multisite, SUM(CASE WHEN cnt > 1 THEN 1 else 0 END)  denominator_multisite \n"
                                    "  FROM deduplicated \n"
                                    "  GROUP BY zip_marker, age_strata, sex, ethnicity, race \n"
                                    "  ORDER BY zip_marker, age_strata, sex, ethnicity, race ) \n";

    expected_sql += "SELECT " + group_by_col + ", SUM(numerator)::INT numerator_cnt, SUM(denominator)::BIGINT denominator_cnt, SUM(numerator_multisite)::INT numerator_multisite, SUM(denominator_multisite)::INT denominator_multisite \n";
    expected_sql += " FROM data_cube \n"
                         " GROUP BY " + group_by_col + " \n"
                                                         " ORDER BY " + group_by_col;

    return expected_sql;

}

void EnrichTest::validateUnion(Operator<emp::Bit> &sortOp, const SortDefinition &expected_sort) const {
    std::string expected_sql = "    WITH labeled as (\n"
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

    PlainTable *observed = sortOp.run()->reveal();

    validateTable(unioned_enrich_db_, expected_sql, expected_sort, observed);
    delete observed;

}


void EnrichTest::validateTable(const std::string & db_name, const std::string & sql, const SortDefinition  & expected_sortDefinition, PlainTable *observed) const {

    PsqlDataProvider data_provider;
    PlainTable *expected = data_provider.getQueryTable(db_name, sql);
    expected->setSortOrder(expected_sortDefinition);

    ASSERT_EQ(*expected, *observed);
    delete expected;

}


/**** end verification methods ****/


TEST_F(EnrichTest, loadAndPrepData) {
    string expected_pat_sql = "       SELECT patid, zip_marker, CASE WHEN age_days <= 28*365 THEN 0\n"
                                      "                WHEN age_days > 28*365 AND age_days <= 39*365 THEN 1\n"
                                      "              WHEN age_days > 39*365  AND age_days <= 50*365 THEN 2\n"
                                      "              WHEN age_days > 50*365 AND age_days <= 61*365 THEN 3\n"
                                      "              WHEN age_days > 61*365 AND age_days <= 72*365 THEN 4\n"
                                      "                WHEN age_days > 72*365  AND age_days <= 83*365 THEN 5\n"
                                      "                ELSE 6 END age_strata,\n"
                                      "            sex, ethnicity, race, numerator \n"
                                      "FROM patient \n"
                                      "ORDER BY patid";

    string expected_pat_excl_sql = "SELECT * FROM patient_exclusion ORDER BY patid, denom_excl";
    SortDefinition pat_sort_def{ColumnSort(0, SortDirection::ASCENDING)};
    SortDefinition pat_excl_sort_def{ColumnSort(0, SortDirection::ASCENDING), ColumnSort(2, SortDirection::ASCENDING)};

    // *** alice test
    PlainTable *alice_pat = loadAndProjectPatientData(alice_enrich_db_);
    // this is more of a smoke test to make sure the setup is aligned correctly, not testing query reading facility
    PlainTable *alice_pat_excl = loadPatientExclusionData(alice_enrich_db_);

    validateTable(alice_enrich_db_, expected_pat_sql, pat_sort_def, alice_pat);
    validateTable(alice_enrich_db_, expected_pat_excl_sql, pat_excl_sort_def, alice_pat_excl);


    // *** bob test
    PlainTable *bob_pat = loadAndProjectPatientData(bob_enrich_db_);
    PlainTable *bob_pat_excl = loadPatientExclusionData(bob_enrich_db_);

    validateTable(bob_enrich_db_, expected_pat_sql, pat_sort_def, bob_pat);
    validateTable(bob_enrich_db_, expected_pat_excl_sql, pat_excl_sort_def, bob_pat_excl);

    delete alice_pat;
    delete alice_pat_excl;
    delete bob_pat;
    delete bob_pat_excl;
}



// need to refactor this to do the join before unioning Alice and Bob's inputs
TEST_F(EnrichTest, loadAndJoinData) {

    string expected_sql = "       SELECT p.patid, zip_marker, CASE WHEN age_days <= 28*365 THEN 0\n"
                                      "                WHEN age_days > 28*365 AND age_days <= 39*365 THEN 1\n"
                                      "              WHEN age_days > 39*365  AND age_days <= 50*365 THEN 2\n"
                                      "              WHEN age_days > 50*365 AND age_days <= 61*365 THEN 3\n"
                                      "              WHEN age_days > 61*365 AND age_days <= 72*365 THEN 4\n"
                                      "                WHEN age_days > 72*365  AND age_days <= 83*365 THEN 5\n"
                                      "                ELSE 6 END age_strata,\n"
                                      "            sex, ethnicity, race, p.numerator, pe.patid, pe.numerator n2, denom_excl \n"
                                      "FROM patient p JOIN patient_exclusion pe ON p.patid = pe.patid \n"
                                      "ORDER BY p.patid, denom_excl";


    SortDefinition  expected_sort{ColumnSort(0, SortDirection::ASCENDING), ColumnSort(7, SortDirection::ASCENDING), ColumnSort(9, SortDirection::ASCENDING)};
    PlainTable *alice_joined = loadAndJoinLocalData(alice_enrich_db_);
    validateTable(alice_enrich_db_, expected_sql, expected_sort, alice_joined);

    PlainTable *bob_joined = loadAndJoinLocalData(bob_enrich_db_);
    validateTable(bob_enrich_db_, expected_sql, expected_sort, bob_joined);

    delete alice_joined;
    delete bob_joined;
}


TEST_F(EnrichTest, loadUnionAndDeduplicateData) {

    std::string expected_sql = "    WITH labeled as (\n"
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
                                    "    SELECT p.patid, zip_marker, age_strata, sex, ethnicity, race, max(p.numerator) numerator, COUNT(*)::BIGINT cnt, max(denom_excl) denom_excl\n"
                                    "    FROM labeled p JOIN patient_exclusion pe ON p.patid = pe.patid AND p.site_id = pe.site_id\n"
                                    "    GROUP BY p.patid, zip_marker, age_strata, sex, ethnicity, race "
                                    "    ORDER BY p.patid, zip_marker, age_strata, sex, ethnicity, race ";

    SecureTable *deduplicated = loadUnionAndDeduplicateData();
    PlainTable *observed = deduplicated->reveal();
    DataUtilities::removeDummies(observed);

    validateTable(unioned_enrich_db_, expected_sql, DataUtilities::getDefaultSortDefinition(6), observed);

    delete observed;
    delete deduplicated;
}

TEST_F(EnrichTest, testExclusionCriteria) {
    std::string expected_sql = "    WITH labeled as (\n"
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
                                    "    SELECT p.patid, zip_marker, age_strata, sex, ethnicity, race, MAX(p.numerator) numerator, COUNT(*)::BIGINT cnt, MAX(denom_excl) denom_excl\n"
                                    "    FROM labeled p JOIN patient_exclusion pe ON p.patid = pe.patid AND p.site_id = pe.site_id\n"
                                    "    GROUP BY p.patid, zip_marker, age_strata, sex, ethnicity, race \n"
                                    "    HAVING MAX(denom_excl) = 0 \n"
                                    "    ORDER BY p.patid, zip_marker, age_strata, sex, ethnicity, race ";


    SecureTable *filtered = filterPatients();
    PlainTable *observed = filtered->reveal();
    DataUtilities::removeDummies(observed);

    validateTable(unioned_enrich_db_, expected_sql, DataUtilities::getDefaultSortDefinition(6), observed);

    delete filtered;
    delete observed;

}


TEST_F(EnrichTest, testPatientCohort) {

    std::string expected_sql = "    WITH labeled as (\n"
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
                                    "  FROM deduplicated ORDER BY (1), (2), (3), (4), (5), (6), (7)";

    SecureTable *pat_cohort = getPatientCohort();
    // give it a collation w/o sorting on dummy tag to trigger sort in reveal()
    auto collation = DataUtilities::getDefaultSortDefinition(7);
    pat_cohort->setSortOrder(collation);
    PlainTable *observed = pat_cohort->reveal();


    validateTable(unioned_enrich_db_, expected_sql, collation, observed);
    delete observed;

}

TEST_F(EnrichTest, testPatientAgggregation) {
    std::string expected_sql = "    WITH labeled as (\n"
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
                                    "  SELECT  zip_marker, age_strata, sex, ethnicity, race, SUM(numerator)::INT, COUNT(*)::BIGINT denominator, SUM((numerator = 1 AND cnt> 1)::INT)::INT numerator_multisite, SUM(CASE WHEN cnt > 1 THEN 1 else 0 END)::INT  multisite \n"
                                    "  FROM deduplicated \n"
                                    "  GROUP BY zip_marker, age_strata, sex, ethnicity, race \n"
                                    "  ORDER BY zip_marker, age_strata, sex, ethnicity, race ";

  SecureTable *aggregator = aggregatePatientData();
  PlainTable *observed = aggregator->reveal();
  DataUtilities::removeDummies(observed);


  validateTable(unioned_enrich_db_, expected_sql, DataUtilities::getDefaultSortDefinition(5), observed);

  delete aggregator;
  delete observed;
}



TEST_F(EnrichTest, testRollups) {
    SecureTable *aggregator = aggregatePatientData();
    SortDefinition order_by = DataUtilities::getDefaultSortDefinition(1);

    // roll-ups:
    // age_strata (1)
    SecureTable *tmp = rollUpAggregate(1, aggregator->clone());
    tmp->setSortOrder(order_by);
    PlainTable *age = tmp->reveal();
    std::string expected_sql = getRollupExpectedResultsSql("age_strata");
    validateTable(unioned_enrich_db_, expected_sql, order_by, age);

    // gender (2)
    tmp = rollUpAggregate(2, aggregator->clone());
    tmp->setSortOrder(order_by);
    PlainTable *gender = tmp->reveal();
    expected_sql = getRollupExpectedResultsSql("sex");
    validateTable(unioned_enrich_db_, expected_sql, order_by, gender);


    // ethnicity (3)
    tmp = rollUpAggregate(3, aggregator->clone());
    tmp->setSortOrder(order_by);
    PlainTable *ethnicity = tmp->reveal();
    expected_sql = getRollupExpectedResultsSql("ethnicity");
    validateTable(unioned_enrich_db_, expected_sql, order_by, ethnicity);

    // race (4)
    tmp = rollUpAggregate(4, aggregator->clone());
    tmp->setSortOrder(order_by);
    PlainTable *race = tmp->reveal();
    expected_sql = getRollupExpectedResultsSql("race");
    validateTable(unioned_enrich_db_, expected_sql, order_by, race);


    // zip marker (0)
    tmp = rollUpAggregate(0, aggregator->clone());
    tmp->setSortOrder(order_by);
    PlainTable *zip_marker = tmp->reveal();
    expected_sql = getRollupExpectedResultsSql("zip_marker");
    validateTable(unioned_enrich_db_, expected_sql, order_by, zip_marker);


    delete aggregator;
    delete gender;
    delete race;
    delete ethnicity;
    delete age;
    delete zip_marker;
}

// set up with 100 tuples:
// bash test/support/load-generated-data.sh 100
int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    gflags::ParseCommandLineFlags(&argc, &argv, false);

    ::testing::GTEST_FLAG(filter)=FLAGS_filter;
    return RUN_ALL_TESTS();

}

