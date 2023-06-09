#include "plain/plain_base_test.h"
#include <util/type_utilities.h>
#include <operators/sql_input.h>
#include <data/csv_reader.h>
#include "pilot/src/common/shared_schema.h"
#include "query_table/table_factory.h"

DEFINE_string(storage, "row", "storage model for tables (row or column)");

using namespace std;


class SerializationTest : public PlainBaseTest {
protected:
  static QuerySchema getCapricornSchema();

};


TEST_F(SerializationTest, typesTest) {
    // selecting one of each type:
    // int
    // varchar(44)
    // fixed char(1)
    // numeric
    // date

   std::string inputQuery = "SELECT l_orderkey, l_comment, l_returnflag, l_discount, "
                            "CAST(EXTRACT(EPOCH FROM l_commitdate) AS BIGINT) AS l_commitdate "
                            "FROM lineitem "
                            "ORDER BY l_shipdate "
                            "LIMIT 10";

    PlainTable *input = DataUtilities::getQueryResults(db_name_, inputQuery, storage_model_, false);

    vector<int8_t> table_data = input->serialize();
    uint32_t expected_size = input->getSchema().size() / 8 * 10 + 1;
    ASSERT_EQ(table_data.size(), expected_size);

    PlainTable *deserialized = TableFactory<bool>::deserialize(input->getSchema(), table_data, storage_model_);
    ASSERT_EQ(*input, *deserialized);

    delete deserialized;
    delete input;

}


QuerySchema SerializationTest::getCapricornSchema() {
    QuerySchema targetSchema;
    // 12,5,U,U,4,f,f
    targetSchema.putField(QueryFieldDesc(0, "pat_id", "patient", FieldType::INT));
    targetSchema.putField(QueryFieldDesc(1, "age_strata", "patient", FieldType::STRING, 1));
    targetSchema.putField(QueryFieldDesc(2, "sex", "patient", FieldType::STRING, 1));
    targetSchema.putField(QueryFieldDesc(3, "ethnicity", "patient", FieldType::STRING, 1));
    targetSchema.putField(QueryFieldDesc(4, "race", "patient", FieldType::STRING, 1));
    targetSchema.putField(QueryFieldDesc(5, "numerator", "patient", FieldType::BOOL));
    targetSchema.putField(QueryFieldDesc(6, "denom_excl", "patient", FieldType::BOOL));

    targetSchema.initializeFieldOffsets();
    return targetSchema;
}



TEST_F(SerializationTest, capricorn_test) {


  QuerySchema schema = getCapricornSchema();
    

    
    string cwd = Utilities::getCurrentWorkingDirectory();
    string src_csv = cwd + "/pilot/test/input/chi-multisite-patient.csv";

    PlainTable *input = CsvReader::readCsv(src_csv, schema);

     vector<int8_t> serialized = input->serialize();

     PlainTable *deserialized = TableFactory<bool>::deserialize(schema, serialized, storage_model_);

    ASSERT_EQ(*input, *deserialized);

    delete input;
    delete deserialized;



}


TEST_F(SerializationTest, xored_serialization_test) {

    QuerySchema targetSchema = getCapricornSchema();


    string cwd = Utilities::getCurrentWorkingDirectory();
    string src_csv = cwd + "/pilot/test/input/chi-multisite-patient.csv";

    PlainTable *input = CsvReader::readCsv(src_csv, targetSchema);
    vector<int8_t> serialized = input->serialize();

    // alice contains random values... ssh!
    string aliceFile = cwd + "/pilot/test/output/chi-patient-multisite.alice";
    vector<int8_t> rand_ints =  DataUtilities::readFile(aliceFile);

    auto write_pos = serialized.begin();
    auto read_pos = rand_ints.begin();

    while(write_pos != serialized.end()) {
        *write_pos ^= *read_pos;
        ++write_pos;
        ++read_pos;
    }


    // repeat the process again to decrypt it

    write_pos = serialized.begin();
    read_pos = rand_ints.begin();

    while(write_pos != serialized.end()) {
        *write_pos ^= *read_pos;
        ++write_pos;
        ++read_pos;
    }


    PlainTable *deserialized = TableFactory<bool>::deserialize(targetSchema, serialized, storage_model_);

    ASSERT_EQ(*input, *deserialized);

    delete deserialized;
    delete input;


}



TEST_F(SerializationTest, capricorn_deserialization) {


  QuerySchema target_schema = SharedSchema::getInputSchema();
    string cwd = Utilities::getCurrentWorkingDirectory();
    string alice_file = cwd + "/pilot/test/output/chi-patient-multisite.alice";
    string bob_file = cwd + "/pilot/test/output/chi-patient-multisite.bob";

    vector<int8_t> alice_bits = DataUtilities::readFile(alice_file);
    vector<int8_t> bob_bits = DataUtilities::readFile(bob_file);

    vector<int8_t> serialized = alice_bits;

    auto write_pos = serialized.begin();
    auto read_pos = bob_bits.begin();

    while(write_pos != serialized.end()) {
        *write_pos ^= *read_pos;
        ++write_pos;
        ++read_pos;
    }

    PlainTable *deserialized = TableFactory<bool>::deserialize(target_schema, serialized, storage_model_);

    std::string expected_query = "SELECT  study_year, pat_id, age_strata, sex, ethnicity, race, numerator, denominator, denom_excl FROM patient WHERE site_id=3 AND multisite ORDER BY study_year, pat_id";
    std::string db_name = "enrich_htn_unioned_3pc";

    PlainTable *expected = DataUtilities::getQueryResults(db_name, expected_query, storage_model_, false);

    ASSERT_EQ(*expected, *deserialized);

    delete expected;
    delete deserialized;



}


