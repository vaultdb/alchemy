#ifndef _SQL_INPUT_H
#define _SQL_INPUT_H


#include <util/data_utilities.h>
#include <data/psql_data_provider.h>
#include "operator.h"

// reads SQL input and stores in a plaintext array
namespace  vaultdb {

    class SqlInput : public Operator<bool> {

    protected:
        std::string input_query_;
        std::string db_name_;
        bool dummy_tagged_;
        StorageModel storage_model_ = StorageModel::ROW_STORE;

    public:
        // bool denotes whether the last col of the SQL statement should be interpreted as a dummy tag
        SqlInput(std::string db, std::string sql, const StorageModel & model, bool dummy_tag = false);
        SqlInput(std::string db, std::string sql, bool dummy_tag, const StorageModel & model, const SortDefinition & sort_def, const size_t & tuple_limit = 0);
        virtual ~SqlInput() {
//            if(output_ != nullptr) {
//                delete output_;
//            }
        }

        void truncateInput(const size_t & limit); // to test on smaller datasets

    private:
        void runQuery();

    protected:
        PlainTable *runSelf() override;
        string getOperatorType() const override;
        string getParameters() const override;
        size_t tuple_limit_;

    };

}

#endif // _SQL_INPUT_H
