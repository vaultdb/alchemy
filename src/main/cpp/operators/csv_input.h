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


    PlainTable *runSelf() override;

    ~CsvInput() = default;

protected:
    string getOperatorType() const override;

    string getParameters() const override;

};

}


#endif // _CSVINPUT_H
