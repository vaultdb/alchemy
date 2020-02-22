//#include <vaultdb.h>
#include <emp-sh2pc/emp-sh2pc.h>
#include <emp-tool/emp-tool.h>
#include <map>
#include <vaultdb.h>

using namespace std;

namespace EmpJniJdbcDemo {

class EmpJniJdbcDemoClass {

  string aliceHost = "127.0.0.1";
  void *output;
  map<string, QueryTable *> inputs;

  unique_ptr<QueryTable> unionOp(int party, emp::NetIO *io) { //, NetIO * io) {

    string firstKey = inputs.begin()->first;
    auto localData = inputs[firstKey];

    int aliceSize, bobSize;

    aliceSize = bobSize = localData->GetNumTuples();

    if (party == emp::ALICE) {
      io->send_data(&aliceSize, 4);
      io->flush();
      io->recv_data(&bobSize, 4);
      io->flush();
    } else if (party == emp::BOB) {
      io->recv_data(&aliceSize, 4);
      io->flush();
      io->send_data(&bobSize, 4);
      io->flush();
    }

    EmpParty my_party = party == emp::ALICE ? EmpParty::ALICE : EmpParty::BOB;
    ShareDef def;
    ShareCount ca = {.party = EmpParty::ALICE};
    ShareCount cb = {.party = EmpParty::BOB};
    ca.num_tuples = aliceSize;
    cb.num_tuples = bobSize;

    def.share_map[EmpParty::ALICE] = ca;
    def.share_map[EmpParty::BOB] = cb;

    auto s_table = ShareData(localData->GetSchema(), my_party, localData, def);
    return s_table;
  }

public:
  void run(int party, int port) {
    emp::NetIO *io =
        new emp::NetIO(party == emp::ALICE ? nullptr : "127.0.0.1", port);
    setup_semi_honest(io, party);
    std::unique_ptr<QueryTable> results = unionOp(party, io);
    inputs["shared"] = results.get();
    results.release();
  }

  // placeholder for maintaining a map of inputs from JDBC
  void addInput(const std::string &opName, const void *bitString, int length) {
    auto table = QueryTable::GetQueryFromProtoStream(bitString, length);
    inputs[opName] = table.get();
    table.release();
  }

  const void *getOutput() { return QueryTable::GetQueryTableXorString(inputs["shared"]).c_str(); }

}; // end class
} // namespace EmpJniJdbcDemo
