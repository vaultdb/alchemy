#include <gflags/gflags.h>
#include <gtest/gtest.h>
#include <util/type_utilities.h>
#include <stdexcept>
#include <operators/sql_input.h>
#include <data/csv_reader.h>
#include "plain/plain_base_test.h"
#include <regex>

DEFINE_string(filter, "*", "run only the tests passing this filter");
DEFINE_string(storage, "column", "storage model for columns (column or compressed)");


// set up this test by running:
// bash test/support/setup-csv.sh

class CsvReaderTest : public PlainBaseTest  {
protected:
    QueryFieldDesc convertDateField(const QueryFieldDesc & srcField) {
        return QueryFieldDesc(srcField, FieldType::DATE);
    }
};





TEST_F(CsvReaderTest, lineitemTest) {

   // generated input file by running:
    std::string input_csv =  current_working_directory_ + "/test/support/csv/lineitem.csv";


    std::string sql = "SELECT  l_orderkey, l_partkey, l_suppkey, l_linenumber, l_quantity, l_extendedprice, l_discount, l_tax, l_returnflag, l_linestatus, l_shipdate, l_commitdate, l_receiptdate, l_shipinstruct, l_shipmode, l_comment FROM lineitem ORDER BY (1), (2)  LIMIT 50";

    auto expected = DataUtilities::getQueryResults(db_name_, sql);
    string schema_str = "(lineitem.l_orderkey:int32, lineitem.l_partkey:int32, lineitem.l_suppkey:int32, lineitem.l_linenumber:int32, lineitem.l_quantity:float, lineitem.l_extendedprice:float, lineitem.l_discount:float, lineitem.l_tax:float, lineitem.l_returnflag:varchar(1), lineitem.l_linestatus:varchar(1), lineitem.l_shipdate:date, lineitem.l_commitdate:date, lineitem.l_receiptdate:date, lineitem.l_shipinstruct:varchar(25), lineitem.l_shipmode:varchar(10), lineitem.l_comment:varchar(44))";

    QuerySchema csv_schema(schema_str);
    PlainTable *observed = CsvReader::readCsv(input_csv, csv_schema);
    observed->setSchema(expected->getSchema()); // switch back from date schema

    ASSERT_EQ(*expected, *observed);

    delete expected;
    delete observed;



}


TEST_F(CsvReaderTest, quotedStringTest) {
    // trick is handling ',' character correctly
    std::string test_str = "16,Customer#000000016,\"cYiaeMLZSMAOQ2 d0W,\",10,20-781-609-3107,4681.03,FURNITURE ,kly silent courts. thinly regular theodolites sleep fluffily after ,0";


    // grab customer table for schema:
    std::string query = "SELECT * FROM customer ORDER BY (1), (2)  LIMIT 16";
    PsqlDataProvider dataProvider;
    PlainTable *expected = dataProvider.getQueryTable(db_name_, query);
    // this is an early stage test, verify psql first
    string psql_validation = expected->getPlainTuple(15).getField(1).toString();
    ASSERT_EQ(psql_validation, "Customer#000000016       ");


    PlainTable *parse_test = expected->clone();
    CsvReader::parseTuple(test_str, expected->getSchema(), parse_test, 15);

    QueryTuple expected_tuple = expected->getPlainTuple(15);
    QueryTuple parsed_tuple = parse_test->getPlainTuple(15);

    std::string customer_id = parsed_tuple.getField(1).toString();
    ASSERT_EQ(customer_id, "Customer#000000016       ");

    // expected:
    // 16 | Customer#000000016 | cYiaeMLZSMAOQ2 d0W,                    |          10 | 20-781-609-3107 |   4681.03 | FURNITURE    | kly silent courts. thinly regular theodolites sleep fluffily after

    ASSERT_EQ(parsed_tuple, expected_tuple);
    delete expected;
    delete parse_test;
}

TEST_F(CsvReaderTest, customerTest) {

    // generated input file by running:
    // psql --csv  -t -c "SELECT * FROM customer ORDER BY (1), (2)  LIMIT 50" tpch_unioned  > test/support/csv/customer.csv

    std::string input_csv =  current_working_directory_ +  "/test/support/csv/customer.csv";
    std::string query = "SELECT c_custkey, c_name, c_address, c_nationkey, c_phone, c_acctbal, c_mktsegment, c_comment FROM customer ORDER BY (1), (2)  LIMIT 50";

    PsqlDataProvider dataProvider;
    PlainTable *expected = dataProvider.getQueryTable(db_name_, query);

    string schema_str = "(customer.c_custkey:int32, customer.c_name:varchar(25), customer.c_address:varchar(40), customer.c_nationkey:int32, customer.c_phone:varchar(15), customer.c_acctbal:float, customer.c_mktsegment:varchar(10), customer.c_comment:varchar(117))";
    QuerySchema csv_schema(schema_str);

    PlainTable *observed = CsvReader::readCsv(input_csv, csv_schema);

    ASSERT_EQ(*expected, *observed);

    delete expected;
    delete observed;


}


TEST_F(CsvReaderTest, ordersTest) {


    std::string input_csv = current_working_directory_ +  "/test/support/csv/orders.csv";
    std::string sql = "SELECT o_orderkey, o_custkey, o_orderstatus, o_totalprice, " +  DataUtilities::queryDatetime("o_orderdate") +  ", o_orderpriority, o_comment  FROM orders ORDER BY (1), (2)  LIMIT 50";

    PlainTable *expected = DataUtilities::getQueryResults(db_name_, sql);

    string orders_schema = "(orders.o_orderkey:int32, orders.o_custkey:int32, orders.o_orderstatus:varchar(1), orders.o_totalprice:float, orders.o_orderdate:date, orders.o_orderpriority:varchar(15), orders.o_comment:varchar(79))";
    QuerySchema csv_schema(orders_schema);
    PlainTable *observed = CsvReader::readCsv(input_csv, csv_schema);

    // RHS || observed is incorrect here.
    ASSERT_EQ(*expected, *observed);

    delete expected;
    delete observed;


}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    gflags::ParseCommandLineFlags(&argc, &argv, false);
    ::testing::GTEST_FLAG(filter)=FLAGS_filter;
    int i = RUN_ALL_TESTS();
    google::ShutDownCommandLineFlags();
    return i;
}
