#ifndef _PLAN_READER_H
#define _PLAN_READER_H

#include <string>
#include <tuple>
#include <map>
#include <boost/property_tree/ptree.hpp>
#include <emp-tool/emp-tool.h>
#include <operators/operator.h>
#include "util/system_configuration.h"
#include <operators/nested_loop_aggregate.h>
#include <operators/group_by_aggregate.h>
#include <operators/sort.h>


// parse this from 1) list of SQL statements, and 2) Apache Calcite JSON for secure plan
// plan generator from SQL is in vaultdb-mock repo

namespace vaultdb {

    template<typename B>
    class PlanParser {
    public:
        PlanParser(const string &db_name, const string & sql_file, const string & json_file, const int &limit = -1);
        // for ZK plans
        PlanParser(const string &db_name, const string & json_file, const int &limit = -1);


        Operator<B> *getRoot() const { return root_; }
        Operator<B> *getOperator(const int & op_id);

        static Operator<B> *parse(const std::string & db_name, const string & sql_file, const string & json_file, const int & limit = -1);
        // for ZK plans
        static Operator <B> *parse(const string &db_name, const string &json_plan, const int &limit = -1);

        static tuple<int, SortDefinition, int> parseSqlHeader(const string & header);

        bool getAutoFlag() const { return agg_auto_flag; }
        void setAutoFlag(bool inputFlag) { agg_auto_flag = inputFlag;}

    protected:
        std::string db_name_;
        StorageModel storage_model_ = SystemConfiguration::getInstance().storageModel();


        Operator<B>  *root_;
        int input_limit_ = -1; // to add a limit clause to SQL statements for efficient testing
        bool zk_plan_ = false;

        std::map<int, Operator<B> * > operators_; // op ID --> operator instantiation
        std::vector<Operator<B> * > support_ops_; // these ones don't get an operator ID from the JSON plan

        void parseSqlInputs(const std::string & input_file);
        void parseSecurePlan(const std::string & plan_file);

        // operator parsers
        void parseOperator(const int & operator_id, const std::string & op_name, const  boost::property_tree::ptree &pt);
        Operator<B> *parseSort(const int &operator_id, const boost::property_tree::ptree &pt);
        Operator<B> *parseAggregate(const int & operator_id, const boost::property_tree::ptree &pt);
        Operator<B> *parseJoin(const int & operator_id, const boost::property_tree::ptree &pt);
        Operator<B> *parseFilter(const int & operator_id, const boost::property_tree::ptree &pt);
        Operator<B> *parseProjection(const int & operator_id, const boost::property_tree::ptree &project_tree);
        Operator<B> *parseSeqScan(const int & operator_id, const boost::property_tree::ptree &seq_scan_tree);
        Operator<B> *parseShrinkwrap(const int & operator_id, const boost::property_tree::ptree &pt);
        void calculateAutoAggregate();

        bool agg_auto_flag = false;
        std::vector<GroupByAggregate<B> * > sma_vector;
        std::vector<NestedLoopAggregate<B> * > nla_vector;
        std::vector<Sort<B> * > sort_vector;
        std::vector<int> agg_id;


        // faux template specialization
        Operator<bool> *createInputOperator(const string &sql, const SortDefinition &collation, const bool &has_dummy_tag, const bool & plain_has_dummy_tag);
        Operator<emp::Bit> *createInputOperator(const string &sql, const SortDefinition &collation, const emp::Bit &has_dummy_tag, const bool & plain_has_dummy_tag);
        Operator<bool> *createInputOperator(const string &sql, const SortDefinition &collation, const int &input_party, const bool &has_dummy_tag, const bool & plain_has_dummy_tag);
        Operator<emp::Bit> *createInputOperator(const string &sql, const SortDefinition &collation, const int &input_party, const emp::Bit &has_dummy_tag, const bool & plain_has_dummy_tag);


        // utils
        Operator<B> * getChildOperator(const int & my_operator_id, const boost::property_tree::ptree &pt) const;
        const std::string truncateInput(const std::string sql) const;




    };





}



#endif // _PLAN_READER_H
