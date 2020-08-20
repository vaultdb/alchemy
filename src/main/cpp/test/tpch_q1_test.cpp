//
// Created by Jennie Rogers on 7/16/20.
//

#include "secure_aggregate.h"
#include "secure_sort.h"
#include "support/tpch_queries.h"
#include "util/emp_manager.h"


#include <data/PsqlDataProvider.h>
#include <gflags/gflags.h>
#include <gtest/gtest.h>


using namespace emp;
using namespace std;


DEFINE_int32(party, 1, "party for EMP execution");
DEFINE_int32(port, 43439, "port for EMP execution");
DEFINE_string(alice_host, "127.0.0.1", "alice hostname for execution");


class tpch_q1_test : public ::testing::Test {


protected:
    void SetUp() override {};
    void TearDown() override{};
};

/* TEST_F(tpch_q1_test, TpcHQ1FullOblivous) {


    PsqlDataProvider pq;
    AggregateDef aggDef;

    EmpManager *empManager = EmpManager::getInstance();
    empManager->configureEmpManager(FLAGS_alice_host.c_str(), FLAGS_port, (EmpParty) FLAGS_party);

    vector<int> sortOrdinals{0, 1};
    SortDef sortDef;
    sortDef.order = SortOrder::ASCENDING; // TODO: each sort ordinal needs its own collation (ASC||DESC)
    sortDef.ordinals = sortOrdinals;


    string baseQuery = tpch_queries[1];
    string db_name =  FLAGS_party == emp::ALICE ? "tpch_alice" : "tpch_bob";

    // Ordinals:
    // l_returnflag, #0
    // l_linestatus, #1
    // l_quantity, #2
    // l_extendedprice, #3
    // l_discount, #4
    // l_extendedprice * (1 - l_discount) AS disc_price, #5
    // l_extendedprice * (1 - l_discount) * (1 + l_tax) AS charge #6

    string inputQuery = "SELECT l_returnflag, l_linestatus, l_quantity, l_extendedprice,  l_discount, l_extendedprice * (1 - l_discount) AS disc_price, l_extendedprice * (1 - l_discount) * (1 + l_tax) AS charge, \n"
            " l_shipdate > date '1998-08-03' AS dummy\n"  // produces true when it is a dummy, reverses the logic of the sort predicate
            " FROM lineitem\n"
            " ORDER BY l_returnflag, l_linestatus"; // TODO: use merge sort after secret sharing

    auto inputTable = pq.getQueryTable(db_name,
                                 inputQuery, "lineitem", true);
    std::cout<<"Party = "<<FLAGS_party << " received " << inputTable->getTupleCount() << " tuples\n";


    std::unique_ptr<QueryTable> encryptedTable = empManager->secretShareTable(inputTable.get());
    empManager->flush();


    // sort the input tuples
    // TODO: use merge sort instead (latter half of bitonic sort network)
    Sort(encryptedTable.get(), sortDef);


    aggDef.scalarAggregates.emplace_back(ScalarAggregateDef(2, vaultdb::AggregateId::SUM, "sum_qty"));
    aggDef.scalarAggregates.emplace_back(ScalarAggregateDef(3, vaultdb::AggregateId::SUM, "sum_base_price"));
    aggDef.scalarAggregates.emplace_back(ScalarAggregateDef(5, vaultdb::AggregateId::SUM, "sum_disc_price"));
    aggDef.scalarAggregates.emplace_back(ScalarAggregateDef(6, vaultdb::AggregateId::SUM, "sum_charge"));
    aggDef.scalarAggregates.emplace_back(ScalarAggregateDef(2, vaultdb::AggregateId::AVG, "avg_qty"));
    aggDef.scalarAggregates.emplace_back(ScalarAggregateDef(3, vaultdb::AggregateId::AVG, "avg_price"));
    aggDef.scalarAggregates.emplace_back(ScalarAggregateDef(4, vaultdb::AggregateId::AVG, "avg_disc"));
    aggDef.scalarAggregates.emplace_back(ScalarAggregateDef(-1, vaultdb::AggregateId::COUNT, "count_order"));

    aggDef.groupByOrdinals.emplace_back(0);
    aggDef.groupByOrdinals.emplace_back(1);



   auto aggregated = Aggregate(encryptedTable.get(), aggDef);



   // TODO: shashank: verify the reveal method in QueryTable
    std::unique_ptr<QueryTable> decrypted = aggregated->reveal(EmpParty::PUBLIC);

    std::unique_ptr<QueryTable> expected = pq.getQueryTable("dbname=tpch_unioned",
                                            baseQuery, "lineitem", false);


    //std::cout << "Decrypted: " << decrypted << endl;
    //std::cout << "Expected: "  << expected << endl;

   assert(decrypted->toString() == expected->toString());
}
 */

