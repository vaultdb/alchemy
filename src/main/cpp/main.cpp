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
#include <operators/secure_aggregate.h>

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
  auto lineitem = pq.GetQueryTable("dbname=tpch_unioned",
                                   "SELECT l_quantity::integer FROM lineitem LIMIT 50");

  EmpParty my_party =
      FLAGS_party == emp::ALICE ? EmpParty::ALICE : EmpParty::BOB;
  ShareDef def;
  ShareCount ca = {.party = EmpParty::ALICE};
  ca.num_tuples = lineitem->GetNumTuples();
  ShareCount cb = {.party = EmpParty::BOB};
  cb.num_tuples = lineitem->GetNumTuples();


  def.share_map[EmpParty::ALICE] = ca;
  def.share_map[EmpParty::BOB] = cb;
  auto s_lineitem =
      ShareData(lineitem->GetSchema(), my_party, lineitem.get(), def);
  AggregateDef d;

  auto agg_result = Aggregate(s_lineitem.get(), d);
  int table_sum = agg_result->GetNumTuples();

  auto result = agg_result->GetTuple(0)->GetField(0)
      ->GetValue()
      ->GetEmpInt();
  std::cout << "\nNo. of rows in table: " << table_sum;
  std::cout << "\nSum (encrypted) : " << result;

  auto dec_result = result->reveal<int64_t>(emp::PUBLIC);
  std::cout << "\nSum (decrypted) : " << dec_result;
}

