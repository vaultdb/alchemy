#ifndef _SQL_INPUT_H
#define _SQL_INPUT_H


#include <util/data_utilities.h>
#include <data/psql_data_provider.h>
#include "operator.h"
#include <util/system_configuration.h>

// reads SQL input and stores in a plaintext array
namespace  vaultdb {

    class SqlInput : public Operator<bool> {

    protected:
        std::string input_query_;
        std::string db_name_;
        bool dummy_tagged_;
        StorageModel storage_model_ = SystemConfiguration::getInstance().storageModel();

    public:
        // bool denotes whether the last col of the SQL statement should be interpreted as a dummy tag
        SqlInput(std::string db, std::string sql, bool dummy_tag);
        SqlInput(std::string db, std::string sql, bool dummy_tag, const SortDefinition &sort_def,
                 const size_t &tuple_limit = 0);
        SqlInput(std::string db, std::string sql, bool dummy_tag, const SortDefinition &sort_def, const int &input_party,
                 const size_t &tuple_limit = 0);

        PlainOperator *clone() const override {
            return new SqlInput(this->db_name_, this->input_query_, this->dummy_tagged_, this->sort_definition_, this->tuple_limit_);
        }

        virtual ~SqlInput() {
        }

        void truncateInput(const size_t & limit); // to test on smaller datasets

    private:
        void runQuery();
        void runQuery(const int & input_party);

    protected:
        PlainTable *runSelf() override;
        string getOperatorTypeString() const override {     return "SqlInput"; }
        string getParameters() const override {
            string str = input_query_;
            std::replace(str.begin(), str.end(), '\n', ' ');
            return "\"" + str + "\", tuple_count=" + std::to_string(this->getOutputCardinality());
         }
        OperatorType getOperatorType() const override { return OperatorType::SQL_INPUT; }
        size_t tuple_limit_;

    };

}

#endif // _SQL_INPUT_H
