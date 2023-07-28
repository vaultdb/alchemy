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
                                                                             input_tuple_limit_(input_tuple_limit) {
    runQuery();
    output_schema_ = QuerySchema::toSecure(plain_input_->getSchema());
}

SecureSqlInput::SecureSqlInput(const string &db, const string &sql, const bool &dummy_tag,  const SortDefinition &sort_def, const size_t & input_tuple_limit) :
        Operator(sort_def),  input_query_(sql), db_name_(db), has_dummy_tag_(dummy_tag), input_tuple_limit_(input_tuple_limit) {

    runQuery();
    output_schema_ = QuerySchema::toSecure(plain_input_->getSchema());

}

SecureSqlInput::SecureSqlInput(const string &db, const string &sql, const bool &dummy_tag, const int &input_party, const size_t & input_tuple_limit, const SortDefinition &sort_def) :
        Operator(sort_def),  input_query_(sql), db_name_(db), has_dummy_tag_(dummy_tag), input_tuple_limit_(input_tuple_limit) {

    runQuery(input_party);
    output_schema_ = QuerySchema::toSecure(plain_input_->getSchema());
}


SecureTable *SecureSqlInput::runSelf() {

    // secret share it
    this->start_time_ = clock_start();
    this->start_gate_cnt_ = system_conf_.andGateCount();

    output_ = plain_input_->secretShare();

    return output_;
}



void SecureSqlInput::runQuery() {
    PsqlDataProvider dataProvider;
    if(input_tuple_limit_ > 0) { // truncate inputs
        // check if it ends in ; - delete it if so
        boost::replace_all(input_query_, ";", "");
        input_query_ = "SELECT * FROM (" + input_query_ + ") input LIMIT " + std::to_string(input_tuple_limit_);
    }

    plain_input_ = dataProvider.getQueryTable(db_name_, input_query_, has_dummy_tag_);
    plain_input_->setSortOrder(getSortOrder());
    EmpManager *manager = SystemConfiguration::getInstance().emp_manager_;
    this->output_cardinality_ = manager->getTableCardinality(plain_input_->getTupleCount());

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
    plain_input_->setSortOrder(getSortOrder());
    if(no_input) plain_input_->resize(0); // in case the domain is fixed (e.g., TPC-H Q1)

    EmpManager *manager = SystemConfiguration::getInstance().emp_manager_;
    this->output_cardinality_ = manager->getTableCardinality(plain_input_->getTupleCount());

}

string SecureSqlInput::getOperatorType() const {
    return "SecureSqlInput";
}

string SecureSqlInput::getParameters() const {
    // N.B., this is the tuple count provided by each party
    // after secret sharing we expect more than this via the other party
    return "\"" + input_query_ + "\", tuple_count=" + std::to_string(plain_input_->getTupleCount());
}


