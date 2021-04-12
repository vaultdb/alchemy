#include "secure_sql_input.h"

std::shared_ptr<SecureTable> SecureSqlInput::runSelf() {
    PsqlDataProvider dataProvider;
    std::shared_ptr<PlainTable> plaintext = dataProvider.getQueryTable(dbName, inputQuery, hasDummyTag);
    plaintext->setSortOrder(sorted_on_);

    // secret share it
    output = PlainTable::secret_share(*plaintext, netio_, srcParty);

    //std::cout << "Secret shared input: " << output->reveal()->toString(true) << std::endl;
    return output;
}

