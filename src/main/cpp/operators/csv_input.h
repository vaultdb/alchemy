#ifndef _CSVINPUT_H
#define _CSVINPUT_H

#include "operator.h"

namespace vaultdb {

class CsvInput : public Operator<bool> {

protected:
    std::string inputFile;
    SortDefinition sortedOn;
    QuerySchema inputSchema;

public:
    CsvInput(string srcFile, const QuerySchema & srcSchema, const SortDefinition & sort = SortDefinition()) : Operator<bool>(sort), inputFile(srcFile), inputSchema(srcSchema) {


    }


    shared_ptr<PlainTable> runSelf() override;

    ~CsvInput() = default;

};

}


#endif // _CSVINPUT_H
