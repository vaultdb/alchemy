#ifndef _CSVINPUT_H
#define _CSVINPUT_H

#include "operators/operator.h"
#include <string>

namespace vaultdb {

class CsvInput : public Operator<bool> {

protected:
    std::string input_file_;

public:
CsvInput(string srcFile, const QuerySchema & srcSchema, const SortDefinition & sort = SortDefinition()) : Operator<bool>(sort), input_file_(srcFile)  {
    output_schema_ = srcSchema;

}
CsvInput(const CsvInput &src) : Operator<bool>(src) {
    input_file_ = src.input_file_;
}

PlainTable *runSelf() override;

~CsvInput() = default;

Operator<bool> *clone() const override {
    return new CsvInput(*this);
}

void updateCollation() override {}

bool operator==(const Operator<bool> &other) const override {
    if (other.getType() != OperatorType::CSV_INPUT) {
        return false;
    }

    auto other_node = dynamic_cast<const CsvInput &>(other);

    if(this->input_file_ != other_node.input_file_) return false;
    return this->operatorEquality(other);
}

protected:

string getParameters() const override {
    return "filename=" + input_file_;
}

OperatorType getType() const override {
    return OperatorType::CSV_INPUT;
}

};

} // namespace vaultdb


#endif // _CSVINPUT_H
