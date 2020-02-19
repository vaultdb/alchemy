//
// Created by madhav on 1/13/20.
//

#ifndef TESTING_QUERY_FIELD_DESC_H
#define TESTING_QUERY_FIELD_DESC_H

#include <map>
#include <memory>
#include <querytable/types/type_id.h>
#include <string>
#include <utility>
#include <variant>
#include <vector>

class QueryFieldDesc {

private:
  const vaultdb::types::TypeId type_;
  const int column_number_;
  const bool is_private_;
  const std::string name_;
  // origin table name
  const std::string table_name;

public:
  [[nodiscard]] int GetColumnNumber() const;

  [[nodiscard]] bool GetIsPrivate() const;

  [[nodiscard]] const std::string &GetName() const;

  [[nodiscard]] vaultdb::types::TypeId GetType() const;

  [[nodiscard]] const std::string &GetTableName() const;

  [[nodiscard]] size_t GetFieldSize() const;

  QueryFieldDesc(QueryFieldDesc &f)
      : type_(f.type_), column_number_(f.column_number_),
        is_private_(f.is_private_), name_(f.name_), table_name(f.table_name){};

  QueryFieldDesc(QueryFieldDesc &f, int col_num)
      : type_(f.type_), column_number_(col_num), is_private_(f.is_private_),
        name_(f.name_), table_name(f.table_name){};
  QueryFieldDesc(int col_num, bool is_priv, const std::string &n,
                 const std::string &tab, vaultdb::types::TypeId ty)
      : type_(ty), column_number_(col_num), is_private_(is_priv), name_(n),
        table_name(tab){};
};

#endif // TESTING_QUERY_FIELD_DESC_H