/****
 * Expected output:
 *
 l_returnflag | l_linestatus | sum_qty | sum_base_price | sum_disc_price  |     sum_charge     |       avg_qty       |     avg_price      |        avg_disc        | count_order
--------------+--------------+---------+----------------+-----------------+--------------------+---------------------+--------------------+------------------------+-------------
 A            | F            | 3774200 |  5320753880.69 | 5054096266.6828 |  5256751331.449234 | 25.5375871168549970 | 36002.123829014142 | 0.05014459706340077136 |      147790
 N            | F            |   95257 |   133737795.84 |  127132372.6512 |   132286291.229445 | 25.3006640106241700 | 35521.326916334661 | 0.04939442231075697211 |        3765
 N            | O            | 7285768 | 10267376849.71 | 9753475393.6898 | 10143398614.479935 | 25.5459919635907182 | 36000.367633151240 | 0.05010827413552499632 |      285202
 R            | F            | 3785523 |  5337950526.47 | 5071818532.9420 |  5274405503.049367 | 25.5259438574251017 | 35994.029214030924 | 0.04998927856184381764 |      148301
(4 rows)


 */

TEST_F(tpch_q1_test, TpcHQ1FullObliviousTruncated) {


  PsqlDataProvider pq;
  AggregateDef aggDef;

  EmpManager *empManager = EmpManager::getInstance();
  empManager->configureEmpManager(FLAGS_alice_host.c_str(), FLAGS_port, (EmpParty) FLAGS_party);

  vector<int> sortOrdinals{0, 1};
  SortDef sortDef;
  sortDef.order = SortOrder::ASCENDING; // TODO: each sort ordinal needs its own collation (ASC||DESC)
  sortDef.ordinals = sortOrdinals;


  string baseQuery = tpch_queries[1];
  string db_name =  FLAGS_party == emp::ALICE ? "tpch_alice" : "tpch_bob";

  // Ordinals:
  // l_returnflag, #0
  // l_linestatus, #1
  // l_quantity, #2
  // l_extendedprice, #3
  // l_discount, #4
  // l_extendedprice * (1 - l_discount) AS disc_price, #5
  // l_extendedprice * (1 - l_discount) * (1 + l_tax) AS charge #6

  string inputQuery = "SELECT l_returnflag, l_linestatus, l_quantity, l_extendedprice,  l_discount, l_extendedprice * (1 - l_discount) AS disc_price, l_extendedprice * (1 - l_discount) * (1 + l_tax) AS charge, \n"
                      " l_shipdate > date '1998-08-03' AS dummy\n"  // produces true when it is a dummy, reverses the logic of the sort predicate
                      " FROM lineitem\n"
                      " ORDER BY l_returnflag, l_linestatus LIMIT 10"; // TODO: use merge sort after secret sharing

  auto inputTable = pq.getQueryTable(db_name,
                                     inputQuery, true);
  std::cout<<"Party = "<<FLAGS_party << " received " << inputTable->getTupleCount() << " tuples\n";


  std::shared_ptr<QueryTable> encryptedTable = empManager->secretShareTable(inputTable.get());
  empManager->flush();


  // sort the input tuples
  // TODO: use merge sort instead (latter half of bitonic sort network)
  Sort(encryptedTable.get(), sortDef);


  aggDef.scalarAggregates.emplace_back(ScalarAggregateDef(2, vaultdb::AggregateId::SUM, "sum_qty"));
  aggDef.scalarAggregates.emplace_back(ScalarAggregateDef(3, vaultdb::AggregateId::SUM, "sum_base_price"));
  aggDef.scalarAggregates.emplace_back(ScalarAggregateDef(5, vaultdb::AggregateId::SUM, "sum_disc_price"));
  aggDef.scalarAggregates.emplace_back(ScalarAggregateDef(6, vaultdb::AggregateId::SUM, "sum_charge"));
  aggDef.scalarAggregates.emplace_back(ScalarAggregateDef(2, vaultdb::AggregateId::AVG, "avg_qty"));
  aggDef.scalarAggregates.emplace_back(ScalarAggregateDef(3, vaultdb::AggregateId::AVG, "avg_price"));
  aggDef.scalarAggregates.emplace_back(ScalarAggregateDef(4, vaultdb::AggregateId::AVG, "avg_disc"));
  aggDef.scalarAggregates.emplace_back(ScalarAggregateDef(-1, vaultdb::AggregateId::COUNT, "count_order"));

  aggDef.groupByOrdinals.emplace_back(0);
  aggDef.groupByOrdinals.emplace_back(1);



  auto aggregated = Aggregate(encryptedTable.get(), aggDef);



  // TODO: shashank: verify the reveal method in QueryTable
  std::unique_ptr<QueryTable> decrypted = aggregated->reveal(EmpParty::PUBLIC);

  std::unique_ptr<QueryTable> expected = pq.getQueryTable("dbname=tpch_unioned",
                                                          baseQuery, false);


  //std::cout << "Decrypted: " << decrypted << endl;
  //std::cout << "Expected: "  << expected << endl;

  assert(decrypted->toString() == expected->toString());
}

// initialize gflags
int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    gflags::ParseCommandLineFlags(&argc, &argv, false);

    return RUN_ALL_TESTS();
}

