//
// Created by madhav on 1/29/20.
//

#include "private_share_utility.h"
#include "common/defs.h"
#include "query_table.h"

std::unique_ptr<emp::Batcher> GetBatcher(ShareCount &c,
                                         const QuerySchema *shared_schema,
                                         const QueryTable *input_table,
                                         EmpParty party) {

  auto batcher = std::make_unique<emp::Batcher>();
  for (int i = 0; i < c.num_tuples; i++) {
    for (int j = 0; j < shared_schema->getFieldCount(); j++) {
      switch (shared_schema->GetField(j)->GetType()) {
         case vaultdb::types::TypeId::INTEGER64: {
        if (party == c.party) {
              int64_t val =
                      input_table->getTuple(i)->GetField(j)->GetValue()->getInt64();
             batcher->add<emp::Integer>(64, val);
        } else {
          batcher->add<emp::Integer>(64, 0);
        }
        break;
      }
      default:
        throw;
      }
    }
  }
  return batcher;
}

void AddToTable(QueryTable *t, const QuerySchema *shared_schema,
                emp::Batcher *b, ShareCount &c, int insert_offset) {

  for (int i = 0; i < c.num_tuples; i++) {
    QueryTuple *tup = t->getTuple(i + insert_offset);
    tup->SetIsEncrypted(true);
    tup->InitDummy();
    for (int ordinal = 0; ordinal < shared_schema->getFieldCount(); ordinal++) {
      std::unique_ptr<vaultdb::QueryField> qf;
      switch (shared_schema->GetField(ordinal)->GetType()) {
      case vaultdb::types::TypeId::INTEGER64: {
        auto val = b->next<emp::Integer>();
        qf = std::make_unique<vaultdb::QueryField>(ordinal, val);
        break;
      }
      default:
        throw;
      }
      tup->PutField(ordinal, std::move(qf));
    }
  }
}

std::unique_ptr<QueryTable> ShareData(const QuerySchema *shared_schema,
                                      EmpParty party, QueryTable *input_table,
                                      ShareDef &def) {

  // TODO(madhavsuresh): copy the schema over to the new table. need to make
  // copy constructor for QuerySchema.

  std::map<EmpParty, std::unique_ptr<emp::Batcher>> batcher_map;

  // TODO(madhavsuresh): don't send in the table unless you are the owner of
  // the batcher
  batcher_map[EmpParty::ALICE] = GetBatcher(def.share_map[EmpParty::ALICE],
                                            shared_schema, input_table, party);

  batcher_map[EmpParty::BOB] = GetBatcher(def.share_map[EmpParty::BOB],
                                          shared_schema, input_table, party);

  batcher_map[EmpParty::ALICE]->make_semi_honest(
      static_cast<int>(EmpParty::ALICE));
  batcher_map[EmpParty::BOB]->make_semi_honest(static_cast<int>(EmpParty::BOB));

  std::unique_ptr<QueryTable> output_table = std::make_unique<QueryTable>(
       def.share_map[EmpParty::ALICE].num_tuples +
                                    def.share_map[EmpParty::BOB].num_tuples,
                                    shared_schema->getFieldCount(), true);

  AddToTable(output_table.get(), shared_schema,
             batcher_map[EmpParty::ALICE].get(), def.share_map[EmpParty::ALICE],
             0);

  AddToTable(output_table.get(), shared_schema,
             batcher_map[EmpParty::BOB].get(), def.share_map[EmpParty::BOB],
             def.share_map[EmpParty::ALICE].num_tuples);
  // TODO(madhavsuresh): this should really be with a copy constructor
  QuerySchema s(input_table->getSchema()->getFieldCount());
  for (int i = 0; i < input_table->getSchema()->getFieldCount(); i++) {
    auto f = input_table->getSchema()->GetField(i);
    switch (f->GetType()) {
    case vaultdb::types::TypeId::INTEGER64: {
      QueryFieldDesc new_field(*f, vaultdb::types::TypeId::ENCRYPTED_INTEGER64,
                               true /*is_private*/);
      s.PutField(i, new_field);
      break;
    }
    case vaultdb::types::TypeId::FLOAT32: {
      QueryFieldDesc new_field(*f, vaultdb::types::TypeId::ENCRYPTED_FLOAT32,
                               true /*is_private*/);
      s.PutField(i, new_field);
      break;
    }
    default: {
      throw;
    }
    }
  }
    output_table->setSchema(&s);
  return output_table;
}

