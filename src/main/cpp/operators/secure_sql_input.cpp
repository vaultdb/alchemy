#include "secure_sql_input.h"

std::shared_ptr<QueryTable<SecureBoolField> > SecureSqlInput::runSelf() {
    PsqlDataProvider dataProvider;
    std::unique_ptr<QueryTable<BoolField> > plaintextTable = dataProvider.getQueryTable(dbName, inputQuery, hasDummyTag);
    if(!sortedOn.empty()) {  output->setSortOrder(sortedOn); }

    // secret share it
    output = plaintextTable->secretShare(netio_, srcParty);


    return output;
}

