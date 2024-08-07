#include <gtest/gtest.h>
#include <gflags/gflags.h>
#include <operators/sort.h>
#include <test/ompc/ompc_base_test.h>
#include <operators/keyed_join.h>
#include <operators/project.h>
#include "util/field_utilities.h"
#include <operators/stored_table_scan.h>

#if __has_include("emp-rescu/emp-rescu.h")


DEFINE_int32(party, 1, "party for EMP execution");
DEFINE_int32(port, 43440, "port for EMP execution");
DEFINE_string(unioned_db, "tpch_unioned_150", "unioned db name");
DEFINE_int32(cutoff, 10, "limit clause for queries");
DEFINE_int32(ctrl_port, 65440, "port for managing EMP control flow by passing public values");
DEFINE_bool(validation, true, "run reveal for validation, turn this off for benchmarking experiments (default true)");
DEFINE_string(filter, "*", "run only the tests passing this filter");
DEFINE_string(storage, "wire_packed", "storage model for columns (column or wire_packed)");
DEFINE_string(empty_db, "tpch_empty", "empty db name for schemas");
DEFINE_string(wire_path, "wires", "local path to wire files");
DEFINE_int32(input_party, 10086, "party for input data");
DEFINE_int32(unpacked_page_size_bits, 2048, "unpacked page size in bits");
DEFINE_int32(page_cnt, 50, "number of pages in buffer pool");

using namespace vaultdb;
using namespace Logging;

class OMPCKeyedJoinTest : public OmpcBaseTest {
protected:

    const int customer_limit_ = 150; // -1
    const int orders_limit_ = 200; // smaller: 30
    const int lineitem_limit_ = 500; // smaller: 90

    const std::string customer_sql_ = "SELECT c_custkey \n" // ignore c_mktsegment <> 'HOUSEHOLD' cdummy for now
                                      "FROM customer \n"
                                      "ORDER BY c_custkey \n"
                                      "LIMIT " + std::to_string(customer_limit_);

    const std::string orders_sql_ = "SELECT o_orderkey, o_custkey, o_orderdate, o_shippriority \n" // ignore o_orderdate >= date '1995-03-25' odummy for now
                                    "FROM orders \n"
                                    "ORDER BY o_orderkey \n"
                                    "LIMIT " + std::to_string(orders_limit_);

    const std::string lineitem_sql_ = "SELECT  l_orderkey, l_extendedprice, l_discount \n" // ignore l_shipdate <= date '1995-03-25' ldummy for now
                                      "FROM lineitem \n"
                                      "ORDER BY l_orderkey, l_linenumber \n"
                                      "LIMIT " + std::to_string(lineitem_limit_);

    Operator<Bit> *getCustomers();
    Operator<Bit> *getOrders();
    Operator<Bit> *getLineitem();

};

Operator<Bit> *OMPCKeyedJoinTest::getCustomers() {
    SystemConfiguration & conf = SystemConfiguration::getInstance();

//    if(conf.storageModel() == StorageModel::PACKED_COLUMN_STORE) {
        return new StoredTableScan<Bit>("customer", "c_custkey", customer_limit_);
        //auto scan = new StoredTableScan<Bit>("customer", "c_custkey, c_mktsegment", customer_limit_);
/*
        // filter
        Integer s = Field<Bit>::secretShareString("HOUSEHOLD ", conf.inputParty(), conf.input_party_, 10);
        Field<Bit> sf(FieldType::SECURE_STRING, s);
        auto schema = scan->getOutputSchema();
        auto predicate = FieldUtilities::getComparisonWithLiteral(schema, sf,1, ExpressionKind::EQ);
        auto filter = new Filter<Bit>(scan, predicate);

        // project it down to the c_custkey
        auto proj = OperatorUtilities::getProjectionFromColNames(filter, "c_custkey");

        return proj;*/
//    }

    // else
//    Operator<Bit> *input  = new SecureSqlInput(db_name_, customer_sql_, true, {ColumnSort(0, SortDirection::ASCENDING)}, customer_limit_);
//    return input;
}

