//
// Created by Salome Kariuki on 4/29/20.
//
#include "secure_sort.h"
#include <ctime>
#include <data/PQDataProvider.h>
#include <gflags/gflags.h>

#include "emp-sh2pc/emp-sh2pc.h"
#include "emp-tool/emp-tool.h"
#include "querytable/private_share_utility.h"
#include <chrono>
#include <emp-sh2pc/emp-sh2pc.h>

DEFINE_int32(party, 1, "party for EMP execution");
DEFINE_int32(port, 43439, "port for EMP execution");
DEFINE_string(hostname, "127.0.0.1", "hostname for execution");
DEFINE_bool(input, false, "input value");
DEFINE_int32(int_input, 10, "int_intput");

using namespace emp;
using namespace std;

int main(int argc, char **argv) {
  gflags::ParseCommandLineFlags(&argc, &argv, true /* remove_flags */);
  emp::NetIO *io = new emp::NetIO(FLAGS_party == emp::ALICE ? nullptr : FLAGS_hostname.c_str(), FLAGS_port);
  setup_semi_honest(io, FLAGS_party);

  PQDataProvider pq;
  auto lineitem =
      pq.GetQueryTable("dbname=tpch_alice", "SELECT c_custkey FROM customer LIMIT 10");

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

  vector<int> ordinals{0};
  SortDef sortdef;
  sortdef.order = SortOrder::ASCENDING;
  sortdef.ordinals = ordinals;
  cout << ((HalfGateGen<NetIO> *)CircuitExecution::circ_exec)->gid<<endl;
  Sort(s_lineitem.get(), sortdef);
  cout << ((HalfGateGen<NetIO> *)CircuitExecution::circ_exec)->gid<<endl;
}