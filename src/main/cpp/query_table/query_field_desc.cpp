#include "query_field_desc.h"
#include "util/utilities.h"
#include "util/type_utilities.h"

#include "util/system_configuration.h"

using namespace vaultdb;



QueryFieldDesc::QueryFieldDesc(const QueryFieldDesc &f, const FieldType &type)
        : field_name_(f.field_name_), table_name_(f.table_name_),
          string_length_(f.getStringLength()),
          type_(type), ordinal_(f.ordinal_), bit_packed_size_(f.bit_packed_size_), field_min_(f.field_min_), secure_field_min_(f.secure_field_min_) { // carry over bit packed size
    initializeFieldSize();
}


QueryFieldDesc::QueryFieldDesc(const QueryFieldDesc &f, const int &  col_num)
        : field_name_(f.field_name_), table_name_(f.table_name_), string_length_(f.string_length_), type_(f.type_), ordinal_(col_num), field_size_(f.field_size_), bit_packed_size_(f.field_size_), field_min_(f.field_min_), secure_field_min_(f.secure_field_min_) {
}

QueryFieldDesc::QueryFieldDesc(const int & anOrdinal, const string &n, const string &tab, const FieldType &aType,
                               const size_t &stringLength)
        : field_name_(n),
          table_name_(tab), string_length_(stringLength), type_(aType), ordinal_(anOrdinal) {
    // since we convert DATEs to int32_t in both operator land and in our verification pipeline,
    // i.e., we compare the output of our queries to SELECT EXTRACT(EPOCH FROM date_)
    // fields of type date have no source table
    if (type_ == FieldType::DATE) {
        table_name_ = "";
        type_ = FieldType::LONG; // we actually store it as an INT32, this is the result of EXTRACT(EPOCH..)
    }
    initializeFieldSize();
}






std::ostream &vaultdb::operator<<(std::ostream &os,  const QueryFieldDesc &desc)  {
    os << "#" << desc.getOrdinal() << " " << TypeUtilities::getTypeString(desc.getType());
    if(desc.getType() == FieldType::STRING || desc.getType() == FieldType::SECURE_STRING) {
        os << "(" << desc.getStringLength() << ")";
    }
    if(desc.getType() == FieldType::SECURE_INT || desc.getType() == FieldType::SECURE_LONG) {
        if(desc.size() < TypeUtilities::getTypeSize(desc.getType())) { // for bit packing
            os << "(" << desc.size() << ")";
        }
    }

    os << " " << desc.getTableName() << "." << desc.getName();
    return os;
}

QueryFieldDesc& QueryFieldDesc::operator=(const QueryFieldDesc& src)  {


    this->type_ = src.type_;
    this->field_name_ = src.field_name_;
    this->ordinal_ = src.ordinal_;
    this->table_name_ = src.table_name_;
    this->string_length_ = src.string_length_;
    this->field_size_ = src.field_size_;
    this->bit_packed_size_ = src.bit_packed_size_;
    this->field_min_ = src.field_min_;
    this->secure_field_min_ = src.secure_field_min_;

    return *this;
}

// only checking for relation compatibility, so don't care about table name or field name
// logical equality
bool QueryFieldDesc::operator==(const QueryFieldDesc& other) const {

    // if types are the same, or int32_t --> date
    if (!(this->getType() == other.getType() ||
          (this->getType() == FieldType::LONG && other.getType() == FieldType::DATE) ||
          (other.getType() == FieldType::LONG && this->getType() == FieldType::DATE))) {
        return false;
    }





    if(other.getOrdinal() != other.getOrdinal()) {

        return false;
    }

    if(other.string_length_ != this->string_length_) {

        return false;
    }

    return true;


}


void QueryFieldDesc::initializeFieldSize() {
    field_size_ = TypeUtilities::getTypeSize(type_);
    bit_packed_size_ = field_size_;

    if(table_name_ != "bit_packing") {

        // only serves ints and longs for now
        if (type_ == FieldType::SECURE_INT || type_ == FieldType::SECURE_LONG
             || type_ == FieldType::INT || type_ == FieldType::LONG) { // it will be trickier to do this with unencrypted INTs, plus not as crucial for our experiments.

            SystemConfiguration &sys_config = SystemConfiguration::getInstance();
            BitPackingDefinition bit_packing_def = sys_config.getBitPackingSpec(table_name_, field_name_);

            if(bit_packing_def.min_ != -1) { // -1 signals it is a string or has some other non-integer domain

                if ((bit_packing_def.domain_size_ == (bit_packing_def.max_ - bit_packing_def.min_ + 1) )
                  || (bit_packing_def.min_ >= 0 && bit_packing_def.max_ > bit_packing_def.min_)){ // sparser key space, happens with some of the < SF1 instance of TPC-H
                    field_min_ = bit_packing_def.min_;
                    secure_field_min_ = emp::Integer(field_size_, field_min_, PUBLIC);
                    bit_packed_size_ = ceil(log2((float) (bit_packing_def.max_ - bit_packing_def.min_ + 1)));
                    if(bit_packed_size_ == 0) bit_packed_size_ = 1;

                }

            }


            if (type_ == FieldType::SECURE_INT || type_ == FieldType::SECURE_LONG) {
                field_size_ = bit_packed_size_;
            }
        }
    }

    if(QueryFieldDesc::type_ == FieldType::STRING || QueryFieldDesc::type_ == FieldType::SECURE_STRING )  {
        field_size_ *= string_length_;
    }
}

void QueryFieldDesc::initializeFieldSizeWithCardinality(int cardinality) {
    this->field_size_ = TypeUtilities::getTypeSize(type_);
    this->bit_packed_size_ = this->field_size_;

    if(this->table_name_ != "bit_packing"){
        this->bit_packed_size_ = ceil(log2((float) (cardinality)));
        this->field_size_ = this->bit_packed_size_;
    }

}

// keep this in the cpp to avoid dependency loop in compilation
bool QueryFieldDesc::bitPacked() const {
     return field_size_ != TypeUtilities::getTypeSize(type_);
}


