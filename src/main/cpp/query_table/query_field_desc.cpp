#include "query_field_desc.h"
#include "util/utilities.h"
#include "util/type_utilities.h"

#include "util/system_configuration.h"

using namespace vaultdb;



QueryFieldDesc::QueryFieldDesc(const QueryFieldDesc &f, const FieldType &type)
        : field_name_(f.field_name_), table_name_(f.table_name_),
          string_length_(f.getStringLength()),
          type_(type), ordinal_(f.ordinal_), field_min_(f.field_min_), bit_packed_size_(f.bit_packed_size_), packed_wires_(f.packed_wires_)  { // carry over bit packed size
    initializeFieldSize();
}


QueryFieldDesc::QueryFieldDesc(const QueryFieldDesc &f, const int &  col_num)
        : field_name_(f.field_name_),
          table_name_(f.table_name_),
          string_length_(f.string_length_),
          type_(f.type_), ordinal_(col_num),
          field_size_(f.field_size_),
          field_min_(f.field_min_), bit_packed_size_(f.field_size_), packed_wires_(f.packed_wires_)  {
}

QueryFieldDesc::QueryFieldDesc(const int & ordinal, const string &col_name, const string &table_name, const FieldType &type,
                               const size_t &str_len)
        : field_name_(col_name),
          table_name_(table_name), string_length_(str_len), type_(type), ordinal_(ordinal) {
    // since we convert DATEs to int32_t in both operator land and in our verification pipeline,
    // i.e., we compare the output of our queries to SELECT EXTRACT(EPOCH FROM date_)
    // fields of type date have no source table
    if (type_ == FieldType::DATE) {
        // table_name_ = "";
        type_ = FieldType::LONG; // we actually store it as an INT64, this is the result of EXTRACT(EPOCH..)
    }
    initializeFieldSize();
}


// spec is of the form "table_name.field_name:field_type"
// e.g. "nation.n_name:varchar(25)"
// table name is optional
QueryFieldDesc::QueryFieldDesc(const int & ordinal, const string & field_spec) :  string_length_(0), ordinal_(ordinal) {

    auto dot_pos = field_spec.find('.');
    auto colon_pos = field_spec.find(':');
    if (dot_pos != string::npos) {
        assert(dot_pos < colon_pos);
        table_name_ = field_spec.substr(0, dot_pos);
        field_name_ = field_spec.substr(dot_pos + 1, colon_pos - dot_pos - 1);
    } else {
        field_name_ = field_spec.substr(0, colon_pos);
    }

    string type_str = field_spec.substr(colon_pos + 1);
    if(type_str.find('(') != string::npos) {
        auto open_paren_pos = type_str.find('(');
        auto close_paren_pos = type_str.find(')');
        string_length_ = std::stoi(type_str.substr(open_paren_pos + 1, close_paren_pos - open_paren_pos - 1));
        type_str = type_str.substr(0, open_paren_pos);
    }

    type_ = TypeUtilities::getTypeFromString(type_str);
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

    if(desc.getTableName().empty()) {
        os << " " << desc.getName();
        return os;
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
    this->packed_wires_ = src.packed_wires_;


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
        packed_wires_ = TypeUtilities::packedWireCount(field_size_);
    }
}

void QueryFieldDesc::initializeFieldSizeWithCardinality(int cardinality) {
    assert(type_ == FieldType::SECURE_INT || type_ == FieldType::SECURE_LONG);

    this->field_size_ = TypeUtilities::getTypeSize(type_);
    this->bit_packed_size_ = this->field_size_;
    this->field_min_ = 0;

    if(this->table_name_ != "bit_packing"){
        this->bit_packed_size_ = ceil(log2((float) (cardinality )));
        if(bit_packed_size_ == 0) bit_packed_size_ = 1;
        this->field_size_ = this->bit_packed_size_;
    }

}

// keep this in the cpp to avoid dependency loop in compilation
bool QueryFieldDesc::bitPacked() const {
    return (field_size_ != (int) TypeUtilities::getTypeSize(type_)) && (type_ == FieldType::SECURE_INT || type_ == FieldType::SECURE_LONG);
}


string QueryFieldDesc::prettyPrint() const {
    stringstream ss;
    if(table_name_ != "") {
        ss << table_name_ << ".";
    }

    ss << field_name_ << ":";

    ss  << TypeUtilities::getTypeString(type_);
    if(type_ == FieldType::STRING || type_ == FieldType::SECURE_STRING) {
        ss << "(" << string_length_ << ")";
    }

    return ss.str();
}