Operator<Bit> *OMPCKeyedJoinTest::getOrders() {
//    SystemConfiguration &conf = SystemConfiguration::getInstance();
//    if(conf.storageModel() == StorageModel::PACKED_COLUMN_STORE) {
        // Project orders table to o_orderkey, o_custkey, o_orderdate, o_shippriority
        vector<int> ordinals = {0, 1, 4, 7};
        auto scan = new StoredTableScan<Bit>("orders", ordinals, orders_limit_);
        return scan;
        /*
        // filter: o_orderdate < date '1995-03-25'
        // $4 < 796089600 ($2 after projection)
        auto schema = scan->getOutputSchema();
        int64_t date =  796089600L;
        PlainField p(FieldType::LONG, date);
        SecureField s = Field<Bit>::secretShareHelper(p, schema.getField(2), conf.input_party_, conf.inputParty());
        auto predicate = FieldUtilities::getComparisonWithLiteral(schema, 2, s, ExpressionKind::LT);

        auto filter = new Filter<Bit>(scan, predicate);

        SecureSqlInput test(FLAGS_unioned_db, orders_sql_, true, {ColumnSort(0, SortDirection::ASCENDING)}, orders_limit_);
        auto test_res = test.run()->revealInsecure();
        DataUtilities::removeDummies(test_res);



        return filter; */
//    }

    // else
//    Operator<Bit> *input  = new SecureSqlInput(db_name_, orders_sql_, true, {ColumnSort(0, SortDirection::ASCENDING)}, orders_limit_);
//    return input;

}

Operator<Bit> *OMPCKeyedJoinTest::getLineitem() {
    SystemConfiguration & conf = SystemConfiguration::getInstance();
//    if(conf.storageModel() == StorageModel::PACKED_COLUMN_STORE) {
      //  auto scan = new StoredTableScan<Bit>("lineitem", "l_orderkey, l_extendedprice, l_discount", lineitem_limit_);
return  new StoredTableScan<Bit>("lineitem", "l_orderkey, l_extendedprice, l_discount", lineitem_limit_);
        //auto scan = new StoredTableScan<Bit>("lineitem", "l_orderkey, l_extendedprice, l_discount, l_shipdate", lineitem_limit_);
       /* auto schema = scan->getOutputSchema();

        // Project lineitem table to l_orderkey, l_extendedprice * (1 - l_discount) revenue, l_shipdate
        ExpressionMapBuilder<Bit> lineitem_builder(schema);
        lineitem_builder.addMapping(0, 0);
        auto extendedprice_field = new InputReference<emp::Bit>(1, schema);
        auto discount_field = new InputReference<emp::Bit>(2, schema);
        auto one_literal = new LiteralNode(Field<Bit>(FieldType::SECURE_FLOAT, emp::Float(1.0)));
        auto one_minus_discount = new MinusNode<emp::Bit>(one_literal, discount_field);
        auto extended_price_times_discount = new TimesNode<emp::Bit>(extendedprice_field, one_minus_discount);
        Expression<emp::Bit> *revenue_expr = new GenericExpression<emp::Bit>(extended_price_times_discount, "revenue", FieldType::SECURE_FLOAT);
        lineitem_builder.addExpression(revenue_expr, 1);
//        lineitem_builder.addMapping(3, 2); // l_shipdate

        auto proj = new Project(scan, lineitem_builder.getExprs());
        return proj;

      // filter: l_shipdate > date '1995-03-25'
        schema = proj->getOutputSchema();
        int64_t date =  796089600L;
        PlainField p(FieldType::LONG, date);
        SecureField s = Field<Bit>::secretShareHelper(p, schema.getField(2), conf.input_party_, conf.inputParty());
        auto predicate = FieldUtilities::getComparisonWithLiteral(schema, 2, s, ExpressionKind::GT);
        auto filter = new Filter<Bit>(proj, predicate);

        // chop off l_shipdate
        ExpressionMapBuilder<Bit> builder(schema);
        builder.addMapping(0, 0);
        builder.addMapping(1, 1);
        auto proj2 = new Project(filter, builder.getExprs());
        return proj2;
*/
//    }

    // else
//    Operator<Bit> *input  = new SecureSqlInput(db_name_, lineitem_sql_, true, {ColumnSort(0, SortDirection::ASCENDING)}, lineitem_limit_);
//    return input;

}


