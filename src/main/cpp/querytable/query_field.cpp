#include <util/type_utilities.h>
#include "query_field.h"
using namespace vaultdb;

QueryField::QueryField(const QueryField &qf)
    : ordinal(qf.ordinal), value_(qf.value_) {}

QueryField::QueryField(int fn, int64_t val)
    : ordinal(fn), value_(val) {}

QueryField::QueryField(int fn, int32_t val)
    : ordinal(fn), value_(val) {}

QueryField::QueryField(int fn, bool val)
        : ordinal(fn), value_(val) {}

QueryField::QueryField(int field_num, emp::Integer val)
    : ordinal(field_num),
      value_(types::TypeId::ENCRYPTED_INTEGER64, val) {}

QueryField::QueryField(int field_num, emp::Bit val)
        : ordinal(field_num),
          value_(val) {}


QueryField::QueryField(int field_num, float val)
        : ordinal(field_num), value_(val) {}


QueryField::QueryField(int field_num, double val)
    : ordinal(field_num), value_(val) {}

QueryField::QueryField(int field_num, std::string varchar) :
        ordinal(field_num),
        value_(varchar) {}


types::Value *QueryField::GetMutableValue() { return &value_; }

types::Value *QueryField::GetValue() const {
    return const_cast<types::Value *>(&value_);
}

QueryField::QueryField() {}

QueryField::QueryField(int fn) : ordinal(fn) {}

void QueryField::SetValue(const types::Value *val) {
    value_.setValue(val);
}


QueryField QueryField::reveal(EmpParty party) const {

    types::Value value = this->value_;
    QueryField result(*this);
    types::Value dstValue = value_.reveal(party);
    result.SetValue(&dstValue);
    return result;


}

std::ostream &vaultdb::operator<<(std::ostream &strm, const QueryField &aField) {

    return strm << aField.value_.getValueString();
}

void QueryField::initialize(QueryField &field) {
    value_.setValue(field.GetValue());
    ordinal = field.ordinal;
}

void QueryField::serialize(bool *dst) {
    value_.serialize(dst);
}

QueryField::QueryField(int field_num, emp::Float32 val) : ordinal(field_num), value_(val)
{}

QueryField::QueryField(int anOrdinal, emp::Integer *aVal, types::TypeId aType) : ordinal(anOrdinal), value_(aVal) {

        value_.setType(aType); // useful for differentiating int32s and varchars from int64s

}

QueryField::QueryField(int fn, types::Value aVal)  : ordinal(fn), value_(aVal){
}



QueryField& QueryField::operator=(const QueryField& other) {
    if(&other == this)
        return *this;

    std::cout << "Equality op writing " << value_.getValueString() << " "  << TypeUtilities::getTypeIdString(other.GetValue()->getType()) << std::endl;
    value_.setValue(this->GetValue());
    this->ordinal = other.getOrdinal();

    std::cout << " new field: " << *this << std::endl;
    return *this;

}