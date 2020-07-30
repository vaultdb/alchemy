//
// Created by madhav on 1/6/20.
//

#include "secure_join.h"
#include "emp-tool/emp-tool.h"
#include <querytable/expression/expression.h>
#include <querytable/query_table.h>

void MergeTuples(QueryTuple *output, const QueryTuple *tuple1,
                 const QuerySchema *schema1, const QueryTuple *tuple2,
                 const QuerySchema *schema2) {
  int counter = 0;
  for (int i = 0; i < schema1->getFieldCount(); i++) {
    output->PutField(counter, tuple1->GetField(i));
    counter++;
  }

  for (int i = 0; i < schema2->getFieldCount(); i++) {
    output->PutField(counter, tuple2->GetField(i));
    counter++;
  }
}

std::unique_ptr<QuerySchema> MergeSchema(const QuerySchema *schema1,
                                         const QuerySchema *schema2) {
  std::unique_ptr<QuerySchema> output = std::make_unique<QuerySchema>(
          schema1->getFieldCount() + schema2->getFieldCount());
  int counter = 0;
  for (int i = 0; i < schema1->getFieldCount(); i++) {
    output->PutField(counter, *schema1->GetField(i));
    counter++;
  }
  for (int i = 0; i < schema2->getFieldCount(); i++) {
    output->PutField(counter, *schema2->GetField(i));
    counter++;
  }
  return output;
}

std::unique_ptr<QueryTable> Join(QueryTable *left, QueryTable *right,
                                 const JoinDef &def) {
  std::unique_ptr<QueryTable> output_table = std::make_unique<QueryTable>(
     left->getTupleCount() * right->getTupleCount(),
     left->GetSchema()->getFieldCount() + right->GetSchema()->getFieldCount(),
     left->GetIsEncrypted());
  output_table->SetSchema(MergeSchema(left->GetSchema(), right->GetSchema()));


  int index = 0;
  for (int li = 0; li < left->getTupleCount(); li++) {
    for (int ri = 0; ri < right->getTupleCount(); ri++) {
      auto lt = left->GetTuple(li);
      auto rt = right->GetTuple(ri);
      expression::Expression ex(lt->GetField(def.left_index)->GetValue(),
                                rt->GetField(def.right_index)->GetValue(),
                                def.id);
      auto output = ex.execute();
      output_table->GetTuple(index)->SetIsEncrypted(
          output_table->GetIsEncrypted());
      output_table->GetTuple(index)->InitDummy();
        output_table->GetTuple(index)->SetDummyTag(&output);
      MergeTuples(output_table->GetTuple(index), lt, left->GetSchema(), rt,
                  right->GetSchema());
      index++;
    }
  }
  return output_table;
}
