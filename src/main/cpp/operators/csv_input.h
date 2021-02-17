#ifndef _CSVINPUT_H
#define _CSVINPUT_H

#include "operator.h"

namespace vaultdb {

class CsvInput : public Operator {

protected:
    std::string inputFile;
    SortDefinition sortedOn;
    QuerySchema inputSchema;

public:
    CsvInput(string srcFile, const QuerySchema & srcSchema ) : inputFile(srcFile), inputSchema(srcSchema) {


    }



    void setSortDefinition(const SortDefinition & aSortDefinition) { sortedOn = aSortDefinition; };

    shared_ptr<QueryTable> runSelf() override;

    ~CsvInput() = default;

};

}


#endif // _CSVINPUT_H
