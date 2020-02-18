//
// Created by madhav on 12/20/19.
//
#include "data/PQDataProvider.h"
#include "emp-sh2pc/emp-sh2pc.h"
#include "emp-tool/emp-tool.h"
#include "gflags/gflags.h"
#include "operators/secure_join.h"
#include "querytable/private_share_utility.h"
#include <chrono>

DEFINE_int32(party, 1, "party for EMP execution");
DEFINE_int32(port, 43439, "port for EMP execution");
DEFINE_string(hostname, "127.0.0.1", "hostname for execution");
DEFINE_bool(input, false, "input value");
DEFINE_int32(int_input, 10, "int_intput");

int main(int argc, char **argv) {
  gflags::ParseCommandLineFlags(&argc, &argv, true /* remove_flags */);
  emp::NetIO *io = new emp::NetIO(
      FLAGS_party == emp::ALICE ? nullptr : FLAGS_hostname.c_str(), FLAGS_port);
  setup_semi_honest(io, FLAGS_party);

  PQDataProvider pq = PQDataProvider();
  auto lineitem =
      pq.GetQueryTable("dbname=tpch_unioned",
                       "SELECT l_orderkey, l_suppkey FROM lineitem LIMIT 50");
  auto orders = pq.GetQueryTable("dbname=tpch_unioned",
                                 "SELECT o_orderkey FROM orders LIMIT 50");

  auto supplier = pq.GetQueryTable("dbname=tpch_unioned",
                                   "SELECT s_suppkey FROM supplier LIMIT 50");

  EmpParty my_party =
      FLAGS_party == emp::ALICE ? EmpParty::ALICE : EmpParty::BOB;
  ShareDef def;
  ShareCount ca = {.party = EmpParty::ALICE, .num_tuples = 50};
  ShareCount cb = {.party = EmpParty::BOB, .num_tuples = 50};

  def.share_map[EmpParty::ALICE] = ca;
  def.share_map[EmpParty::BOB] = cb;
  auto s_lineitem =
      ShareData(lineitem->GetSchema(), my_party, lineitem.get(), def);
  auto s_orders = ShareData(orders->GetSchema(), my_party, orders.get(), def);
  auto s_supplier =
      ShareData(supplier->GetSchema(), my_party, supplier.get(), def);

  JoinDef x = {.left_index = 0, .right_index = 0};
  x.id = vaultdb::expression::ExpressionId::EQUAL;
  auto start = std::chrono::system_clock::now();
  auto s_OL = Join(orders.get(), lineitem.get(), x);
  auto end = std::chrono::system_clock::now();
  std::chrono::duration<double> elapsed_seconds = end - start;
  std::time_t end_time = std::chrono::system_clock::to_time_t(end);

  std::cout << "finished computation at " << std::ctime(&end_time)
            << "elapsed time: " << elapsed_seconds.count() << "s\n";
  JoinDef y = {.left_index = 0, .right_index = 0};
  auto s_OLS = Join(s_OL.get(), supplier.get(), x);
  std::cout << "NUM TUPLES :" << s_OL->GetNumTuples() << ", "
            << s_OLS->GetNumTuples();
}
