#include "enrich_htn_query.h"
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


EnrichHtnQuery::EnrichHtnQuery(shared_ptr<SecureTable> & input, const size_t & cardinality) : input_table_(input), cardinality_bound_(cardinality) {

  // takes in shared_schema
    shared_ptr<SecureTable> filtered = filterPatients();
    shared_ptr<SecureTable> projected = projectPatients(filtered);
    auto logger = vaultdb_logger::get();
    BOOST_LOG(logger) << "Completed deduplication and exclusion at epoch " << Utilities::getEpoch() << endl;
    aggregatePatients(projected);
    filtered.reset();
    projected.reset();

}


// input schema: study_year (0), pat_id (1),  age_strata (2), sex (3), ethnicity (4), race (5), numerator (6), denominator (7), denom_excl (8)
shared_ptr<SecureTable> EnrichHtnQuery::filterPatients() {
    auto logger = vaultdb_logger::get();

    // sort it on group-by cols to prepare for aggregate
    // TODO: integrate this with the unioning step.  See UnionHybridData for more on this
    // patid, age_days, sex, ethnicity, race
    auto start_time = emp::clock_start();

    SortDefinition unionSortDefinition = DataUtilities::getDefaultSortDefinition(6);

    // destructor handled within Operator
    Sort<emp::Bit> sortUnioned(input_table_, unionSortDefinition);
    shared_ptr<SecureTable> sorted = sortUnioned.run();

    
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
    shared_ptr<SecureTable> aggregated = unionedPatients.run();

    double runtime = emp::time_from(start_time);
    BOOST_LOG(logger) << "Runtime for aggregate #1 (patid): " <<  (runtime+0.0)*1e6*1e-9 << " secs." << endl;
    sorted.reset();

    // filter ones with denom_excl = 1
    // *** Filter
    // HAVING max(denom_excl) = false

    shared_ptr<ExpressionNode<emp::Bit> > zero(new LiteralNode<emp::Bit>(Field<emp::Bit>(FieldType::SECURE_BOOL, emp::Bit(false))));;
    shared_ptr<ExpressionNode<emp::Bit> > input(new InputReferenceNode<emp::Bit>(9));
    shared_ptr<ExpressionNode<emp::Bit> > equality(new EqualNode<emp::Bit>(input, zero));

    BoolExpression<emp::Bit> equality_expr(equality);


    start_time = emp::clock_start();

    Filter inclusionCohort(aggregated, equality_expr);

    shared_ptr<SecureTable> output =   inclusionCohort.run();
    runtime = emp::time_from(start_time);
    BOOST_LOG(logger) << "Runtime for filter: " <<  (runtime+0.0)*1e6*1e-9 << " secs." << endl;

    aggregated.reset();
    return output;




}

  
  //  input schema: study_year (0), pat_id (1),  age_strata (2), sex (3), ethnicity (4), race (5), max(numerator) (6), max(denominator) (7),  site_count (8), max(denom_excl) (9)
  //  output schema: age_strata (0), sex (1), ethnicity (2), race (3),
  //  max(p.numerator) numerator (4),   max(p.numerator) denominator (5),
  //  COUNT(*) > 1 ^ numerator numerator_multisite (6)
  //  COUNT(*) > 1 ^ denominator denom_multisite (7),

  shared_ptr<SecureTable> EnrichHtnQuery::projectPatients(const shared_ptr<SecureTable> &src) {

    // *** Project on aggregate outputs:
    //     CASE WHEN count(*) > 1 THEN 1 else 0 END AS multisite
    //    CASE WHEN MAX(numerator)=1 ^ COUNT(*) > 1 THEN 1 ELSE 0 END AS numerator_multisite

    ExpressionMapBuilder<emp::Bit> builder(*src->getSchema());
    builder.addMapping(0, 0);
    for(int i = 2; i < 6; ++i) {
            // push back age_strata, sex, ethnicity, race
            builder.addMapping(i, i-1);
    }

      // cast numerator to INT for summing in next step
      // references (5)

      shared_ptr<ExpressionNode<emp::Bit> > numerator(new InputReferenceNode<emp::Bit>(6));
      shared_ptr<ExpressionNode<emp::Bit> > castNumerator(new CastNode<emp::Bit>(numerator, FieldType::SECURE_INT));
      shared_ptr<Expression<emp::Bit> >  numeratorToIntExpression(new GenericExpression<emp::Bit>(castNumerator, "numerator", FieldType::SECURE_INT));

      shared_ptr<ExpressionNode<emp::Bit> > denominator(new InputReferenceNode<emp::Bit>(7));
      shared_ptr<ExpressionNode<emp::Bit> > castDenominator(new CastNode<emp::Bit>(denominator, FieldType::SECURE_INT));
      shared_ptr<Expression<emp::Bit> >  denominatorToIntExpression(new GenericExpression<emp::Bit>(castDenominator, "denominator", FieldType::SECURE_INT));


    // references (6) (9)
    shared_ptr<Expression<emp::Bit> >  multisiteNumeratorExpression(new FunctionExpression(&EnrichHtnQuery::projectNumeratorMultisite<emp::Bit>, "numerator_multisite", FieldType::SECURE_INT));
      // references (7) (9)
      shared_ptr<Expression<emp::Bit> >  multisiteDenominatorExpression(new FunctionExpression(&EnrichHtnQuery::projectDenominatorMultisite<emp::Bit>, "denominator_multisite", FieldType::SECURE_INT));

    builder.addExpression(numeratorToIntExpression, 5);
    builder.addExpression(denominatorToIntExpression, 6);

    builder.addExpression(multisiteNumeratorExpression, 7);
      builder.addExpression(multisiteDenominatorExpression, 8);

      auto start_time = emp::clock_start();
      auto logger = vaultdb_logger::get();

      Project project(src, builder.getExprs());

    shared_ptr<SecureTable> projected =  project.run();

      double runtime = emp::time_from(start_time);
      BOOST_LOG(logger) << "Runtime for projection: " <<  (runtime+0.0)*1e6*1e-9 << " secs." << endl;


      return projected;
  }

