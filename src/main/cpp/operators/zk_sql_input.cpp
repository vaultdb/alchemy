#include "zk_sql_input.h"

ZkSqlInput::ZkSqlInput(string db, string sql, bool dummyTag,  emp::BoolIO<NetIO> *ios[], const size_t & thread_count,  int aSrcParty, const size_t & input_tuple_limit) : ios_(ios), src_party_(aSrcParty),
                                                                                                         input_query_(sql), db_name_(db), has_dummy_tag_(dummyTag), input_tuple_limit_(input_tuple_limit),  thread_count_(thread_count) {

    runQuery();
    output_schema_ = QuerySchema::toSecure(*plain_input_->getSchema());
}

ZkSqlInput::ZkSqlInput(const string &db, const string &sql, const bool &dummyTag,
                               const SortDefinition &sortDefinition,  emp::BoolIO<NetIO> *ios[], const size_t & thread_count, const int &party, const size_t & input_tuple_limit) :
        Operator(sortDefinition), ios_(ios), src_party_(party), input_query_(sql), db_name_(db), has_dummy_tag_(dummyTag), input_tuple_limit_(input_tuple_limit), thread_count_(thread_count) {

    runQuery();
    output_schema_ = QuerySchema::toSecure(*plain_input_->getSchema());

}


shared_ptr<SecureTable> ZkSqlInput::runSelf() {
    // secret share it
    output_ = PlainTable::secretShare(plain_input_.get(), ios_, thread_count_, src_party_);
    return output_;
}



void ZkSqlInput::runQuery() {
    PsqlDataProvider dataProvider;
    if(input_tuple_limit_ > 0) { // truncate inputs
        input_query_ = "SELECT * FROM (" + input_query_ + ") input LIMIT " + std::to_string(input_tuple_limit_);
    }

    plain_input_ = dataProvider.getQueryTable(db_name_, input_query_, has_dummy_tag_);
    plain_input_->setSortOrder(getSortOrder());
}

string ZkSqlInput::getOperatorType() const {
    return "ZkSqlInput";
}

string ZkSqlInput::getParameters() const {
    return "\"" + input_query_ + "\", tuple_count=" + std::to_string(plain_input_->getTupleCount());
}


