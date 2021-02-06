#include "enrich_htn_query.h"
#include <util/data_utilities.h>
#include <sort.h>
#include <common_table_expression.h>
#include <filter.h>
#include <project.h>

using namespace vaultdb;

EnrichHtnQuery::EnrichHtnQuery(shared_ptr<QueryTable> & input) : inputTable(input) {
    shared_ptr<QueryTable> filtered = filterPatients();
    shared_ptr<QueryTable> projected = projectPatients(filtered);
    aggregatePatients(projected);

}


shared_ptr<QueryTable> EnrichHtnQuery::filterPatients() {

    // sort it on group-by cols to prepare for aggregate
    // patid, zip_marker, age_days, sex, ethnicity, race
    SortDefinition unionSortDefinition = DataUtilities::getDefaultSortDefinition(6);
    // site_id
    unionSortDefinition.push_back(ColumnSort(8, SortDirection::ASCENDING)); // last sort makes it verifiable

    // destructor handled within Operator
    CommonTableExpression *inputOp = new CommonTableExpression(inputTable);
    Sort *sortUnioned = new Sort(unionSortDefinition, inputOp->getPtr());


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

    GroupByAggregate *unionedPatients = new GroupByAggregate(sortUnioned->getPtr(), groupByCols, aggregators );

    // filter ones with denom_excl = 1
    // *** Filter
    // HAVING max(denom_excl) = 0
    std::shared_ptr<Predicate> predicateClass(new FilterExcludedPatients(true));
    Filter *inclusionCohort = new Filter(predicateClass, unionedPatients->getPtr());

    return  inclusionCohort->run();


}

shared_ptr<QueryTable> EnrichHtnQuery::projectPatients(shared_ptr<QueryTable> src) {

    // *** Project on aggregate outputs:
    //     CASE WHEN count(*) > 1 THEN 1 else 0 END AS multisite
    //    CASE WHEN MAX(numerator)=1 ^ COUNT(*) > 1 THEN 1 ELSE 0 END AS numerator_multisite
    // output schema:
    // zip_marker, age_strata, sex, ethnicity, race, max(p.numerator) numerator, COUNT(*) > 1, COUNT(*) > 1 ^ numerator
    CommonTableExpression *srcExpression = new CommonTableExpression(src);
    Project *projectOp = new Project(srcExpression->getPtr());
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

    Expression multisiteExpression(&EnrichHtnQuery::projectMultisite, "multisite", intType);
    Expression multisiteNumeratorExpression(&EnrichHtnQuery::projectNumeratorMultisite, "numerator_multisite", intType);

    projectOp->addColumnMappings(mappingSet);
    projectOp->addExpression(multisiteExpression, 6);
    projectOp->addExpression(multisiteNumeratorExpression, 7);

    return projectOp->run();

}

// TODO: initialize dataCube, aggregator here
void EnrichHtnQuery::aggregatePatients(shared_ptr<QueryTable> src) {

    std::shared_ptr<Operator> includedPatients(new CommonTableExpression(src));

    // sort it on cols [0,5)
    Sort *sortOp = new Sort(DataUtilities::getDefaultSortDefinition(5), includedPatients->getPtr());

    std::vector<int32_t> groupByCols{0, 1, 2, 3, 4};
    std::vector<ScalarAggregateDefinition> aggregators {
            ScalarAggregateDefinition(5, AggregateId::SUM, "numerator_cnt"),
            ScalarAggregateDefinition(-1, AggregateId::COUNT, "denominator_cnt"),
            ScalarAggregateDefinition(7, AggregateId::SUM, "numerator_multisite"),
            ScalarAggregateDefinition(6, AggregateId::SUM, "denominator_multisite")
    };


    // output schema:
    // zip_marker (0), age_strata (1), sex (2), ethnicity (3) , race (4), numerator_cnt (5), denominator_cnt (6), numerator_multisite (7), denominator_multisite (8)
    aggregator = shared_ptr<GroupByAggregate>(new GroupByAggregate(sortOp->getPtr(), groupByCols, aggregators ));
    dataCube = aggregator->run();
}

// roll up one group-by col at a time
// input schema:
// zip_marker (0) age_strata (1), sex (2), ethnicity (3) , race (4), numerator_cnt (5), denominator_cnt (6), numerator_multisite (7), denominator_multisite (8)

shared_ptr<QueryTable> EnrichHtnQuery::rollUpAggregate(const int &ordinal) {
    SortDefinition sortDefinition{ColumnSort(ordinal, SortDirection::ASCENDING)};
    Sort *sort = new Sort(sortDefinition, aggregator->getPtr());

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

Value EnrichHtnQuery::projectMultisite(const QueryTuple &aTuple) {
    Integer siteCount = aTuple.getFieldPtr(7)->getValue().getEmpInt();

    Bit condition = siteCount > Integer(64, 1, PUBLIC);

    // get from Value::TypeId bool --> int
    Integer result(32, 0, PUBLIC);
    result.bits[0] = condition;
    return Value(TypeId::ENCRYPTED_INTEGER32, result);
}

Value EnrichHtnQuery::projectNumeratorMultisite(const QueryTuple &aTuple) {
    Integer inNumerator = aTuple.getFieldPtr(6)->getValue().getEmpInt();
    Integer siteCount = aTuple.getFieldPtr(7)->getValue().getEmpInt();

    Bit multisite = siteCount > Integer(64, 1, PUBLIC);
    // only 0 || 1
    Bit numeratorTrue = inNumerator.bits[0];
    Bit condition = multisite & numeratorTrue;

    // get from Value::TypeId bool --> int
    Integer result(32, 0, PUBLIC);
    result.bits[0] = condition;
    return Value(TypeId::ENCRYPTED_INTEGER32, result);
}