// input schema: study_years (0), age_strata (1), sex (2), ethnicity (3), race (4), numerator (5), denominator (6), denom_multisite (7), numerator_multisite (8)
void EnrichHtnQuery::aggregatePatients( shared_ptr<SecureTable> &src) {
    auto start_time = emp::clock_start();
    auto logger = vaultdb_logger::get();

    // sort it on cols [0,6)
    Sort sort(src, DataUtilities::getDefaultSortDefinition(5));
    shared_ptr<SecureTable> sorted = sort.run();
    src.reset();

    Logger::write("Finished sort for data cube.");


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
    sorted.reset();

    if(cardinality_bound_ < data_cube_->getTupleCount()) {

        Shrinkwrap wrapper(data_cube_, cardinality_bound_);
        data_cube_ = wrapper.run();
        double runtime = emp::time_from(start_time);
        BOOST_LOG(logger) << "Runtime for aggregate #2 (data cube): " << (runtime + 0.0) * 1e6 * 1e-9 << " secs."
                          << endl;
    }

}


// input schema: study_year (0), age_strata (1), sex (2), ethnicity (3), race (4), numerator (5),  denominator (6), denom_multisite (7), numerator_multisite (8) - last 4 cols are ints
shared_ptr<SecureTable> EnrichHtnQuery::aggregatePartialPatientCounts( shared_ptr<SecureTable> &src, const size_t & cardinality_bound) {
    auto start_time = emp::clock_start();
    auto logger = vaultdb_logger::get();


    // sort it on cols [0,6)
    Sort sort(src, DataUtilities::getDefaultSortDefinition(5));
    shared_ptr<SecureTable> sorted = sort.run();


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
    shared_ptr<SecureTable> dst = aggregator.run();
    sorted.reset();


    Shrinkwrap wrapper(dst, cardinality_bound);
    dst = wrapper.run();
    double runtime = emp::time_from(start_time);


    BOOST_LOG(logger) << "Runtime for partial counts rollup: " <<  (runtime+0.0)*1e6*1e-9 << " secs at epoch " <<  Utilities::getEpoch() << endl;

    return dst;

}

// partials schema:
// study_year (0), age_strata (1), sex (2), ethnicity (3) , race (4), numerator_cnt (5), denominator_cnt (6), numerator_multisite (7), denominator_multisite (8)
void EnrichHtnQuery::unionWithPartialAggregates(vector<shared_ptr<SecureTable>> partials) {
    shared_ptr<SecureTable> summed_partials = addPartialAggregates(partials);

    Union<emp::Bit> union_op(data_cube_, summed_partials);
    data_cube_ = union_op.run();


    // basically a rerun of aggregatePatients
    // TODO: clean this up!

    // sort it on cols [0,5)
    Sort sort(data_cube_, DataUtilities::getDefaultSortDefinition(5));
    shared_ptr<SecureTable> sorted = sort.run();

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
    sorted.reset();

    if(cardinality_bound_ < data_cube_->getTupleCount()) {
        Shrinkwrap wrapper(data_cube_, cardinality_bound_);
        data_cube_ = wrapper.run();
    }



}

shared_ptr<SecureTable> EnrichHtnQuery::addPartialAggregates(vector<shared_ptr<SecureTable>> partials) {
    shared_ptr<SecureTable> summed_partials(new SecureTable(*partials[0]));
    size_t tuple_cnt = summed_partials->getTupleCount();

    for(size_t i = 1; i < partials.size(); ++i) {
        shared_ptr<SecureTable> partial = partials[i];
        assert(tuple_cnt == partial->getTupleCount()); // check that they line up
        assert(QuerySchema::toPlain(*(partial->getSchema())) == SharedSchema::getPartialCountSchema());
    }

    // for each tuple
    for(size_t i = 0; i < tuple_cnt; ++i) {
        SecureTuple dst = (*summed_partials)[i];

        // for each partial count
        for(size_t j = 1; j < partials.size(); ++j) {
            SecureTuple src = (*(partials[j]))[i];

            // if one of them is not a dummy, then it's not a dummy
            dst.setDummyTag(src.getDummyTag() | dst.getDummyTag());


            dst.setField(5, dst[5] + src[5]);
            dst.setField(6, dst[6] + src[6]);
            dst.setField(7, dst[7] + src[7]);
            dst.setField(8, dst[8] + src[8]);


        }
    }
    return summed_partials;
}

EnrichHtnQuery::EnrichHtnQuery(vector<shared_ptr<SecureTable>> &input) {
    data_cube_ = addPartialAggregates(input);
}



