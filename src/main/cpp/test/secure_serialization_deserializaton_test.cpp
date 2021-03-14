#include <gflags/gflags.h>
#include <gtest/gtest.h>
#include <util/type_utilities.h>
#include <util/utilities.h>
#include <stdexcept>
#include <operators/sql_input.h>
#include <test/support/EmpBaseTest.h>
#include <data/CsvReader.h>

using namespace std;
using namespace emp;
using namespace vaultdb::types;
using namespace vaultdb;


DEFINE_int32(party, 1, "party for EMP execution");
DEFINE_int32(port, 54321, "port for EMP execution");
DEFINE_string(alice_host, "127.0.0.1", "alice hostname for execution");



class SecureSerializationDeserializationTest : public EmpBaseTest {
protected:
    string currentWorkingDirectory = Utilities::getCurrentWorkingDirectory();
    string srcCsvFile = currentWorkingDirectory + "/pilot/test/input/chi-patient.csv";
    string dstRoot = currentWorkingDirectory + "/pilot/test/output/chi-patient"; // chi-patient.alice || chi-patient.bob

    static QuerySchema getInputSchema();
    string printFirstBytes(vector<int8_t> & bytes, const size_t &  byteCount);

};


QuerySchema SecureSerializationDeserializationTest::getInputSchema() {
    // CREATE TABLE patient (
//    patid int,
//    zip_marker varchar(3),
//    age_days integer,
//    sex varchar(1),
//    ethnicity bool,
//    race int,
//    numerator int default 0, -- denotes 0 = false, 1 = true
//    denom_excl int default 0 -- denotes 0 = false, 1 = true
//);
    QuerySchema targetSchema(9);
    targetSchema.putField(QueryFieldDesc(0, "patid", "patient", FieldType::INT));
    targetSchema.putField(QueryFieldDesc(1, "zip_marker", "patient", FieldType::STRING, 3));
    targetSchema.putField(QueryFieldDesc(2, "age_days", "patient", FieldType::INT));
    targetSchema.putField(QueryFieldDesc(3, "sex", "patient", FieldType::STRING, 1));
    targetSchema.putField(QueryFieldDesc(4, "ethnicity", "patient", FieldType::BOOL));
    targetSchema.putField(QueryFieldDesc(5, "race", "patient", FieldType::INT));
    targetSchema.putField(QueryFieldDesc(6, "numerator", "patient", FieldType::INT));
    targetSchema.putField(QueryFieldDesc(7, "denom_excl", "patient", FieldType::INT));
    targetSchema.putField(QueryFieldDesc(8, "site_id", "patient", FieldType::INT));

    return targetSchema;
}

string SecureSerializationDeserializationTest::printFirstBytes(vector<int8_t> & bytes, const size_t &  byteCount) {
    stringstream ss;
    assert(byteCount > 0 && byteCount <= bytes.size());
    vector<int8_t>::iterator  readPos = bytes.begin();
    ss << (int) *readPos;
    while((readPos - bytes.begin()) < (long) byteCount) {
        ++readPos;
        ss << "," << (int) *readPos;
    }
    return ss.str();
}



TEST_F(SecureSerializationDeserializationTest, serialize_and_secret_share) {
    QuerySchema srcSchema = getInputSchema();
    std::unique_ptr<QueryTable> inputTable = CsvReader::readCsv(srcCsvFile, srcSchema);
    vector<int8_t> serialized = inputTable->serialize();
    std::cout << "Initial bits, unencrypted: " << printFirstBytes(serialized, 10) << std::endl;
    SecretShares shares = inputTable->generateSecretShares();

    // discard the bits, this is just an example
}

// read file to ensure used the same secret sharing
TEST_F(SecureSerializationDeserializationTest, deserialize_and_reveal_shares) {
    string srcShareFile = (FLAGS_party == 1) ? dstRoot + ".alice"
                : dstRoot + ".bob";


    vector<int8_t> myShares = DataUtilities::readFile(srcShareFile);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    gflags::ParseCommandLineFlags(&argc, &argv, false);

    return RUN_ALL_TESTS();
}


