#include "enrich_htn_query.h"
#include <util/data_utilities.h>
#include <sort.h>
#include <project.h>
#include <query_table/plain_tuple.h>
#include <expression/function_expression.h>
#include <expression/comparator_expression_nodes.h>
#include <operators/shrinkwrap.h>
#include <operators/union.h>

using namespace vaultdb;


EnrichHtnQuery::EnrichHtnQuery(shared_ptr<SecureTable> & input, const size_t & cardinality) : inputTable(input), cardinalityBound(cardinality) {

  // takes in shared_schema
    shared_ptr<SecureTable> filtered = filterPatients();
    shared_ptr<SecureTable> projected = projectPatients(filtered);
    aggregatePatients(projected);
    filtered.reset();
    projected.reset();

}


// input schema: pat_id (0),  age_strata (1), sex (2), ethnicity (3), race (4), numerator (5), denom_excl (6)
shared_ptr<SecureTable> EnrichHtnQuery::filterPatients() {

    // sort it on group-by cols to prepare for aggregate
    // TODO: integrate this with the unioning step.  See UnionHybridData for more on this
    // patid, age_days, sex, ethnicity, race
  SortDefinition unionSortDefinition{ColumnSort(0, SortDirection::ASCENDING), // pat_ID
				     ColumnSort(1, SortDirection::ASCENDING), // age_strata
				     ColumnSort(2, SortDirection::ASCENDING), // sex
				     ColumnSort(3, SortDirection::ASCENDING), // ethnicity
				     ColumnSort(4, SortDirection::ASCENDING)}; // race

    // destructor handled within Operator
    Sort<emp::Bit> sortUnioned(inputTable, unionSortDefinition);
    shared_ptr<SecureTable> sorted = sortUnioned.run();

    
    // aggregate to deduplicate
    // finds if a patient meets exclusion criteria, if a patient is in the numerator in at least one site
    // aka:
    //    SELECT  p.patid,  age_strata, sex, ethnicity, race, max(p.numerator) numerator, COUNT(*), max(denom_excl)
    // GROUP BY   p.patid, age_strata, sex, ethnicity, race
    std::vector<int32_t> groupByCols{0, 1, 2, 3, 4};
    std::vector<ScalarAggregateDefinition> aggregators {
            ScalarAggregateDefinition(5, AggregateId::MAX, "numerator"),
            ScalarAggregateDefinition(-1, AggregateId::COUNT, "site_count"),
            ScalarAggregateDefinition(6, AggregateId::MAX, "denom_excl")
    };

    GroupByAggregate unionedPatients(sorted, groupByCols, aggregators );
    shared_ptr<SecureTable> aggregated = unionedPatients.run();

    sorted.reset();

    // filter ones with denom_excl = 1
    // *** Filter
    // HAVING max(denom_excl) = false
    shared_ptr<ExpressionNode<emp::Bit> > zero(new LiteralNode<emp::Bit>(Field<emp::Bit>(FieldType::SECURE_BOOL, emp::Bit(false))));;
    shared_ptr<ExpressionNode<emp::Bit> > input(new InputReferenceNode<emp::Bit>(7));
    shared_ptr<ExpressionNode<emp::Bit> > equality(new EqualNode<emp::Bit>(input, zero));

    BoolExpression<emp::Bit> equality_expr(equality);

    Filter inclusionCohort(aggregated, equality_expr);

    shared_ptr<SecureTable> output =   inclusionCohort.run();
    aggregated.reset();
    return output;




}

  
  //  input schema: pat_id (0),  age_strata (1), sex (2), ethnicity (3), race (4), max(numerator) (5), site_count (6), max(denom_excl) (7)
  //  output schema: age_strata (0), sex (1), ethnicity (2), race (3),
  //  max(p.numerator) numerator (4), COUNT(*) > 1 denom_multisite (5), COUNT(*) > 1 ^ numerator numerator_multisite (6)

  shared_ptr<SecureTable> EnrichHtnQuery::projectPatients(const shared_ptr<SecureTable> &src) {

    // *** Project on aggregate outputs:
    //     CASE WHEN count(*) > 1 THEN 1 else 0 END AS multisite
    //    CASE WHEN MAX(numerator)=1 ^ COUNT(*) > 1 THEN 1 ELSE 0 END AS numerator_multisite

    ExpressionMapBuilder<emp::Bit> builder(*src->getSchema());
    for(int i = 1; i < 5; ++i) {
            // push back age_strata, sex, ethnicity, race
            builder.addMapping(i, i-1);
    }

      // cast numerator to INT for summing in next step
      // references (5)
      shared_ptr<ExpressionNode<emp::Bit> > castNumerator(new CastNode<emp::Bit>(5, FieldType::SECURE_INT));
      shared_ptr<Expression<emp::Bit> >  numeratorToIntExpression(new GenericExpression<emp::Bit>(castNumerator, "numerator", FieldType::SECURE_INT));

    // references (6)
    shared_ptr<Expression<emp::Bit> >  multisiteExpression(new FunctionExpression(&EnrichHtnQuery::projectMultisite<emp::Bit>, "multisite", FieldType::SECURE_INT));
    // references (5) (6)
    shared_ptr<Expression<emp::Bit> >  multisiteNumeratorExpression(new FunctionExpression(&EnrichHtnQuery::projectNumeratorMultisite<emp::Bit>, "numerator_multisite", FieldType::SECURE_INT));

    builder.addExpression(numeratorToIntExpression, 4);
    builder.addExpression(multisiteExpression, 5);
    builder.addExpression(multisiteNumeratorExpression, 6);

    Project project(src, builder.getExprs());
    return project.run();

}

