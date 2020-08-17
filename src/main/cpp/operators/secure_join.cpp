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
     left->getSchema()->getFieldCount() + right->getSchema()->getFieldCount(),
     left->isEncrypted());
    output_table->setSchema(MergeSchema(left->getSchema(), right->getSchema()));


  int index = 0;
  for (int li = 0; li < left->getTupleCount(); li++) {
    for (int ri = 0; ri < right->getTupleCount(); ri++) {
      auto lt = left->getTuple(li);
      auto rt = right->getTuple(ri);
      expression::Expression ex(lt->GetField(def.left_index)->GetValue(),
                                rt->GetField(def.right_index)->GetValue(),
                                def.id);
      auto output = ex.execute();
      output_table->getTuple(index)->SetIsEncrypted(
              output_table->isEncrypted());
      output_table->getTuple(index)->InitDummy();
        output_table->getTuple(index)->SetDummyTag(&output);
      MergeTuples(output_table->getTuple(index), lt, left->getSchema(), rt,
                  right->getSchema());
      index++;
    }
  }
  return output_table;
}
