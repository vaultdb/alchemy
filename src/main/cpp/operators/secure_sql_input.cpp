#include <boost/algorithm/string/replace.hpp>
#include "secure_sql_input.h"
#include "util/field_utilities.h"

SecureSqlInput::SecureSqlInput(string db, string sql, bool dummyTag, emp::NetIO *netio, int aSrcParty, const size_t & input_tuple_limit) : netio_(netio), src_party_(aSrcParty),
                                                                                                         input_query_(sql), db_name_(db), has_dummy_tag_(dummyTag), input_tuple_limit_(input_tuple_limit) {

    runQuery();
    output_schema_ = QuerySchema::toSecure(plain_input_->getSchema());
}

SecureSqlInput::SecureSqlInput(const string &db, const string &sql, const bool &dummyTag,
                               const SortDefinition &sortDefinition, NetIO *netio, const int &party, const size_t & input_tuple_limit) :
        Operator(sortDefinition), netio_(netio), src_party_(party), input_query_(sql), db_name_(db), has_dummy_tag_(dummyTag), input_tuple_limit_(input_tuple_limit) {

    runQuery();
    output_schema_ = QuerySchema::toSecure(plain_input_->getSchema());

}


SecureTable *SecureSqlInput::runSelf() {

    // secret share it
    output_ = plain_input_->secretShare(netio_, src_party_);

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

}

string SecureSqlInput::getOperatorType() const {
    return "SecureSqlInput";
}

string SecureSqlInput::getParameters() const {
    // N.B., this is the tuple count provided by each party
    // after secret sharing we expect more than this via the other party
    return "\"" + input_query_ + "\", tuple_count=" + std::to_string(plain_input_->getTupleCount());
}


