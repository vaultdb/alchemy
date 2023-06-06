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

    protected:

        std::map<int32_t, QueryFieldDesc> fields_;
        size_t tuple_size_; // in bits
    public:
        std::map<int32_t, size_t> offsets_;

        QuerySchema() {} // empty setup
        QuerySchema(const QuerySchema & s) {

            for (size_t i = 0; i < s.getFieldCount(); i++) {
                fields_[i] = s.getField(i);
            }
            initializeFieldOffsets();

        }

        inline int getFieldCount() const {  return fields_.size() - 1;  } // does not include dummy tag


        inline void  putField(const QueryFieldDesc &fd) {     fields_[fd.getOrdinal()]  = fd; }

        // returns size in bits
        size_t size() const;

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


        inline bool fieldInitialized(const int & ordinal) { return fields_.find(ordinal) != fields_.end();}
        void initializeFieldOffsets();

        inline void reset() {
            fields_.clear();
            offsets_.clear();
            tuple_size_ = 0;
        }

    };

    std::ostream &operator<<(std::ostream &os, const QuerySchema &schema);

}

#endif // _QUERY_SCHEMA_H
