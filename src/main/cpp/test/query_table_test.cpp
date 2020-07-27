//
// Created by Jennie Rogers on 7/18/20.
//

#include "secure_aggregate.h"
#include "secure_sort.h"
#include "querytable/private_share_utility.h"

#include <data/PsqlDataProvider.h>
#include <gflags/gflags.h>
#include <gtest/gtest.h>
#include <emp-sh2pc/emp-sh2pc.h>

using namespace emp;
using namespace std;


DEFINE_int32(party, 1, "party for EMP execution");
DEFINE_int32(port, 43439, "port for EMP execution");
DEFINE_string(hostname, "127.0.0.1", "alice hostname for execution");


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
                                              "EXTRACT(EPOCH FROM l_commitdate) AS l_commitdate "  // handle timestamps by converting them to longs using SQL - "CAST(EXTRACT(EPOCH FROM l_commitdate) AS BIGINT) AS l_commitdate,
                                              "FROM lineitem "
                                              "ORDER BY l_orderkey "
                                              "LIMIT 10";
        return inputQuery;
    }

    static const std::string getExpectedOutput() {

        /*
         * Expected output:
         * tpch_alice=# SELECT l_orderkey, l_comment, l_returnflag, l_discount, EXTRACT(EPOCH FROM l_commitdate) AS l_commitdate FROM lineitem ORDER BY l_orderkey LIMIT 10;
          1 |  pending foxes. slyly re                   | N            |       0.10 |    826761600
          1 | arefully slyly ex                          | N            |       0.07 |    823651200
          3 | nal foxes wake.                            | A            |       0.06 |    753926400
          5 | ts wake furiously                          | R            |       0.02 |    778291200
          5 | sts use slyly quickly special instruc      | R            |       0.07 |    780451200
          7 | es. instructions                           | N            |       0.08 |    825724800
          7 |  unusual reques                            | N            |       0.10 |    827884800
          7 | . slyly special requests haggl             | N            |       0.03 |    828921600
          7 | jole. excuses wake carefully alongside of  | N            |       0.06 |    825033600
         32 | lithely regular deposits. fluffily         | N            |       0.02 |    813024000
(10 rows)
         */

        static const std::string queryOutput = "(#0 int32 lineitem.l_orderkey, #1 varchar(44) lineitem.l_comment, #2 varchar(1) lineitem.l_returnflag, #3 float lineitem.l_discount, #4 double lineitem.l_commitdate) isEncrypted? 0\n"
                                               "(1,  pending foxes. slyly re, N, 0.100000, 826761600.000000)\n"
                                               "(1, arefully slyly ex, N, 0.070000, 823651200.000000)\n"
                                               "(3, nal foxes wake., A, 0.060000, 753926400.000000)\n"
                                               "(5, ts wake furiously , R, 0.020000, 778291200.000000)\n"
                                               "(5, sts use slyly quickly special instruc, R, 0.070000, 780451200.000000)\n"
                                               "(7, es. instructions, N, 0.080000, 825724800.000000)\n"
                                               "(7,  unusual reques, N, 0.100000, 827884800.000000)\n"
                                               "(7, . slyly special requests haggl, N, 0.030000, 828921600.000000)\n"
                                               "(7, jole. excuses wake carefully alongside of , N, 0.060000, 825033600.000000)\n"
                                               "(32, lithely regular deposits. fluffily , N, 0.020000, 813024000.000000)\n";

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

        static const std::string inputQueryDummyTag = "SELECT l_orderkey, l_comment, l_returnflag, l_discount, "
                                              "EXTRACT(EPOCH FROM l_commitdate) AS l_commitdate, "  // handle timestamps by converting them to longs using SQL - "CAST(EXTRACT(EPOCH FROM l_commitdate) AS BIGINT) AS l_commitdate,
                                              "l_returnflag = 'N' AS dummy "
                                              "FROM lineitem "
                                              "ORDER BY l_orderkey "
                                              "LIMIT 10";
        return inputQueryDummyTag;
    }

    virtual void SetUp() { }
};


class query_table_test : public ::testing::Test {


protected:
    void SetUp() override {};
    void TearDown() override{};
};

// tests how we handle each type
// also validates overload of << operator
TEST_F(query_table_test, read_table) {


    PsqlDataProvider dataProvider;
    string db_name =  FLAGS_party == emp::ALICE ? "tpch_alice" : "tpch_bob";

    cout << "Querying " << db_name << " with: " << QueryTableTestEnvironment::getInputQuery() << endl;




    std::unique_ptr<QueryTable>  inputTable = dataProvider.GetQueryTable(db_name,
                                        QueryTableTestEnvironment::getInputQuery(), "lineitem", false);


    QueryTable * local = inputTable.get();

    string observedTable = inputTable.get()->toString();
    string expectedTable = QueryTableTestEnvironment::getExpectedOutput();

    cout << "Expected:\n" << expectedTable << endl;
    cout << "Observed: \n" << *local << endl;

    ASSERT_EQ(expectedTable, observedTable) << "Query table was not parsed correctly.";



}


// tests handling of a dummy tag from SQL query
TEST_F(query_table_test, read_table_dummy_tag) {


    PsqlDataProvider dataProvider;
    AggregateDef aggDef;
    ShareDef def;

    string db_name =  FLAGS_party == emp::ALICE ? "tpch_alice" : "tpch_bob";

    cout << "Querying " << db_name << " with: " << QueryTableTestEnvironment::getInputQueryDummyTag() << endl;




    std::unique_ptr<QueryTable>  inputTable = dataProvider.GetQueryTable(db_name,
                                                                         QueryTableTestEnvironment::getInputQuery(),
                                                                         "lineitem",
                                                                         true);


    cout << "Received: " << inputTable.get() << endl;
}


// test encrypting the query table with EMP
TEST_F(query_table_test, encrypt_table) {


    PsqlDataProvider dataProvider;
    AggregateDef aggDef;
    ShareDef def;

    emp::NetIO *io = new emp::NetIO(
            FLAGS_party == emp::ALICE ? nullptr : FLAGS_hostname.c_str(), FLAGS_port);
    setup_semi_honest(io, FLAGS_party);


    EmpParty party =
            FLAGS_party == emp::ALICE ? EmpParty::ALICE : EmpParty::BOB;

    string db_name =  FLAGS_party == emp::ALICE ? "tpch_alice" : "tpch_bob";


    // selecting one of each type:
    // int
    // varchar
    // fixed char
    // numeric
    // date
    // dummy tag



    auto inputTuples = dataProvider.GetQueryTable("dbname=" + db_name,
                                        QueryTableTestEnvironment::getInputQuery(), "lineitem", false);

    ShareCount ca = {.party = EmpParty::ALICE};
    ca.num_tuples = inputTuples->GetNumTuples();

    ShareCount cb = {.party = EmpParty::BOB};
    cb.num_tuples = inputTuples->GetNumTuples();

    def.share_map[EmpParty::ALICE] = ca;
    def.share_map[EmpParty::BOB] = cb;

    auto encryptedInputTuples =
            ShareData(inputTuples->GetSchema(), party, inputTuples.get(), def);

   /* std::unique_ptr<QueryTable> decrypted = encryptedInputTuples->reveal(EmpParty::PUBLIC);
    std::unique_ptr<QueryTable> expected = dataProvider.GetQueryTable("dbname=tpch_unioned",
                                                            QueryTableTestEnvironment::getInputQuery(), "lineitem", true);


    //std::cout << "Decrypted: " << decrypted << endl;
    //std::cout << "Expected: "  << expected << endl;

    assert((decrypted.get()) == (expected.get()));*/
}
