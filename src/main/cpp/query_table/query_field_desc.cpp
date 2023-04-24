#include "query_field_desc.h"
#include "util/type_utilities.h"
#include "util/system_configuration.h"

using namespace vaultdb;

// default constructor setting up unique_ptr
QueryFieldDesc::QueryFieldDesc() : type_(FieldType::INVALID) {

}

QueryFieldDesc::QueryFieldDesc(const QueryFieldDesc &f, const FieldType &type)
        : name_(f.name_), table_name_(f.table_name_),
          string_length_(f.getStringLength()), type_(type), ordinal_(f.ordinal_), bit_packed_size_(f.bit_packed_size_) { // carry over bit packed size
    initializeFieldSize();
}


QueryFieldDesc::QueryFieldDesc(const QueryFieldDesc &f, int col_num)
        : name_(f.name_), table_name_(f.table_name_), string_length_(f.string_length_), type_(f.type_), ordinal_(col_num), field_size_(f.field_size_), bit_packed_size_(f.field_size_) {
}

QueryFieldDesc::QueryFieldDesc(uint32_t anOrdinal, const string &n, const string &tab, const FieldType &aType,
                               const size_t &stringLength)
        : name_(n),
          table_name_(tab), string_length_(stringLength), type_(aType), ordinal_(anOrdinal) {
    // since we convert DATEs to int32_t in both operator land and in our verification pipeline,
    // i.e., we compare the output of our queries to SELECT EXTRACT(EPOCH FROM date_)
    // fields of type date have no source table
    if (type_ == FieldType::DATE) {
        table_name_ = "";
        type_ = FieldType::LONG; // we actually store it as an INT32, this is the result of EXTRACT(EPOCH..)
    }
    //string_length_ = stringLength;
    initializeFieldSize();
}



int QueryFieldDesc::getOrdinal() const {
  return QueryFieldDesc::ordinal_;
}


const std::string &QueryFieldDesc::getName() const {
  return QueryFieldDesc::name_;
}

FieldType QueryFieldDesc::getType() const {
    return type_;
}


const std::string &QueryFieldDesc::getTableName() const {
  return QueryFieldDesc::table_name_;
}

size_t QueryFieldDesc::size() const {
    // if size == 0 then it is an invalid field
    assert(field_size_ != 0);

    return field_size_;
}

void QueryFieldDesc::setStringLength(size_t len) {

    string_length_ = len;
    initializeFieldSize();

}

std::ostream &vaultdb::operator<<(std::ostream &os,  const QueryFieldDesc &desc)  {
    os << "#" << desc.getOrdinal() << " " << TypeUtilities::getTypeString(desc.getType());
    if(desc.getType() == FieldType::STRING || desc.getType() == FieldType::SECURE_STRING) {
        os << "(" << desc.getStringLength() << ")";
    }

    os << " " << desc.getTableName() << "." << desc.getName();
    return os;
}

QueryFieldDesc& QueryFieldDesc::operator=(const QueryFieldDesc& src)  {


    this->type_ = src.type_;
    this->name_ = src.name_;
    this->ordinal_ = src.ordinal_;
    this->table_name_ = src.table_name_;
    this->string_length_ = src.string_length_;
    this->field_size_ = src.field_size_;
    this->bit_packed_size_ = src.bit_packed_size_;

    return *this;
}

// only checking for relation compatibility, so don't care about table name or field name
bool QueryFieldDesc::operator==(const QueryFieldDesc& other) {

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


    // only serves ints and longs for now
    if((type_ == FieldType::SECURE_INT || type_ == FieldType::SECURE_LONG
        || type_ == FieldType::INT || type_ == FieldType::LONG) && table_name_ != "bit_packing") { // it will be trickier to do this with unencrypted INTs, plus not as crucial for our experiments.
        SystemConfiguration &sys_config = SystemConfiguration::getInstance();
        BitPackingDefinition bit_packing_def = sys_config.getBitPackingSpec(table_name_, name_);

        if (bit_packing_def.domain_size_ == (bit_packing_def.max_ - bit_packing_def.min_ + 1)) {

            field_offset_ = bit_packing_def.min_;
            bit_packed_size_ = ceil(log2((float) bit_packing_def.domain_size_)); // how many bits do we need?
            if (type_ == FieldType::SECURE_INT || type_ == FieldType::SECURE_LONG) {
                field_size_ = bit_packed_size_;
            }
        }
    }

    if(QueryFieldDesc::type_ == FieldType::STRING || QueryFieldDesc::type_ == FieldType::SECURE_STRING )  {
        field_size_ *= string_length_;
    }
}


