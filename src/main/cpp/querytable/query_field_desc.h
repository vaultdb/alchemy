//
// Created by madhav on 1/13/20.
//

#ifndef _QUERY_FIELD_DESC_H
#define _QUERY_FIELD_DESC_H

#include <map>
#include <memory>
#include <querytable/types/type_id.h>
#include <string>
#include <utility>
#include <variant>
#include <vector>
#include <ostream>

class QueryFieldDesc {

private:
    vaultdb::types::TypeId type_;
   int column_number_;
   bool is_private_;
   std::string name_;
   std::string table_name;
  size_t string_length; // for varchars

public:
  [[nodiscard]] int GetColumnNumber() const;

  [[nodiscard]] bool GetIsPrivate() const;

  [[nodiscard]] const std::string &GetName() const;

  [[nodiscard]] vaultdb::types::TypeId GetType() const;

  [[nodiscard]] const std::string &GetTableName() const;

  [[nodiscard]] size_t size() const;

  QueryFieldDesc(QueryFieldDesc &f)
      : type_(f.type_), column_number_(f.column_number_),
        is_private_(f.is_private_), name_(f.name_), table_name(f.table_name), string_length(f.string_length)
        {};

  QueryFieldDesc(QueryFieldDesc &f, vaultdb::types::TypeId type,
                 bool is_private)
      : type_(type), column_number_(f.column_number_), is_private_(is_private),
        name_(f.name_), table_name(f.table_name), string_length(0)
        {};

  QueryFieldDesc(QueryFieldDesc &f, int col_num)
      : type_(f.type_), column_number_(col_num), is_private_(f.is_private_),
        name_(f.name_), table_name(f.table_name), string_length(0)
        {};

  QueryFieldDesc(int col_num, bool is_priv, const std::string &n,
                 const std::string &tab, vaultdb::types::TypeId ty)
      : type_(ty), column_number_(col_num), is_private_(is_priv), name_(n),
        table_name(tab), string_length(0)
        {};

    void setStringLength(size_t i);
    void initialize(QueryFieldDesc src);

    friend std::ostream &operator<<(std::ostream &os, const QueryFieldDesc &desc);

    QueryFieldDesc();
};

#endif // _QUERY_FIELD_DESC_H
