#include <boost/algorithm/string/replace.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <regex>
#include "secure_sql_input.h"
#include "util/field_utilities.h"

using namespace vaultdb;
using namespace std;

SecureSqlInput::SecureSqlInput(const string & db, const string & sql, const bool & dummy_tag, const size_t &input_tuple_limit) :
                                                                             input_query_(sql), db_name_(db),
                                                                             has_dummy_tag_(dummy_tag),
                                                                             input_tuple_limit_(input_tuple_limit),
                                                                             original_input_query_(sql) {
    runQuery();
    output_schema_ = QuerySchema::toSecure(plain_input_->getSchema());
    EmpManager *manager = SystemConfiguration::getInstance().emp_manager_;
    this->output_cardinality_ = manager->getTableCardinality(plain_input_->getTupleCount());


}

// assume that sql is sorted on given sort definition
SecureSqlInput::SecureSqlInput(const string &db, const string &sql, const bool &dummy_tag,  const SortDefinition &sort_def, const size_t & input_tuple_limit) :
        Operator(sort_def),  input_query_(sql), db_name_(db), has_dummy_tag_(dummy_tag), input_tuple_limit_(input_tuple_limit),
        original_input_query_(sql), original_collation_(sort_def) {

    runQuery();
    output_schema_ = QuerySchema::toSecure(plain_input_->getSchema());
    EmpManager *manager = SystemConfiguration::getInstance().emp_manager_;
    this->output_cardinality_ = manager->getTableCardinality(plain_input_->getTupleCount());


}

SecureSqlInput::SecureSqlInput(const string &db, const string &sql, const bool &dummy_tag, const int &input_party, const size_t & input_tuple_limit, const SortDefinition &sort_def) :
        Operator(sort_def),  input_query_(sql), db_name_(db), has_dummy_tag_(dummy_tag), input_tuple_limit_(input_tuple_limit), input_party_(input_party),
        original_input_query_(sql), original_collation_(sort_def) {

    runQuery();
    output_schema_ = QuerySchema::toSecure(plain_input_->getSchema());
    EmpManager *manager = SystemConfiguration::getInstance().emp_manager_;
    this->output_cardinality_ = manager->getTableCardinality(plain_input_->getTupleCount());

}


SecureTable *SecureSqlInput::runSelf() {

    if(plain_input_ == nullptr || plain_input_->getSortOrder() != this->sort_definition_) {
        runQuery();
    }

    // secret share it
    this->start_time_ = clock_start();
    this->start_gate_cnt_ = system_conf_.andGateCount();

    return plain_input_->secretShare();

}



void SecureSqlInput::runQuery() {
    PsqlDataProvider data_provider;
    string sql = input_query_;
    bool has_input = true;
    int party = SystemConfiguration::getInstance().party_;
    EmpMode emp_mode = SystemConfiguration::getInstance().emp_mode_;

    if(input_party_ > 0) {
        has_input = (party == input_party_);
    }
    if(emp_mode == EmpMode::ZK && party == 2) {
        has_input = false;
    }

    // for now we only have input from TP
    if(emp_mode == EmpMode::OUTSOURCED && input_party_ == 1 && party == emp::TP) {
        has_input = true;
    }


    string local_db = (!has_input) ? SystemConfiguration::getInstance().getEmptyDbName() : db_name_;

    // running the query on all parties to get the schema
    if(input_tuple_limit_ > 0) { // truncate inputs
        // check if it ends in ";" - delete it if detected
        boost::replace_all(input_query_, ";", "");
        sql = "SELECT * FROM (" + input_query_ + ") input LIMIT " + std::to_string(input_tuple_limit_);
    }

    plain_input_ = data_provider.getQueryTable(local_db, sql, has_dummy_tag_);
    plain_input_->setSortOrder(this->sort_definition_);

    if(!has_input) plain_input_->resize(0); // just to double-check - some cases like Q1 might need this for fixed domain from supporting table like order_keys


}




