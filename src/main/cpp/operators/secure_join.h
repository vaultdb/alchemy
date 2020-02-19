//
// Created by madhav on 1/6/20.
//

#ifndef TESTING_SECURE_JOIN_H
#define TESTING_SECURE_JOIN_H
#include <map>
#include <querytable/expression/expression_id.h>
#include <querytable/query_table.h>
#include <vector>

struct FieldMap {
  int input_field;
  int output_field;
};

enum MERGE_SIDE { LEFT, RIGHT };

struct MergeIndex {
  MergeIndex(int i, int i1, int i2)
      : in_table_index(i), in_field_index(i1), out_field_index(i2){};
  int in_table_index;
  int in_field_index;
  int out_field_index;
};

struct ProjectList {
  std::vector<MergeIndex> pl;
};
// Only support equi-joins for now
struct JoinDef {
  int left_index;
  int right_index;
  std::map<int, FieldMap> left_fields;
  std::map<int, FieldMap> right_fields;
  vaultdb::expression::ExpressionId id;
};

std::unique_ptr<QueryTable> Join(QueryTable *left, QueryTable *right,
                                 const JoinDef &def);

#endif // TESTING_SECURE_JOIN_H
