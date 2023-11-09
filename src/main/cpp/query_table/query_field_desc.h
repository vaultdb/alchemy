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
        size_t string_length_;
        FieldType type_;
        int ordinal_;
        int field_size_ = 0; // size in bits
        int64_t field_min_ = 0; // if bits are packed and min != 0
        int packed_wires_ = 1;


    public:

        inline QueryFieldDesc() : type_(FieldType::INVALID) { }
        inline int getOrdinal() const {   return ordinal_; }


        inline const std::string &getName() const { return field_name_; }

        inline FieldType getType() const { return type_; }

       inline const std::string &getTableName() const { return table_name_; }

        inline  size_t size() const {
            assert(field_size_ > 0);  // if size == 0 then it is an invalid field
            return field_size_;
        }

        QueryFieldDesc(const QueryFieldDesc &f) = default;

        QueryFieldDesc(const QueryFieldDesc &f, const FieldType & type);

        QueryFieldDesc(const QueryFieldDesc &f, const int & col_num);

        QueryFieldDesc(const int & ordinal, const std::string &col_name, const std::string &table_name, const FieldType &type, const size_t & str_len = 0);
        QueryFieldDesc(const int & ordinal, const string & field_spec);

        inline void setStringLength(size_t len) {   string_length_ = len;
            initializeFieldSize();
        }
        inline int64_t getFieldMin() const { return field_min_; }

        size_t getStringLength() const { return string_length_; }

        void inline setOrdinal(const int &  ordinal) { ordinal_ = ordinal; }
        void inline setName(const string & table_name, const string & field_name) {
            table_name_ = table_name;
            field_name_ = field_name;
        }

        void inline setSize(const int & size) { field_size_ = size; }
        QueryFieldDesc &operator=(const QueryFieldDesc &other);

        bool operator==(const QueryFieldDesc &other) const;
        inline bool operator!=(const QueryFieldDesc &other) const { return !(*this == other); }

        bool bitPacked() const;
        // fields not only the same type, but have same packed size, min value and other metadata
        // If two fields passed into a packed expression evaluation, such as "lhs == rhs" will they produce correct results?
        inline static bool storageCompatible(const QueryFieldDesc & lhs, const QueryFieldDesc & rhs) {
            if((lhs != rhs) || (lhs.bitPacked() != rhs.bitPacked())) return false;
            if(lhs.bitPacked()) {
                if(lhs.size() != rhs.size()) return false;
                if(lhs.field_min_ != rhs.field_min_) return false;
            }
            return true;
        }
        void initializeFieldSizeWithCardinality(int cardinality);
        string prettyPrint() const;
        int packedWires() const { return packed_wires_; }

    private:
        void initializeFieldSize();

    };

         std::ostream& operator<<(std::ostream &strm, const QueryFieldDesc &desc);

}

#endif // _QUERY_FIELD_DESC_H
