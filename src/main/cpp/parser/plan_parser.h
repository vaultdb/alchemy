#ifndef _PLAN_READER_H
#define _PLAN_READER_H


#include <boost/property_tree/ptree.hpp>
#include <emp-tool/emp-tool.h>
#include <operators/operator.h>
#include "util/system_configuration.h"
#include <operators/sort.h>
#include <operators/merge_input.h>
#include <algorithm>
#include <string>
#include <tuple>
#include <map>
#include <vector>

// parse this from 1) list of SQL statements, and 2) Apache Calcite JSON for secure plan
// plan generator from SQL is in vaultdb-mock repo

namespace vaultdb {

template<typename B> class SortMergeAggregate;
template<typename B> class NestedLoopAggregate;

template<typename B>
class PlanParser {
public:
    typedef std::tuple<SortDefinition, int /*parent_id*/, int /*child_id*/, std::string> SortEntry;// Adding operator type as a string.

    PlanParser(const string &db_name, const string &sql_file, const string &json_file, const int &limit = -1);

    PlanParser(const string &db_name, const string &json_file, const int &limit = -1, const bool read_from_file=true);

    PlanParser(const string &db_name, const string &json_file, const int &party = emp::PUBLIC, const int &limit = -1, const bool read_from_file=true);


    Operator<B> *getRoot() const { return root_; }
    map<int, Operator<B> * > getOperatorMap() const { return operators_; }
    vector<Operator<B> * > getSupportOps() const { return support_ops_; }
    map<int, vector<SortDefinition>> getInterestingSortOrders() { return interesting_sort_orders_; }

    Operator<B> *getOperator(const int &op_id);

    static Operator<B> *
    parse(const std::string &db_name, const string &sql_file, const string &json_file, const int &limit = -1);

    static Operator<B> *parse(const string &db_name, const string &json_plan, const int &limit = -1);

    static Operator<B> *parseJSONString(const string &db_name, const string &json_string, const int &limit = -1);

    static tuple<int, SortDefinition, int> parseSqlHeader(const string &header);

private:
    std::string db_name_;
    StorageModel storage_model_ = SystemConfiguration::getInstance().storageModel();
    int party_ = emp::PUBLIC;

    Operator<B> *root_;
    int input_limit_ = -1; // to add a limit clause to SQL statements for efficient testing
    bool zk_plan_ = false;
    bool json_only_ = false;

    // plan enumerator state
    map<int, vector<SortDefinition>> interesting_sort_orders_;
    std::map<int, Operator<B> * > operators_; // op ID --> operator instantiation
    std::vector<Operator<B> * > support_ops_; // these ones don't get an operator ID from the JSON plan
    std::map<int, std::vector<SortDefinition>> scan_sorts_; // op ID --> sort definition
    std::vector<Operator<B>*> operatorPool;

    void parseSqlInputs(const std::string &input_file);
    void parseSecurePlan(const std::string &plan_file);
    void parseSecurePlanString(const string & json_plan);

    // operator parsers
    void parseOperator(const int &operator_id, const std::string &op_name, const boost::property_tree::ptree &pt);
    Operator<B> *parseSort(const int &operator_id, const boost::property_tree::ptree &pt);
    Operator<B> *parseAggregate(const int &operator_id, const boost::property_tree::ptree &pt);
    Operator<B> *parseJoin(const int &operator_id, const boost::property_tree::ptree &pt);
    Operator<B> *parseFilter(const int &operator_id, const boost::property_tree::ptree &pt);
    Operator<B> *parseProjection(const int &operator_id, const boost::property_tree::ptree &project_tree);
    Operator<B> *parseSeqScan(const int &operator_id, const boost::property_tree::ptree &seq_scan_tree);
    Operator<B> *parseTableScan(const int &operator_id, const boost::property_tree::ptree &scan_tree);
    Operator<B> *parsePackedTableScan(const int &operator_id, const boost::property_tree::ptree &packed_table_scan_tree);
    Operator<B> *parseShrinkwrap(const int &operator_id, const boost::property_tree::ptree &pt);
    Operator<B> *parseLocalScan(const int & operator_id, const boost::property_tree::ptree &local_scan_tree);
    Operator<B> *parseUnion(const int & operator_id, const boost::property_tree::ptree &union_tree);
    void calculateAutoAggregate();

    std::vector<SortMergeAggregate<B> *> sma_vector_;
    std::vector<NestedLoopAggregate<B> *> nla_vector_;
    std::vector<Sort<B> *> sort_vector_;
    std::vector<int> agg_id_;


    // faux template specialization
    Operator<bool> *createInputOperator(const string &sql, const SortDefinition &collation, const bool &has_dummy_tag,
                        const bool &plain_has_dummy_tag, const int & input_limit, const int & input_party = 0);

    Operator<emp::Bit> *createInputOperator(const string &sql, const SortDefinition &collation, const emp::Bit &has_dummy_tag,
                        const bool &plain_has_dummy_tag, const int & input_limit, const int & input_party = 0);

    // placeholder for template specialization
    Operator<Bit> *createMergeInput(const string &sql, const bool &dummy_tag, const size_t &input_tuple_cnt, const SortDefinition &def, Bit & placeholder) {
        return new MergeInput(db_name_, sql, dummy_tag, input_tuple_cnt, def);
    }


    Operator<bool> *createMergeInput(const string &sql, const bool &dummy_tag, const size_t &input_tuple_cnt, const SortDefinition &def, bool & placeholder) {
        // this operator is N/A in plaintext mode
        throw;
    }

    // utils
    Operator<B> *getChildOperator(const int &my_operator_id, const boost::property_tree::ptree &pt) const;
};



} // namespace vaultdb


#endif // _PLAN_READER_H
