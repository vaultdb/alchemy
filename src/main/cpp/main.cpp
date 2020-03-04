//
// Created by madhav on 12/20/19.
//
#include "data/PQDataProvider.h"
#include "emp-sh2pc/emp-sh2pc.h"
#include "emp-tool/emp-tool.h"
#include "operators/secure_join.h"
#include "querytable/private_share_utility.h"
#include <chrono>
#include <gflags/gflags.h>
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


  auto lineitem = pq.GetQueryTable(
      "dbname= tpch_unioned",
      "SELECT l_quantity::integer, l_partkey::integer, l_suppkey::integer, "
      "l_orderkey::integer FROM lineitem ORDER BY l_orderkey, l_linenumber LIMIT 50");

  EmpParty my_party =
      FLAGS_party == emp::ALICE ? EmpParty::ALICE : EmpParty::BOB;

  //string db_name = "tpch_" +
      //(my_party == EmpParty::ALICE) ? "alice" :"bob";
  //std::cout<<db_name;
//  auto lineitem = pq.GetQueryTable(
//      "dbname=" + db_name,
//      "SELECT l_quantity::integer, l_partkey::integer, l_suppkey::integer, "
//      "l_orderkey::integer FROM lineitem ORDER BY l_orderkey, l_linenumber LIMIT 50");
  ShareDef def;
  ShareCount ca = {.party = EmpParty::ALICE};
  ca.num_tuples = lineitem->GetNumTuples();
  ShareCount cb = {.party = EmpParty::BOB};
  cb.num_tuples = lineitem->GetNumTuples();

  def.share_map[EmpParty::ALICE] = ca;
  def.share_map[EmpParty::BOB] = cb;
  // lineitem.get()->GetTuple(0)->GetField(ord)
  auto s_lineitem =
      ShareData(lineitem->GetSchema(), my_party, lineitem.get(), def);

  AggregateDef d;
  //d.defs.at(0).ordinal;
  // TODO: hardcoding defs just to get it to work, automate later
  ScalarAggregateDef def1 = {0, vaultdb::AggregateId::COUNT};
  ScalarAggregateDef def2 = {1, vaultdb::AggregateId::AVG};
  ScalarAggregateDef def3 = {2, vaultdb::AggregateId::SUM};
  ScalarAggregateDef def4 = {3, vaultdb::AggregateId::SUM};

  d.defs.push_back(def1); //COUNT
  d.defs.push_back(def2); //COUNT
  d.defs.push_back(def3); //SUM TODO(shawshank-cs: IMPLEMENT AVERAGE
  d.defs.push_back(def4); //SUM

//  for (int i = 0; i < s_lineitem.get()->GetSchema()->GetNumFields(); i++) {
//    d.index.push_back(
//        s_lineitem.get()->GetSchema()->GetField(i)->GetColumnNumber());
//  }
  // s_lineitem.get()->GetSchema()->GetNumFields();
  auto agg_result = Aggregate(s_lineitem.get(), d);
  int table_avg = agg_result->GetNumTuples();

  for (ScalarAggregateDef ord : d.defs) {
    // d.index.front();
    auto result = agg_result->GetTuple(0)
                      ->GetField(ord.ordinal)
                      ->GetValue()
                      ->GetEmpInt();

    string type = "";
    if (ord.id == AggregateId ::COUNT) { type = "Count"; }
    if (ord.id == AggregateId ::SUM) { type = "Sum"; }
    if (ord.id == AggregateId ::AVG) { type = "Average"; }

    std::cout << "\n"<<type<<" (encrypted) : "<< result;
    auto dec_result = result->reveal<int64_t>(emp::PUBLIC);
    std::cout << "\n"<<type<<" (decrypted) : " << dec_result;
  }
}
