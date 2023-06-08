#include "enrich_htn_query.h"
#include "expression/generic_expression.h"
#include <util/data_utilities.h>
#include <operators/sort.h>
#include <operators/project.h>
#include <query_table/plain_tuple.h>
#include <expression/function_expression.h>
#include <expression/comparator_expression_nodes.h>
#include <operators/shrinkwrap.h>
#include <operators/union.h>
#include <pilot/src/common/shared_schema.h>

using namespace vaultdb;


EnrichHtnQuery::EnrichHtnQuery(SecureTable *input, const size_t & cardinality) : input_table_(input), cardinality_bound_(cardinality) {

  // takes in shared_schema
    SecureTable *filtered = filterPatients();
    SecureTable *projected = projectPatients(filtered);
    cout << "Completed deduplication and exclusion at epoch " << Utilities::getEpoch() << endl;
    aggregatePatients(projected);

    delete filtered;
    delete projected;

}


// input schema: study_year (0), pat_id (1),  age_strata (2), sex (3), ethnicity (4), race (5), numerator (6), denominator (7), denom_excl (8)
SecureTable *EnrichHtnQuery::filterPatients() {

    // sort it on group-by cols to prepare for aggregate
    // TODO: integrate this with the unioning step.  See UnionHybridData for more on this
    // patid, age_days, sex, ethnicity, race
    auto start_time = emp::clock_start();

    SortDefinition unionSortDefinition = DataUtilities::getDefaultSortDefinition(6);

    // destructor handled within Operator
    Sort<emp::Bit> sortUnioned(input_table_, unionSortDefinition);
    SecureTable *sorted = sortUnioned.run();

    
    // aggregate to deduplicate
    // finds if a patient meets exclusion criteria, if a patient is in the numerator in at least one site
    // aka:
    //    SELECT p.study_year, p.patid,  age_strata, sex, ethnicity, race, max(p.numerator) numerator, MAX(denominator::INT), max(denom_excl)
    // GROUP BY   p.study_year, p.patid, age_strata, sex, ethnicity, race
    std::vector<int32_t> groupByCols{0, 1, 2, 3, 4, 5};
    std::vector<ScalarAggregateDefinition> aggregators {
            ScalarAggregateDefinition(6, AggregateId::MAX, "numerator"), // #6
            ScalarAggregateDefinition(7, AggregateId::MAX, "denominator"),  // #7

            ScalarAggregateDefinition(-1, AggregateId::COUNT, "site_count"), // #8
            ScalarAggregateDefinition(8, AggregateId::MAX, "denom_excl")  // #9
    };

    GroupByAggregate unionedPatients(sorted, groupByCols, aggregators );
    SecureTable *aggregated = unionedPatients.run();

    double runtime = emp::time_from(start_time);
    cout << "Runtime for aggregate #1 (patid): " <<  (runtime+0.0)*1e6*1e-9 << " secs." << endl;
    delete sorted;

    // filter ones with denom_excl = 1
    // *** Filter
    // HAVING max(denom_excl) = false

    ExpressionNode<emp::Bit>  *zero = new LiteralNode<emp::Bit>(Field<emp::Bit>(FieldType::SECURE_BOOL, emp::Bit(false)));;
    ExpressionNode<emp::Bit> *input = new InputReferenceNode<emp::Bit>(9);
    ExpressionNode<emp::Bit> *equality = new EqualNode<emp::Bit>(input, zero);
    Expression<emp::Bit> *equality_expr = new GenericExpression<emp::Bit>(equality, "predicate", FieldType::SECURE_BOOL);


    start_time = emp::clock_start();

    Filter inclusionCohort(aggregated, equality_expr);

    SecureTable *output = inclusionCohort.run()->clone();
    runtime = emp::time_from(start_time);
    cout << "Runtime for filter: " <<  (runtime+0.0)*1e6*1e-9 << " secs." << endl;


    delete aggregated;
    return output;




}

  
  //  input schema: study_year (0), pat_id (1),  age_strata (2), sex (3), ethnicity (4), race (5), max(numerator) (6), max(denominator) (7),  site_count (8), max(denom_excl) (9)
  //  output schema: age_strata (0), sex (1), ethnicity (2), race (3),
  //  max(p.numerator) numerator (4),   max(p.numerator) denominator (5),
  //  COUNT(*) > 1 ^ numerator numerator_multisite (6)
  //  COUNT(*) > 1 ^ denominator denom_multisite (7),

  SecureTable *EnrichHtnQuery::projectPatients(SecureTable *src) {

    // *** Project on aggregate outputs:
    //     CASE WHEN count(*) > 1 THEN 1 else 0 END AS multisite
    //    CASE WHEN MAX(numerator)=1 ^ COUNT(*) > 1 THEN 1 ELSE 0 END AS numerator_multisite

    ExpressionMapBuilder<emp::Bit> builder(src->getSchema());
    builder.addMapping(0, 0);
    for(int i = 2; i < 6; ++i) {
            // push back age_strata, sex, ethnicity, race
            builder.addMapping(i, i-1);
    }

      // cast numerator to INT for summing in next step
      // references (5)

      ExpressionNode<emp::Bit> *numerator = new InputReferenceNode<emp::Bit>(6);
      ExpressionNode<emp::Bit> *castNumerator = new CastNode<emp::Bit>(numerator, FieldType::SECURE_INT);
      Expression<emp::Bit> *numeratorToIntExpression = new GenericExpression<emp::Bit>(castNumerator, "numerator", FieldType::SECURE_INT);

      ExpressionNode<emp::Bit> *denominator = new InputReferenceNode<emp::Bit>(7);
      ExpressionNode<emp::Bit> *castDenominator = new CastNode<emp::Bit>(denominator, FieldType::SECURE_INT);
      Expression<emp::Bit> * denominatorToIntExpression = new GenericExpression<emp::Bit>(castDenominator, "denominator", FieldType::SECURE_INT);


    // references (6) (9)
    Expression<emp::Bit>  *multisiteNumeratorExpression = new
        FunctionExpression(&EnrichHtnQuery::projectNumeratorMultisite<emp::Bit>, &EnrichHtnQuery::projectNumeratorMultisiteTable<emp::Bit>,
            "numerator_multisite", FieldType::SECURE_INT);
      // references (7) (9)
      Expression<emp::Bit> *  multisiteDenominatorExpression = new
         FunctionExpression(&EnrichHtnQuery::projectDenominatorMultisite<emp::Bit>, &EnrichHtnQuery::projectDenominatorMultisiteTable<emp::Bit>, "denominator_multisite", FieldType::SECURE_INT);

    builder.addExpression(numeratorToIntExpression, 5);
    builder.addExpression(denominatorToIntExpression, 6);

    builder.addExpression(multisiteNumeratorExpression, 7);
      builder.addExpression(multisiteDenominatorExpression, 8);

      auto start_time = emp::clock_start();

      Project project(src, builder.getExprs());

    SecureTable *projected = project.run()->clone();

      double runtime = emp::time_from(start_time);
      cout << "Runtime for projection: " <<  (runtime+0.0)*1e6*1e-9 << " secs." << endl;


      return projected;
  }

