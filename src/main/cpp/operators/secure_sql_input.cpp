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
        original_input_query_(sql) {

    runQuery();
    output_schema_ = QuerySchema::toSecure(plain_input_->getSchema());
    EmpManager *manager = SystemConfiguration::getInstance().emp_manager_;
    this->output_cardinality_ = manager->getTableCardinality(plain_input_->getTupleCount());


}

SecureSqlInput::SecureSqlInput(const string &db, const string &sql, const bool &dummy_tag, const int &input_party, const size_t & input_tuple_limit, const SortDefinition &sort_def) :
        Operator(sort_def),  input_query_(sql), db_name_(db), has_dummy_tag_(dummy_tag), input_tuple_limit_(input_tuple_limit), input_party_(input_party),
        original_input_query_(sql) {

    runQuery(input_party_);
    output_schema_ = QuerySchema::toSecure(plain_input_->getSchema());
    EmpManager *manager = SystemConfiguration::getInstance().emp_manager_;
    this->output_cardinality_ = manager->getTableCardinality(plain_input_->getTupleCount());

}


SecureTable *SecureSqlInput::runSelf() {

    // secret share it
    this->start_time_ = clock_start();
    this->start_gate_cnt_ = system_conf_.andGateCount();

    return plain_input_->secretShare();

}



void SecureSqlInput::runQuery() {
    PsqlDataProvider dataProvider;
    if(input_tuple_limit_ > 0) { // truncate inputs
        // check if it ends in ";" - delete it if detected
        boost::replace_all(input_query_, ";", "");
        input_query_ = "SELECT * FROM (" + input_query_ + ") input LIMIT " + std::to_string(input_tuple_limit_);
    }

    plain_input_ = dataProvider.getQueryTable(db_name_, input_query_, has_dummy_tag_);
    plain_input_->setSortOrder(this->sort_definition_);

}

void SecureSqlInput::runQuery(const int & input_party) {
    PsqlDataProvider dataProvider;
    if(input_tuple_limit_ > 0) { // truncate inputs
        // check if it ends in ; - delete it if so
        boost::replace_all(input_query_, ";", "");
        input_query_ = "SELECT * FROM (" + input_query_ + ") input LIMIT " + std::to_string(input_tuple_limit_);
    }

    int party = SystemConfiguration::getInstance().party_;
    string local_db = db_name_;

    bool no_input = (input_party > 0 && (party != input_party));
    if(no_input) local_db = SystemConfiguration::getInstance().getEmptyDbName();

    plain_input_ = dataProvider.getQueryTable(local_db, input_query_, has_dummy_tag_);
    plain_input_->setSortOrder(this->sort_definition_);
    if(no_input) plain_input_->resize(0); // in case the domain is fixed (e.g., TPC-H Q1)


}