TEST_F(OMPCKeyedJoinTest, test_tpch_q3_customer_orders) {
    std::string sql = "WITH orders_cte AS (" + orders_sql_ + "), "
                           "customer_cte AS (" + customer_sql_ + ") "
                      "SELECT o_orderkey, o_custkey, o_orderdate, o_shippriority, c_custkey "
                      "FROM orders_cte, customer_cte "
                      "WHERE o_custkey = c_custkey "
                      "ORDER BY o_orderkey, o_custkey, o_orderdate, o_shippriority, c_custkey"; // ignore NOT cdummy AND NOT odummy for now

    Operator<Bit> *orders = getOrders();
    Operator<Bit> *customers = getCustomers();

    // test fkey-pkey join
    // join output schema: (orders, customer)
    // o_orderkey, o_custkey, o_orderdate, o_shippriority, c_custkey
    Expression<emp::Bit> *predicate = FieldUtilities::getEqualityPredicate<emp::Bit>(orders, 1, customers, 4);
    KeyedJoin<emp::Bit> *join = new KeyedJoin(orders, customers, predicate);
    time_point<high_resolution_clock> querying_start_time = high_resolution_clock::now();
    SecureTable *join_res = join->run();
    double querying_runtime = time_from(querying_start_time)/1e6;
    cout << "querying time: " << querying_runtime << "s" << endl;
    Utilities::checkMemoryUtilization(true);

    if(FLAGS_validation) {
        SortDefinition sort_def = DataUtilities::getDefaultSortDefinition(join->getOutputSchema().getFieldCount());
        PlainTable *observed = join_res->revealInsecure();
        DataUtilities::removeDummies(observed);
        Sort<bool> sorter(observed, sort_def);
        observed = sorter.run();

        PlainTable *expected = DataUtilities::getQueryResults(FLAGS_unioned_db, sql,false);
        expected->order_by_ = sort_def;

        ASSERT_EQ(*expected, *observed);
    }

}

TEST_F(OMPCKeyedJoinTest, test_tpch_q3_lineitem_orders) {
    // get inputs from local oblivious ops
    // first 3 customers, propagate this constraint up the join tree for the test
    std::string sql = "WITH orders_cte AS (" + orders_sql_ + "), "
                           "lineitem_cte AS (" + lineitem_sql_ + ") "
                      "SELECT l_orderkey, l_extendedprice, l_discount, o_orderkey, o_custkey, o_orderdate, o_shippriority "
                      "FROM lineitem_cte, orders_cte "
                      "WHERE l_orderkey = o_orderkey "
                      "ORDER BY l_orderkey, l_extendedprice, l_discount, o_orderkey, o_custkey, o_orderdate, o_shippriority"; // ignore NOT odummy AND NOT ldummy for now

    Operator<Bit> *lineitem = getLineitem();
    Operator<Bit> *orders = getOrders();

    // test pkey-fkey join
    // join output schema: (lineitem, orders)
    // l_orderkey, l_extendedprice, l_discount, o_orderkey, o_custkey, o_orderdate, o_shippriority
    Expression<emp::Bit> * predicate = FieldUtilities::getEqualityPredicate<emp::Bit>(lineitem, 0, orders, 3);
    KeyedJoin<emp::Bit> *join = new KeyedJoin(lineitem, orders, predicate);
    time_point<high_resolution_clock> querying_start_time = high_resolution_clock::now();
    SecureTable *join_res = join->run();
    double querying_runtime = time_from(querying_start_time)/1e6;
    cout << "querying time: " << querying_runtime << "s" << endl;
    Utilities::checkMemoryUtilization(true);

    if(FLAGS_validation) {
        SortDefinition sort_def = DataUtilities::getDefaultSortDefinition(join->getOutputSchema().getFieldCount());
        PlainTable  *observed = join_res->revealInsecure();
        DataUtilities::removeDummies(observed);
        Sort<bool> sorter(observed, sort_def);
        observed = sorter.run();

        PlainTable *expected = DataUtilities::getQueryResults(FLAGS_unioned_db, sql, false);
        expected->order_by_ = sort_def;

        ASSERT_EQ(*expected, *observed);
    }


}

