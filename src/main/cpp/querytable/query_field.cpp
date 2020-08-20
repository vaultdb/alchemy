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


QueryField QueryField::reveal(EmpParty party) const {

    types::Value value = this->value_;
    QueryField result(*this);
    types::Value dstValue = value_.reveal(party);
    result.setValue(dstValue);
    return result;


}

std::ostream &vaultdb::operator<<(std::ostream &strm, const QueryField &aField) {

    return strm << aField.value_;
}



void QueryField::serialize(bool *dst) {
    value_.serialize(dst);
}





QueryField& QueryField::operator=(const QueryField& other) {
    if(&other == this)
        return *this;

    std::cout << "Other field:  " << other << std::endl;

    value_.setValue(other.getValue());
    this->ordinal = other.getOrdinal();

    std::cout << " new field: " << *this << std::endl;
    return *this;

}