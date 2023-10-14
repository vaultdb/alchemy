#ifndef _QUERY_SCHEMA_H
#define _QUERY_SCHEMA_H

#include "query_field_desc.h"
#include <map>
#include <memory>
#include <string>
#include <variant>
#include <vector>
#include <ostream>
#include <util/type_utilities.h>


namespace  vaultdb {
    class QuerySchema {

    public:
        std::map<int32_t, QueryFieldDesc> fields_;
        size_t tuple_size_; // in bits


        // offsets are used to jump to the specified field in memory
        // In OMPC the unit of offsets is packed wires
        // In all others, it is bits
        std::map<int32_t, size_t> offsets_;

        QuerySchema()  : tuple_size_(0) {         } // empty setup
        QuerySchema(const QuerySchema & s) :   fields_(s.fields_), tuple_size_(s.tuple_size_), offsets_(s.offsets_) { }

        // parse the CSV schema, e.g.,
        // (field1:INT,field2:float,...,fieldN:varchar(10))
        // entries can also be of the form (table_name.field_name:TYPE, ...)
        QuerySchema(const string & schema_spec);

        inline int getFieldCount() const {  return fields_.size() - 1;  } // does not include dummy tag

        // remember to run initializeFieldOffsets() after adding all fields
        inline void  putField(const QueryFieldDesc &fd) {     fields_[fd.getOrdinal()]  = fd; }

        inline void setSchema(const QuerySchema & s) {
            for (int i = 0; i < s.getFieldCount(); i++) {
                fields_[i] = s.getField(i);
            }

            initializeFieldOffsets();
        }

        // format inspired by Google BigTable's inline query schema
        // https://cloud.google.com/bigquery/docs/external-table-definition#use-inline-schema
        // example:
        // (id:INT64,name:STRING,age:INT64,weight:FLOAT64,is_active:BOOL,last_updated:TIMESTAMP)
        inline string prettyPrint() const {
            stringstream s;

            s << "(";
            if(getFieldCount() > 0) {
                s << fields_.at(0).prettyPrint();
                for (int i = 1; i < getFieldCount(); i++) {
                    s  << ", " << fields_.at(i).prettyPrint();
                }
            }
            s << ")";
            return s.str();
        }

        // returns size in bits
        inline size_t size() const { return tuple_size_; }

        inline bool   isSecure() const {
            return (fields_.at(0).getType() == TypeUtilities::toSecure(fields_.at(0).getType())); }

        QueryFieldDesc getField(const int &i) const {     return fields_.at(i);  }

        QueryFieldDesc getField(const std::string & fieldName) const;

        QuerySchema &operator=(const QuerySchema &other);

        inline size_t getFieldOffset(const int32_t idx) const { return offsets_.at(idx); }


        bool operator==(const QuerySchema &other) const;

        inline bool operator!=(const QuerySchema &other) const { return !(*this == other); }

        static QuerySchema toSecure(const QuerySchema &plainSchema);

        static QuerySchema toPlain(const QuerySchema &secureSchema);

        static QuerySchema concatenate(const QuerySchema & lhs, const QuerySchema & rhs, const bool &append_bool = false);

        static QuerySchema fromFile(const string & filename);
        void toFile(const string & filename) const;

        inline bool fieldInitialized(const int & ordinal) { return fields_.find(ordinal) != fields_.end();}
        void initializeFieldOffsets();

        inline void reset() {
            fields_.clear();
            offsets_.clear();
            tuple_size_ = 0;
        }


        ~QuerySchema() {
            reset();

        }

        // schema width in packed wires
        size_t packedWires() const {
            size_t running_cnt = 0;
           for(auto & field : fields_) {
               running_cnt += field.second.packedWires();
           }
           return running_cnt;
        }

        // schema width in bits
        size_t bitCnt() const {
            size_t running_cnt = 0;
            for(auto & field : fields_) {
                running_cnt += field.second.size();
            }
            return running_cnt;
        }

    };


    std::ostream &operator<<(std::ostream &os, const QuerySchema &schema);

}

#endif // _QUERY_SCHEMA_H
