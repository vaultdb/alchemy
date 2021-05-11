#include "secure_sql_input.h"

SecureSqlInput::SecureSqlInput(string db, string sql, bool dummyTag, emp::NetIO *netio, int aSrcParty, const size_t & input_tuple_limit) : netio_(netio), src_party_(aSrcParty),
                                                                                                         input_query_(sql), db_name_(db), has_dummy_tag_(dummyTag), input_tuple_limit_(input_tuple_limit) {

    runQuery();
    output_schema_ = QuerySchema::toSecure(*plain_input_->getSchema());
}

SecureSqlInput::SecureSqlInput(const string &db, const string &sql, const bool &dummyTag,
                               const SortDefinition &sortDefinition, NetIO *netio, const int &party, const size_t & input_tuple_limit) :
        Operator(sortDefinition), netio_(netio), src_party_(party), input_query_(sql), db_name_(db), has_dummy_tag_(dummyTag), input_tuple_limit_(input_tuple_limit) {

    runQuery();
    output_schema_ = QuerySchema::toSecure(*plain_input_->getSchema());

}


shared_ptr<SecureTable> SecureSqlInput::runSelf() {

    // secret share it
    output_ = PlainTable::secretShare(*plain_input_, netio_, src_party_);

    //cout << "Secret shared input: " << output->reveal()->toString(true) << endl;
    return output_;
}



void SecureSqlInput::runQuery() {
    PsqlDataProvider dataProvider;
    if(input_tuple_limit_ > 0) { // truncate inputs
        input_query_ = "SELECT * FROM (" + input_query_ + ") input LIMIT " + std::to_string(input_tuple_limit_);
    }

    plain_input_ = dataProvider.getQueryTable(db_name_, input_query_, has_dummy_tag_);
    plain_input_->setSortOrder(getSortOrder());
    // one side is ok to have empty inputs - they will contribute later in a join
    //if(plain_input_->getTupleCount() <= 0) {
    //    throw std::invalid_argument("read empty input from \"" + input_query_ + "\"");
  //  }



}

string SecureSqlInput::getOperatorType() const {
    return "SecureSqlInput";
}

string SecureSqlInput::getParameters() const {
    return "\"" + input_query_ + "\", tuple_count=" + std::to_string(plain_input_->getTupleCount());
}