// input schema: study_years (0), age_strata (1), sex (2), ethnicity (3), race (4), numerator (5), denominator (6), denom_multisite (7), numerator_multisite (8)
void EnrichHtnQuery::aggregatePatients(SecureTable *src) {
    auto start_time = emp::clock_start();

    // sort it on cols [0,6)
    Sort sort(src, DataUtilities::getDefaultSortDefinition(5));
    SecureTable *sorted = sort.run();
    delete src;

    cout << "Finished sort for data cube.\n";


    std::vector<int32_t> groupByCols{0, 1, 2, 3, 4};
    std::vector<ScalarAggregateDefinition> aggregators {
            ScalarAggregateDefinition(5, AggregateId::SUM, "numerator_cnt"),
            ScalarAggregateDefinition(6, AggregateId::SUM, "denominator_cnt"),
            ScalarAggregateDefinition(7, AggregateId::SUM, "numerator_multisite"),
            ScalarAggregateDefinition(8, AggregateId::SUM, "denominator_multisite")
    };


    // output schema:
    // study_year (0), age_strata (1), sex (2), ethnicity (3) , race (4), numerator_cnt (5), denominator_cnt (6), numerator_multisite (7), denominator_multisite (8)
    GroupByAggregate aggregator(sorted, groupByCols, aggregators);
    data_cube_ = aggregator.run();
    delete sorted;

    if(cardinality_bound_ < data_cube_->getTupleCount()) {

        Shrinkwrap wrapper(data_cube_, cardinality_bound_);
        data_cube_ = wrapper.run();
        double runtime = emp::time_from(start_time);
        cout << "Runtime for aggregate #2 (data cube): " << (runtime + 0.0) * 1e6 * 1e-9 << " secs."
                          << endl;
    }

}


