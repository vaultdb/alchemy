#ifndef _SQL_INPUT_H
#define _SQL_INPUT_H


#include <util/data_utilities.h>
#include <data/PsqlDataProvider.h>
#include "operator.h"

// reads SQL input and stores in a plaintext array
namespace  vaultdb {

    class SqlInput : public Operator<bool> {

    protected:
        std::string inputQuery;
        std::string dbName;
        bool hasDummyTag;

    public:
        // bool denotes whether the last col of the SQL statement should be interpreted as a dummy tag
        SqlInput(std::string db, std::string sql, bool dummyTag = false);

        SqlInput(std::string db, std::string sql, bool dummyTag, const SortDefinition & sortDefinition);

        ~SqlInput() = default;
        std::shared_ptr<PlainTable> runSelf() override;
    private:
        void runQuery();

    protected:
        string getOperatorType() const override;

        string getParameters() const override;

    };

}

#endif // _SQL_INPUT_H
