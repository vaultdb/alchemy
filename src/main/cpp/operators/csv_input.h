#ifndef _CSVINPUT_H
#define _CSVINPUT_H

#include "operator.h"

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

protected:

    string getParameters() const {
        return "filename=" + input_file_;
    }

    OperatorType getType() const {
        return OperatorType::CSV_INPUT;
    }

};

}


#endif // _CSVINPUT_H