// input schema: age_strata (0), sex (1), ethnicity (2), race (3), numerator (4),  denom_multisite (5), numerator_multisite (6)
void EnrichHtnQuery::aggregatePatients( shared_ptr<SecureTable> &src) {
    // sort it on cols [0,5)
    Sort sort(src, DataUtilities::getDefaultSortDefinition(4));
    shared_ptr<SecureTable> sorted = sort.run();
    src.reset();

    Logger::write("Finished sort for data cube.");
    Utilities::checkMemoryUtilization();

    std::vector<int32_t> groupByCols{0, 1, 2, 3};
    std::vector<ScalarAggregateDefinition> aggregators {
            ScalarAggregateDefinition(4, AggregateId::SUM, "numerator_cnt"),
            ScalarAggregateDefinition(-1, AggregateId::COUNT, "denominator_cnt"),
            ScalarAggregateDefinition(6, AggregateId::SUM, "numerator_multisite"),
            ScalarAggregateDefinition(5, AggregateId::SUM, "denominator_multisite")
    };


    // output schema:
    // age_strata (0), sex (1), ethnicity (2) , race (3), numerator_cnt (4), denominator_cnt (5), numerator_multisite (6), denominator_multisite (7)
    GroupByAggregate aggregator(sorted, groupByCols, aggregators);
    dataCube = aggregator.run();
    sorted.reset();
    Logger::write("Aggregated.");
    Utilities::checkMemoryUtilization();


    Shrinkwrap wrapper(dataCube, cardinalityBound);
    dataCube = wrapper.run();
    Logger::write("shrinkwrapped.");
    Utilities::checkMemoryUtilization();

}

// partials schema:
// age_strata (0), sex (1), ethnicity (2) , race (3), numerator_cnt (4), denominator_cnt (5), numerator_multisite_cnt (6), denominator_multisite_cnt (7)
void EnrichHtnQuery::addPartialAggregates(vector<shared_ptr<SecureTable>> partials) {
    shared_ptr<SecureTable> summedPartials(new SecureTable(*partials[0]));
    size_t tuple_cnt = summedPartials->getTupleCount();
    assert(*(summedPartials->getSchema()) == *(dataCube->getSchema()));

    for(size_t i = 1; i < partials.size(); ++i) {
        shared_ptr<SecureTable> partial = partials[i];
        assert(tuple_cnt == partial->getTupleCount()); // check that they line up
        assert(*(partial->getSchema()) == *(dataCube->getSchema()));
    }

    // for each tuple
    for(size_t i = 0; i < tuple_cnt; ++i) {
            SecureTuple dst = (*summedPartials)[i];

            // for each partial count
            for(size_t j = 1; j < partials.size(); ++j) {
                SecureTuple src = (*(partials[j]))[i];
                // if one of them is not a dummy, then it's not a dummy
                dst.setDummyTag(src.getDummyTag() | dst.getDummyTag());

                // add up the counts - only need this for single-site figures
                dst.setField(4, dst[4] + src[4]);
                dst.setField(5, dst[5] + src[5]);

            }
    }

    Union<emp::Bit> union_op(dataCube, summedPartials);
    dataCube = union_op.run();


    // basically a rerun of aggregatePatients
    // TODO: clean this up!

    // sort it on cols [0,5)
    Sort sort(dataCube, DataUtilities::getDefaultSortDefinition(4));
    shared_ptr<SecureTable> sorted = sort.run();

    std::vector<int32_t> groupByCols{0, 1, 2, 3};
    std::vector<ScalarAggregateDefinition> aggregators {
            ScalarAggregateDefinition(4, AggregateId::SUM, "numerator_cnt"),
            ScalarAggregateDefinition(5, AggregateId::SUM, "denominator_cnt"),
            ScalarAggregateDefinition(6, AggregateId::SUM, "numerator_multisite"),
            ScalarAggregateDefinition(7, AggregateId::SUM, "denominator_multisite")
    };


    // output schema:
    // age_strata (0), sex (1), ethnicity (2) , race (3), numerator_cnt (4), denominator_cnt (5), numerator_multisite (6), denominator_multisite (7)
    GroupByAggregate aggregator(sorted, groupByCols, aggregators);
    dataCube = aggregator.run();
    sorted.reset();

    Shrinkwrap wrapper(dataCube, cardinalityBound);
    dataCube = wrapper.run();



}