// compose C-O-L join
TEST_F(OMPCKeyedJoinTest, test_tpch_q3_lineitem_orders_customer) {
    std::string sql = "WITH lineitem_cte AS (" + lineitem_sql_ + "), "
                           "orders_cte AS (" + orders_sql_ + "), "
                           "customer_cte AS (" + customer_sql_ + ") "
                     "SELECT l_orderkey, l_extendedprice, l_discount, o_orderkey, o_custkey, o_orderdate, o_shippriority, c_custkey "
                     "FROM lineitem_cte, orders_cte, customer_cte "
                     "WHERE l_orderkey = o_orderkey AND c_custkey = o_custkey "
                     "ORDER BY l_orderkey, l_extendedprice, l_discount, o_orderkey, o_custkey, o_orderdate, o_shippriority, c_custkey"; // ignore NOT odummy AND NOT ldummy AND NOT cdummy for now

    Operator<Bit> *orders = getOrders();
    Operator<Bit> *customers = getCustomers();

    // join output schema: (orders, customer)
    // o_orderkey, o_custkey, o_orderdate, o_shippriority, c_custkey
    Expression<emp::Bit> * customer_orders_predicate = FieldUtilities::getEqualityPredicate<emp::Bit>(orders, 1,customers,4);
    KeyedJoin<Bit> *co_join = new KeyedJoin(orders, customers, customer_orders_predicate);

    Operator<Bit> *lineitem = getLineitem();

    // join output schema:
    //  l_orderkey, l_extendedprice, l_discount, o_orderkey, o_custkey, o_orderdate, o_shippriority, c_custkey
    Expression<emp::Bit> * lineitem_orders_predicate = FieldUtilities::getEqualityPredicate<emp::Bit>( lineitem, 0, co_join, 3);
    KeyedJoin<Bit> *col_join = new KeyedJoin(lineitem, co_join, lineitem_orders_predicate);
    time_point<high_resolution_clock> querying_start_time = high_resolution_clock::now();
    SecureTable *join_res = col_join->run();
    double querying_runtime = time_from(querying_start_time)/1e6;
    cout << "querying time: " << querying_runtime << "s" << endl;
    Utilities::checkMemoryUtilization(true);

    if(FLAGS_validation) {
        SortDefinition sort_def = DataUtilities::getDefaultSortDefinition(col_join->getOutputSchema().getFieldCount());
        PlainTable *observed = join_res->revealInsecure();
        DataUtilities::removeDummies(observed);
        Sort<bool> sorter(observed, sort_def);
        observed = sorter.run();

        PlainTable *expected = DataUtilities::getQueryResults(FLAGS_unioned_db, sql, false);
        expected->order_by_ = sort_def;

        ASSERT_EQ(*expected, *observed);
    }
}










int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    gflags::ParseCommandLineFlags(&argc, &argv, false);

    ::testing::GTEST_FLAG(filter)=FLAGS_filter;
    int i = RUN_ALL_TESTS();
    google::ShutDownCommandLineFlags();
    return i;

}

#else
int main(int argc, char **argv) {
    std::cout << "emp-rescu backend not found!" << std::endl;
}

#endif


