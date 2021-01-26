#include <util/type_utilities.h>
#include "query_field.h"
using namespace vaultdb;

QueryField::QueryField(const QueryField &qf)
    : ordinal(qf.ordinal), value_(qf.value_) {}

types::Value QueryField::getValue() const {
   return value_;
}



void QueryField::setValue(const types::Value & val) {
    value_.setValue(val);
}


QueryField QueryField::reveal(const int &party) const {

    types::Value value = this->value_;
    types::Value dstValue = value_.reveal(party);
    return QueryField(this->getOrdinal(), dstValue);


}

std::ostream &vaultdb::operator<<(std::ostream &strm, const QueryField &aField) {

    return strm << aField.value_;
}



void QueryField::serialize(int8_t *dst) {
    value_.serialize(dst);
}





QueryField& QueryField::operator=(const QueryField& other) {
    if(&other == this)
        return *this;


    value_.setValue(other.getValue());
    this->ordinal = other.getOrdinal();

    return *this;

}

bool QueryField::operator==(const QueryField &other) const {

    if(this->getOrdinal() != other.getOrdinal()) {
        return false;
    }

    types::Value cmp = (value_ == other.getValue());
    return cmp.getBool(); // throws if we are in encrypted mode
}

QueryField QueryField::deserialize(const QueryFieldDesc desc, int8_t *cursor) {
    QueryField result(desc.getOrdinal());
    types::Value aValue = types::Value::deserialize(desc, cursor);
    result.setValue(aValue);
    return result;
}

QueryField QueryField::deserialize(const QueryFieldDesc desc, Bit *cursor) {
    QueryField result(desc.getOrdinal());
    types::Value aValue = types::Value::deserialize(desc, cursor);
    result.setValue(aValue);
    return result;
}
