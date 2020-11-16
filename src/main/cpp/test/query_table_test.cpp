#include <data/PsqlDataProvider.h>
#include <gflags/gflags.h>
#include <gtest/gtest.h>
#include <util/type_utilities.h>

using namespace emp;
using namespace vaultdb;


class QueryTableTestEnvironment : public ::testing::Environment {
public:
    // Assume there's only going to be a single instance of this class, so we can just
    // hold the timestamp as a const static local variable and expose it through a
    // static member function
    static std::string getInputQuery() {
        // selecting one of each type:
        // int
        // varchar
        // fixed char
        // numeric
        // date

        static const std::string inputQuery = "SELECT l_orderkey, l_comment, l_returnflag, l_discount, "
                                              " l_commitdate "  // handle timestamps by converting them to longs using SQL - "CAST(EXTRACT(EPOCH FROM l_commitdate) AS BIGINT) AS l_commitdate,
                                              "FROM lineitem "
                                              "ORDER BY l_shipdate "
                                              "LIMIT 10";
        return inputQuery;
    }

    static const std::string getExpectedOutput() {

        /*
         * Expected output:
         * tpch_unioned=# SELECT l_orderkey, l_comment, l_returnflag, l_discount,   CAST(EXTRACT(EPOCH FROM l_commitdate) AS BIGINT) l_commitdate  FROM lineitem ORDER BY l_shipdate LIMIT 10;
                 l_orderkey |                 l_comment                 | l_returnflag | l_discount | l_commitdate
                ------------+-------------------------------------------+--------------+------------+--------------
                    5018977 |  packages detect furiously quick          | A            |       0.00 |    700963200
                    3973414 | eodolites. carefully pending packages hag | R            |       0.00 |    699753600
                    5885633 | e ironic dolphins hag                     | R            |       0.00 |    700185600
                    2167527 |  foxes sleep blithely along the idle exc  | R            |       0.00 |    698371200
                     842980 | lly regular asymptotes. unu               | A            |       0.01 |    701049600
                    5431079 | xes are fu                                | R            |       0.03 |    701827200
                    1054181 | y unusual instructions. furiously reg     | R            |       0.03 |    697248000
                    5002593 | ss accounts. ironic, sp                   | R            |       0.05 |    701913600
                    2184032 |  even ideas breach slyly above the d      | A            |       0.06 |    698976000
                    3273383 | the carefully ironic accounts hin         | R            |       0.06 |    700963200
         */


        // N.B. l_commitdate has no table name because it is the output of an expression
        static const std::string queryOutput = "(#0 int32 lineitem.l_orderkey, #1 varchar(44) lineitem.l_comment, #2 varchar(1) lineitem.l_returnflag, #3 float lineitem.l_discount, #4 int64 .l_commitdate) isEncrypted? 0\n"
                                               "(5018977, ' packages detect furiously quick            ', 'A', 0, 700963200)\n"
                                               "(3973414, 'eodolites. carefully pending packages hag   ', 'R', 0, 699753600)\n"
                                               "(5885633, 'e ironic dolphins hag                       ', 'R', 0, 700185600)\n"
                                               "(2167527, ' foxes sleep blithely along the idle exc    ', 'R', 0, 698371200)\n"
                                               "(842980, 'lly regular asymptotes. unu                 ', 'A', 0.01, 701049600)\n"
                                               "(5431079, 'xes are fu                                  ', 'R', 0.03, 701827200)\n"
                                               "(1054181, 'y unusual instructions. furiously reg       ', 'R', 0.03, 697248000)\n"
                                               "(5002593, 'ss accounts. ironic, sp                     ', 'R', 0.05, 701913600)\n"
                                               "(2184032, ' even ideas breach slyly above the d        ', 'A', 0.06, 698976000)\n"
                                               "(3273383, 'the carefully ironic accounts hin           ', 'R', 0.06, 700963200)\n";

        return queryOutput;

    }

    static std::string getInputQueryDummyTag() {
        // selecting one of each type:
        // int
        // varchar
        // fixed char
        // numeric
        // date
        // dummy tag

        static const std::string inputQueryDummyTag = "SELECT l_orderkey, l_comment, l_returnflag, l_discount,  l_commitdate, l_shipinstruct <> 'NONE' AS dummy "
                                                      "FROM lineitem "
                                                      "ORDER BY l_shipdate "
                                                      "LIMIT 10";
        return inputQueryDummyTag;
    }


