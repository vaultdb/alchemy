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
   int ordinal_;
   bool is_private_;
   std::string name_;
   std::string table_name;
    vaultdb::types::TypeId type_;
  size_t string_length; // for varchars

public:
    QueryFieldDesc();

    [[nodiscard]] int getOrdinal() const;

  [[nodiscard]] bool getIsPrivate() const;

  [[nodiscard]] const std::string &getName() const;

  [[nodiscard]] vaultdb::types::TypeId getType() const;

  [[nodiscard]] const std::string &getTableName() const;

  [[nodiscard]] size_t size() const;

  QueryFieldDesc(const QueryFieldDesc &f)
      : ordinal_(f.ordinal_),
        is_private_(f.is_private_), name_(f.name_), table_name(f.table_name), string_length(f.string_length), type_(f.type_)
        {};

  QueryFieldDesc(const QueryFieldDesc &f, vaultdb::types::TypeId type,
                 bool is_private)
      : type_(type), ordinal_(f.ordinal_), is_private_(is_private),
        name_(f.name_), table_name(f.table_name), string_length(0)
        {};

  QueryFieldDesc(const QueryFieldDesc &f, int col_num)
      :  ordinal_(col_num), is_private_(f.is_private_),
        name_(f.name_), table_name(f.table_name), string_length(0), type_(f.type_)
        {};

  QueryFieldDesc(int anOrdinal, bool is_priv, const std::string &n,
                 const std::string &tab, const vaultdb::types::TypeId & aType)
      : type_(aType), ordinal_(anOrdinal), is_private_(is_priv), name_(n),
        table_name(tab), string_length(0)
        {
            // since we convert DATEs to int32_t in both operator land and in our verification pipeline,
            // i.e., we compare the output of our queries to SELECT EXTRACT(EPOCH FROM date_)
            // fields of type date have no source table
            if(type_ == vaultdb::types::TypeId::DATE) {
                table_name = "";
                type_ = vaultdb::types::TypeId::INTEGER64; // we actually store it as an INT32, this is the result of EXTRACT(EPOCH..)
            }
        };

    void setStringLength(size_t i);
    size_t getStringLength() const { return string_length; }


    friend std::ostream &operator<<(std::ostream &os, const QueryFieldDesc &desc);
    QueryFieldDesc& operator=(const QueryFieldDesc& other);

     bool operator==(const QueryFieldDesc& other);

     inline bool operator!=(const QueryFieldDesc &other) {  return !(*this == other); }


};

#endif // _QUERY_FIELD_DESC_H
