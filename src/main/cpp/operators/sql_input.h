#ifndef _SQL_INPUT_H
#define _SQL_INPUT_H


#include <util/data_utilities.h>
#include <data/PsqlDataProvider.h>
#include "operator.h"

// reads SQL input and stores in a plaintext array
namespace  vaultdb {

    class SqlInput : public Operator<bool> {

    protected:
        std::string input_query_;
        std::string db_name_;
        bool dummy_tagged_;

    public:
        // bool denotes whether the last col of the SQL statement should be interpreted as a dummy tag
        SqlInput(std::string db, std::string sql, bool dummyTag = false);
        SqlInput(std::string db, std::string sql, bool dummyTag, const SortDefinition & sortDefinition, const size_t & tuple_limit = 0);

        ~SqlInput() = default;
        void truncateInput(const size_t & limit); // to test on smaller datasets

    private:
        void runQuery();

    protected:
        std::shared_ptr<PlainTable> runSelf() override;
        string getOperatorType() const override;
        string getParameters() const override;
        size_t tuple_limit_;

    };

}

#endif // _SQL_INPUT_H