// input schema: study_year (0), age_strata (1), sex (2), ethnicity (3), race (4), numerator (5),  denominator (6), denom_multisite (7), numerator_multisite (8) - last 4 cols are ints
SecureTable *EnrichHtnQuery::aggregatePartialPatientCounts( SecureTable *src, const size_t & cardinality_bound) {
    auto start_time = emp::clock_start();


    // sort it on cols [0,6)
    Sort sort(src, DataUtilities::getDefaultSortDefinition(5));
    SecureTable *sorted = sort.run();


    std::vector<int32_t> groupByCols{0, 1, 2, 3, 4};
    std::vector<ScalarAggregateDefinition> aggregators {
            ScalarAggregateDefinition(5, AggregateId::SUM, "numerator_cnt"),
            ScalarAggregateDefinition(6, AggregateId::SUM, "denominator_cnt"),
            ScalarAggregateDefinition(7, AggregateId::SUM, "numerator_multisite"),
            ScalarAggregateDefinition(8, AggregateId::SUM, "denominator_multisite")
    };


    // output schema:
    // study_year (0), age_strata (1), sex (2), ethnicity (3) , race (4), numerator_cnt (5), denominator_cnt (6), numerator_multisite (7), denominator_multisite (8)
    GroupByAggregate aggregator(sorted, groupByCols, aggregators);
    SecureTable *dst = aggregator.run();
    delete sorted;


    Shrinkwrap wrapper(dst, cardinality_bound);
    dst = wrapper.run();
    double runtime = emp::time_from(start_time);


    cout << "Runtime for partial counts rollup: " <<  (runtime+0.0)*1e6*1e-9 << " secs at epoch " <<  Utilities::getEpoch() << endl;

    return dst;

}

// partials schema:
// study_year (0), age_strata (1), sex (2), ethnicity (3) , race (4), numerator_cnt (5), denominator_cnt (6), numerator_multisite (7), denominator_multisite (8)
void EnrichHtnQuery::unionWithPartialAggregates(vector<SecureTable *> partials) {
    SecureTable *summed_partials = addPartialAggregates(partials);

    Union<emp::Bit> union_op(data_cube_, summed_partials);
    data_cube_ = union_op.run();


    // basically a rerun of aggregatePatients
    // TODO: clean this up!

    // sort it on cols [0,5)
    Sort sort(data_cube_, DataUtilities::getDefaultSortDefinition(5));
    SecureTable *sorted = sort.run();

    std::vector<int32_t> groupByCols{0, 1, 2, 3, 4};
    std::vector<ScalarAggregateDefinition> aggregators {
            ScalarAggregateDefinition(5, AggregateId::SUM, "numerator_cnt"),
            ScalarAggregateDefinition(6, AggregateId::SUM, "denominator_cnt"),
            ScalarAggregateDefinition(7, AggregateId::SUM, "numerator_multisite"),
            ScalarAggregateDefinition(8, AggregateId::SUM, "denominator_multisite")
    };


    // output schema:
    // study_year (0), age_strata (1), sex (2), ethnicity (3) , race (4), numerator_cnt (5), denominator_cnt (6), numerator_multisite (7), denominator_multisite (8)
    GroupByAggregate aggregator(sorted, groupByCols, aggregators);
    data_cube_ = aggregator.run();
    delete sorted;

    if(cardinality_bound_ < data_cube_->getTupleCount()) {
        Shrinkwrap wrapper(data_cube_, cardinality_bound_);
        data_cube_ = wrapper.run();
    }



}

SecureTable *EnrichHtnQuery::addPartialAggregates(vector<SecureTable *> partials) {
    SecureTable *summed_partials = partials[0]->clone();
    size_t tuple_cnt = summed_partials->getTupleCount();

    for(size_t i = 1; i < partials.size(); ++i) {
        SecureTable *partial = partials[i];
        assert(tuple_cnt == partial->getTupleCount()); // check that they line up
        assert(QuerySchema::toPlain(partial->getSchema()) == SharedSchema::getPartialCountSchema());
    }

    // for each tuple
    for(size_t i = 0; i < tuple_cnt; ++i) {
        //SecureTuple dst = (*summed_partials)[i];

        // for each partial count
        for(size_t j = 1; j < partials.size(); ++j) {
           // SecureTuple src = (*(partials[j]))[i];

            // if one of them is not a dummy, then it's not a dummy
            summed_partials->setDummyTag(i, partials[j]->getDummyTag(i) | summed_partials->getDummyTag(i));

            for(int k = 5; k < 9; ++k) {
                summed_partials->setField(i, k, summed_partials->getField(i, k) + partials[j]->getField(i, k));
            }

//            dst.setField(5, dst[5] + src[5]);
//            dst.setField(6, dst[6] + src[6]);
//            dst.setField(7, dst[7] + src[7]);
//            dst.setField(8, dst[8] + src[8]);


        }
    }
    return summed_partials;
}

EnrichHtnQuery::EnrichHtnQuery(vector<SecureTable *> input) {
    data_cube_ = addPartialAggregates(input);
}



