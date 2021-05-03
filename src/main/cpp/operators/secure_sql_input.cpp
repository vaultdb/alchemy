#include "secure_sql_input.h"

std::shared_ptr<SecureTable> SecureSqlInput::runSelf() {
    PsqlDataProvider dataProvider;
    std::shared_ptr<PlainTable> plaintext = dataProvider.getQueryTable(dbName, inputQuery, hasDummyTag);

    plaintext->setSortOrder(getSortOrder());

    // secret share it
    output_ = PlainTable::secretShare(*plaintext, netio_, srcParty);

    //std::cout << "Secret shared input: " << output->reveal()->toString(true) << std::endl;
    return output_;
}

