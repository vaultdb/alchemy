#ifndef _QUERY_FIELD_DESC_H
#define _QUERY_FIELD_DESC_H

#include <map>
#include <memory>
#include <string>
#include <utility>
#include <variant>
#include <vector>
#include <ostream>
#include <query_table/field/field_type.h>
#include <common/defs.h>

namespace  vaultdb {
    class QueryFieldDesc {

    protected:
        std::string field_name_;
        std::string table_name_;
        size_t string_length_; // for varchars
        FieldType type_;
        int ordinal_;
        int field_size_ = 0; // size in bits
        int64_t field_min_ = 0; // if bits are packed and min > 0
        size_t bit_packed_size_ = 0;
        
    public:
        QueryFieldDesc();

        int getOrdinal() const;


        const std::string &getName() const;

        FieldType getType() const;

       const std::string &getTableName() const;

         size_t size() const;

        QueryFieldDesc(const QueryFieldDesc &f) = default;

        QueryFieldDesc(const QueryFieldDesc &f, const FieldType & type);

        QueryFieldDesc(const QueryFieldDesc &f, const int & col_num);

        QueryFieldDesc(const int & anOrdinal, const std::string &n, const std::string &tab, const FieldType &aType, const size_t & stringLength = 0);

        void setStringLength(size_t i);
        int64_t getFieldMin() const { return field_min_; }

        size_t getStringLength() const { return string_length_; }

        void inline setOrdinal(const int &  ordinal) { ordinal_ = ordinal; }
        void inline setName(const string & table_name, const string & field_name) {
            table_name_ = table_name;
            field_name_ = field_name;
        }

        QueryFieldDesc &operator=(const QueryFieldDesc &other);

        bool operator==(const QueryFieldDesc &other);

        inline bool operator!=(const QueryFieldDesc &other) { return !(*this == other); }

    private:
        void initializeFieldSize();

    };

         std::ostream& operator<<(std::ostream &strm, const QueryFieldDesc &desc);

}

#endif // _QUERY_FIELD_DESC_H
