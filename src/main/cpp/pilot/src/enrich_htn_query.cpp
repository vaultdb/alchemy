#include "enrich_htn_query.h"
#include <util/data_utilities.h>
#include <sort.h>
#include <filter.h>
#include <project.h>
#include <query_table/secure_tuple.h>
#include <query_table/plain_tuple.h>
#include <expression/function_expression.h>
#include <expression/comparator_expression_nodes.h>

using namespace vaultdb;


EnrichHtnQuery::EnrichHtnQuery(shared_ptr<SecureTable> & input) : inputTable(input) {

    shared_ptr<SecureTable> filtered = filterPatients();
    shared_ptr<SecureTable> projected = projectPatients(filtered);
    aggregatePatients(projected);
    filtered.reset();
    projected.reset();

}


shared_ptr<SecureTable> EnrichHtnQuery::filterPatients() {

    // sort it on group-by cols to prepare for aggregate
    // patid, zip_marker, age_days, sex, ethnicity, race
    SortDefinition unionSortDefinition = DataUtilities::getDefaultSortDefinition(6);
    // site_id
    unionSortDefinition.push_back(ColumnSort(8, SortDirection::ASCENDING)); // last sort makes it verifiable

    // destructor handled within Operator
    Sort<emp::Bit> sortUnioned(inputTable, unionSortDefinition);
    shared_ptr<SecureTable> sorted = sortUnioned.run();




              // aggregate to deduplicate
    // finds if a patient meets exclusion criteria, if a patient is in the numerator in at least one site
    // aka:
    //    SELECT  p.patid, zip_marker, age_strata, sex, ethnicity, race, max(p.numerator) numerator, COUNT(*), max(denom_excl)
    // GROUP BY   p.patid, zip_marker, age_strata, sex, ethnicity, race
    std::vector<int32_t> groupByCols{0, 1, 2, 3, 4, 5};
    std::vector<ScalarAggregateDefinition> aggregators {
            ScalarAggregateDefinition(6, AggregateId::MAX, "numerator"),
            ScalarAggregateDefinition(-1, AggregateId::COUNT, "site_count"),
            ScalarAggregateDefinition(7, AggregateId::MAX, "denom_excl")
    };

    GroupByAggregate unionedPatients(sorted, groupByCols, aggregators );
    shared_ptr<SecureTable> aggregated = unionedPatients.run();

    sorted.reset();
    Utilities::checkMemoryUtilization("Deleted sort");


    // filter ones with denom_excl = 1
    // *** Filter
    // HAVING max(denom_excl) = 0
    shared_ptr<ExpressionNode<emp::Bit> > zero(new LiteralNode<emp::Bit>(Field<emp::Bit>(FieldType::SECURE_INT, emp::Integer(32, 0))));;
    shared_ptr<ExpressionNode<emp::Bit> > input(new InputReferenceNode<emp::Bit>(8));
    shared_ptr<ExpressionNode<emp::Bit> > equality(new EqualNode<emp::Bit>(input, zero));

    BoolExpression<emp::Bit> equality_expr(equality);

    Filter inclusionCohort(aggregated, equality_expr);

    shared_ptr<SecureTable> output =   inclusionCohort.run();
    aggregated.reset();

    Utilities::checkMemoryUtilization("Deleted aggregate");
    return output;




}

shared_ptr<SecureTable> EnrichHtnQuery::projectPatients(const shared_ptr<SecureTable> &src) {

    // *** Project on aggregate outputs:
    //     CASE WHEN count(*) > 1 THEN 1 else 0 END AS multisite
    //    CASE WHEN MAX(numerator)=1 ^ COUNT(*) > 1 THEN 1 ELSE 0 END AS numerator_multisite
    // output schema:
    // zip_marker, age_strata, sex, ethnicity, race, max(p.numerator) numerator, COUNT(*) > 1, COUNT(*) > 1 ^ numerator
    Utilities::checkMemoryUtilization("before projection");

    ExpressionMapBuilder<emp::Bit> builder;
    for(int i = 1; i < 7; ++i)
        if(i != 2)
            builder.addMapping(i, i-1);

        shared_ptr<Expression<emp::Bit> > ageStrataExpression(new FunctionExpression(&EnrichHtnQuery::projectAgeStrata<emp::Bit>, "age_strata", FieldType::SECURE_INT));
    shared_ptr<Expression<emp::Bit> >  multisiteExpression(new FunctionExpression(&EnrichHtnQuery::projectMultisite<emp::Bit>, "multisite", FieldType::SECURE_INT));
    shared_ptr<Expression<emp::Bit> >  multisiteNumeratorExpression(new FunctionExpression(&EnrichHtnQuery::projectNumeratorMultisite<emp::Bit>, "numerator_multisite", FieldType::SECURE_INT));


    builder.addExpression(ageStrataExpression, 1);
    builder.addExpression(multisiteExpression, 6);
    builder.addExpression(multisiteNumeratorExpression, 7);

    Project project(src, builder.getExprs());

    return project.run();

}


void EnrichHtnQuery::aggregatePatients(const shared_ptr<SecureTable> &src) {

    Utilities::checkMemoryUtilization("before sort");

    // sort it on cols [0,5)
    Sort sort(src, DataUtilities::getDefaultSortDefinition(5));
    shared_ptr<SecureTable> sorted = sort.run();

    Utilities::checkMemoryUtilization("deleting sort");

    std::vector<int32_t> groupByCols{0, 1, 2, 3, 4};
    std::vector<ScalarAggregateDefinition> aggregators {
            ScalarAggregateDefinition(5, AggregateId::SUM, "numerator_cnt"),
            ScalarAggregateDefinition(-1, AggregateId::COUNT, "denominator_cnt"),
            ScalarAggregateDefinition(7, AggregateId::SUM, "numerator_multisite"),
            ScalarAggregateDefinition(6, AggregateId::SUM, "denominator_multisite")
    };


    // output schema:
    // zip_marker (0), age_strata (1), sex (2), ethnicity (3) , race (4), numerator_cnt (5), denominator_cnt (6), numerator_multisite (7), denominator_multisite (8)
    GroupByAggregate aggregator(sorted, groupByCols, aggregators);
    dataCube = aggregator.run();
    sorted.reset();

    Utilities::checkMemoryUtilization("deleting aggregate");


}

// roll up one group-by col at a time
// input schema:
// zip_marker (0) age_strata (1), sex (2), ethnicity (3) , race (4), numerator_cnt (5), denominator_cnt (6), numerator_multisite (7), denominator_multisite (8)

shared_ptr<SecureTable> EnrichHtnQuery::rollUpAggregate(const int &ordinal) const {

    SortDefinition sortDefinition{ColumnSort(ordinal, SortDirection::ASCENDING)};
    Sort sort(dataCube, sortDefinition);
    shared_ptr<SecureTable> sorted = sort.run();

    std::vector<int32_t> groupByCols{ordinal};
    // ordinals 0...4 are group-by cols in input schema
    std::vector<ScalarAggregateDefinition> aggregators {
            ScalarAggregateDefinition(5, AggregateId::SUM, "numerator_cnt"),
            ScalarAggregateDefinition(6, AggregateId::SUM, "denominator_cnt"),
            ScalarAggregateDefinition(7, AggregateId::SUM, "numerator_multisite"),
            ScalarAggregateDefinition(8, AggregateId::SUM, "denominator_multisite")
    };

    GroupByAggregate rollupStrata(sorted, groupByCols, aggregators );
    shared_ptr<SecureTable> result = rollupStrata.run();

    return result;


}



