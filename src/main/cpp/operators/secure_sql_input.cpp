#include "secure_sql_input.h"

SecureSqlInput::SecureSqlInput(string db, string sql, bool dummyTag, emp::NetIO *netio, int aSrcParty) : netio_(netio), src_party_(aSrcParty),
                                                                                                         input_query_(sql), db_name_(db), has_dummy_tag_(dummyTag) {

    runQuery();
    output_schema_ = QuerySchema::toSecure(*plain_input_->getSchema());
}

SecureSqlInput::SecureSqlInput(const string &db, const string &sql, const bool &dummyTag,
                               const SortDefinition &sortDefinition, NetIO *netio, const int &party) :
        Operator(sortDefinition), netio_(netio), src_party_(party), input_query_(sql), db_name_(db), has_dummy_tag_(dummyTag) {

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
    plain_input_ = dataProvider.getQueryTable(db_name_, input_query_, has_dummy_tag_);
    plain_input_->setSortOrder(getSortOrder());


}

