#ifndef GENERATE_SYNTHETIC_DATA_H
#define GENERATE_SYNTHETIC_DATA_H

#include <string>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <map>
#include <unistd.h>
#include <sys/stat.h>
#include <iostream>
#include <vector>
#include <boost/property_tree/ptree.hpp>
#include <query_table/field/field_factory.h>

#include "query_table/query_field_desc.h"
#include "query_table/field/field.h"
#include "query_table/query_schema.h"
#include "query_table/plain_tuple.h"

using namespace std;
using namespace vaultdb;

enum class DomainType { RANGE, LIST, WEIGHT };
// plaintext definition only
class ColumnDefinition {
   public:
    QueryFieldDesc def_;
    DomainType type_;
    long min_; // range only works for long and int
    long max_;
    vector<PlainField> domain_;
    float weight_; // probability of a bool column being true

   int domainSize() const {
     if(type_ == DomainType::RANGE) {
       return (max_ - min_) +  1;
      }
      else {
        return domain_.size();
      }
    }

    string getSchema() const {
      return def_.prettyPrint();
      }



  string toString() const {
     stringstream ss;
     ss << def_.prettyPrint() << ": ";
      if(type_ == DomainType::RANGE) {
        ss << "[" << min_ << "..." << max_ << "]";
      }
      else {
        ss << "[" << domain_[0];
        for(int i = 1; i < domain_.size(); ++i) {
          ss << ", " << domain_[i];
        }
        ss << "]";
      }
     return ss.str();
   }

      PlainField generateField() {
        if(type_ == DomainType::RANGE) {
          long selected = rand() % domainSize();
          auto val = min_ + selected;
          if(def_.getType() == FieldType::INT) {
            return PlainField(def_.getType(), (int) val);
          }

          if(def_.getType() == FieldType::LONG) {
            return PlainField(def_.getType(), (int64_t) val);
          }

          throw runtime_error("generateField(): unsupported field type");
        }
       else if (type_ == DomainType::WEIGHT) {
          // select a random value in domain
         float selected = rand() / (float) RAND_MAX;
          if (selected < weight_) {
            return PlainField(def_.getType(), true);
          }
          else {
            return PlainField(def_.getType(), false);
          }
        }
        else {
          long selected = rand() % domainSize();
          // select a random value in domain
          return domain_[selected];
        }


      }



};



class TableDefinition {
  public:
    string name_;
    QuerySchema schema_;
    map<int, ColumnDefinition> col_defs_; // map ordinals to column definitions
    int key_idx_ = -1;
    size_t cardinality_;
    float overlap_ = -1.0; // 0.0 - 1.0, % of rows that have key in common among the parties


  string toString() const {
    stringstream ss;
    ss << name_ << schema_ << " tuple cnt: " << cardinality_ << " key idx: " << key_idx_ << " overlap: " << overlap_
      << ", domains: ";
    for (auto & col_def : col_defs_) {
      ss << col_def.second.toString() << ", ";
    }
    return ss.str();
  }

  string generateRow(const int & key) {

    stringstream ss;
    for (int i = 0; i < this->schema_.getFieldCount(); ++i) {
      PlainField f;
        if (i == key_idx_) {
          f = PlainField(FieldType::INT, key);
        }
        else if (col_defs_.find(i) != col_defs_.end()) {
            f = col_defs_.at(i).generateField();
          }
          else {
              // default value
            auto ft = schema_.getField(i).getType();
            f = FieldFactory<bool>::getZero(ft);
          }
          if (i > 0) {
            ss << ",";
          }
           ss << f.toString();
      }

     return ss.str();
  }

};


class GenerateSyntheticData {
public:
   GenerateSyntheticData(const string & json_filename) {
     parseJson(json_filename);
     if(party_cnt_ > 1) {
       for (int i = 0; i < party_cnt_; ++i) {
         string dst_path = dst_path_ + "/" + std::to_string(i);
         Utilities::runCommand("mkdir -p " + dst_path);
       }
      }
   }

   void generate() {
     if(party_cnt_ > 1)
        generateOverlapping();
      else
        generateOneHost();
   }

  string dst_path_;


   private:
    int party_cnt_;

     void parseJson(const string & json);
     void parseTable(boost::property_tree::ptree & table);
     void writeSchemaFile(const string & dst_file, QuerySchema & schema);

     template<typename  T>
     T parseField(const boost::property_tree::ptree & pt, const string & key) {
       if(pt.count(key) > 0) {
         return pt.get_child(key).template get_value<T>();
       }
       else {
            throw runtime_error("Missing field: " + key);
         }
     }

      vector<TableDefinition> table_defs_;

       // random key from some other party's keyspace
       int generateOverlappingRowId(const int & party, const int & rows_per_party) {
         if (party_cnt_ == 1) return rand() % rows_per_party;
         int other = rand() % party_cnt_;
         while (other == party) { other = rand() % party_cnt_; }
         int idx = rand() % rows_per_party;
         return idx + (other * rows_per_party);
       }

       void generateOverlapping();
       void generateOneHost();

};



#endif // GENERATE_SYNTHETIC_DATA_H