    static const std::string getExpectedOutputDummyTag() {

        /*
         * tpch_alice=# SELECT l_orderkey, l_comment, l_returnflag, l_discount,  l_commitdate, l_shipinstruct <> 'NONE' AS dummy FROM lineitem ORDER BY l_shipdate LIMIT 10;
 l_orderkey |                 l_comment                  | l_returnflag | l_discount | l_commitdate | dummy
------------+--------------------------------------------+--------------+------------+--------------+-------
     598849 | ar courts wake fluf                        | A            |       0.04 | 1992-03-14   | f
     359170 |  the accounts. slyly                       | R            |       0.08 | 1992-02-18   | t
     434081 | e. regular instructions among the doggedly | A            |       0.02 | 1992-03-06   | f
      27137 | s above th                                 | R            |       0.06 | 1992-02-18   | t
     301446 |  slyly even foxes. fluffily ironic req     | A            |       0.09 | 1992-02-03   | t
     555654 | fily in place of the evenly reg            | R            |       0.08 | 1992-03-05   | t
     530500 | gular foxes are fluffily across the iron   | A            |       0.01 | 1992-02-23   | t
     515077 | accounts use;                              | R            |       0.03 | 1992-03-21   | t
      27137 | lthy packages might                        | A            |       0.03 | 1992-02-24   | t
     358471 | kages nag carefully agains                 | R            |       0.03 | 1992-03-28   | t
(10 rows)

         */

        // N.B. l_commitdate has no table name because it is the output of an expression
       static const std::string queryOutput = "(#0 int32 lineitem.l_orderkey, #1 varchar(44) lineitem.l_comment, #2 varchar(1) lineitem.l_returnflag, #3 float lineitem.l_discount, #4 int64 .l_commitdate) isEncrypted? 0\n"
                                              "(5018977, ' packages detect furiously quick            ', 'A', 0, 700963200)\n"
                                              "(5431079, 'xes are fu                                  ', 'R', 0.03, 701827200)\n"
                                              "(1054181, 'y unusual instructions. furiously reg       ', 'R', 0.03, 697248000)\n";
        return queryOutput;

    }
    virtual void SetUp() {

    }

};


class QueryTableTest : public ::testing::Test {


protected:
    void SetUp() override {};
    void TearDown() override{};
};

// tests how we handle each type
// also validates overload of << operator
// TODO: refactor this to use syscall to invoke psql and dump query results to a CSV file.  Compare this to the outputs here to account for diffs in the TPC-H datagen process.
TEST_F(QueryTableTest, read_table) {


    PsqlDataProvider dataProvider;
    string db_name =  "tpch_unioned";

    std::string inputQuery = QueryTableTestEnvironment::getInputQuery();
    std::cout << "Querying " << db_name << " with: " << inputQuery << std::endl;




    std::unique_ptr<QueryTable>  inputTable = dataProvider.getQueryTable(db_name,
                                                                         inputQuery, false);


    string observedTable = inputTable->toString();
    string expectedTable = QueryTableTestEnvironment::getExpectedOutput();

    std::cout << "Expected:\n" << expectedTable << std::endl;
    std::cout << "Observed: \n" << observedTable << std::endl;

    ASSERT_EQ(expectedTable, observedTable) << "Query table was not parsed correctly.";



}


// tests handling of a dummy tag from SQL query
TEST_F(QueryTableTest, read_table_dummy_tag) {


    PsqlDataProvider dataProvider;

    string db_name =  "tpch_unioned";
    string expectedTable = QueryTableTestEnvironment::getExpectedOutputDummyTag();
    std::string inputQuery = QueryTableTestEnvironment::getInputQueryDummyTag();

    std::cout << "Querying " << db_name << " with: " << inputQuery  << std::endl;




    std::unique_ptr<QueryTable>  inputTable = dataProvider.getQueryTable(db_name,
                                                                         inputQuery,
                                                                         true);


    std::string observedTable = inputTable->toString();

    std::cout << "Expected:\n" << expectedTable << std::endl;
    std::cout << "Observed: \n" << observedTable << std::endl;

    ASSERT_EQ(expectedTable, observedTable) << "Query table was not parsed correctly.";
}



int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    gflags::ParseCommandLineFlags(&argc, &argv, false);

    return RUN_ALL_TESTS();
}

