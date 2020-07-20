//
// Created by Jennie Rogers on 7/18/20.
//

#include "secure_aggregate.h"
#include "secure_sort.h"
#include "querytable/private_share_utility.h"

#include <data/PsqlDataProvider.h>
#include <gflags/gflags.h>
#include <gtest/gtest.h>

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
        // dummy tag

        static const std::string inputQuery = "SELECT l_orderkey, l_comment, l_returnflag, l_discount, "
                                              "EXTRACT(EPOCH FROM l_commitdate) AS l_commitdate, "  // handle timestamps by converting them to longs using SQL - "CAST(EXTRACT(EPOCH FROM l_commitdate) AS BIGINT) AS l_commitdate,
                                              "l_returnflag = 'N' AS dummy "
                                              "FROM lineitem "
                                              "ORDER BY l_orderkey "
                                              "LIMIT 10";
        return inputQuery;
    }

    virtual void SetUp() { }
};


class query_table_test : public ::testing::Test {


protected:
    void SetUp() override {};
    void TearDown() override{};
};

// tests how we handle each type
// also covers handling dummy flag as last column of input
TEST_F(query_table_test, read_table) {


    PsqlDataProvider dataProvider;
    AggregateDef aggDef;
    ShareDef def;



    string db_name =  FLAGS_party == emp::ALICE ? "tpch_alice" : "tpch_bob";

    cout << "Querying " << db_name << " with: " << QueryTableTestEnvironment::getInputQuery() << endl;




    auto inputTuples = dataProvider.GetQueryTable(db_name,
                                        QueryTableTestEnvironment::getInputQuery(), true);


}


TEST_F(query_table_test, encrypt_table) {


    PsqlDataProvider dataProvider;
    AggregateDef aggDef;
    ShareDef def;


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
                                        QueryTableTestEnvironment::getInputQuery(), true);

    ShareCount ca = {.party = EmpParty::ALICE};
    ca.num_tuples = inputTuples->GetNumTuples();

    ShareCount cb = {.party = EmpParty::BOB};
    cb.num_tuples = inputTuples->GetNumTuples();

    def.share_map[EmpParty::ALICE] = ca;
    def.share_map[EmpParty::BOB] = cb;

    auto encryptedInputTuples =
            ShareData(inputTuples->GetSchema(), party, inputTuples.get(), def);

    std::unique_ptr<QueryTable> decrypted = encryptedInputTuples->reveal(EmpParty::PUBLIC);
    std::unique_ptr<QueryTable> expected = dataProvider.GetQueryTable("dbname=tpch_unioned",
                                                            QueryTableTestEnvironment::getInputQuery(), true);


    std::cout << "Decrypted: " << decrypted << endl;
    std::cout << "Expected: "  << expected << endl;

    assert((decrypted.get()) == (expected.get()));
}
