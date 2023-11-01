#include "plain/plain_base_test.h"
#include <util/type_utilities.h>
#include <operators/sql_input.h>
#include <data/csv_reader.h>
#include "pilot/src/common/shared_schema.h"
#include "query_table/query_table.h"

using namespace std;

#if __has_include("emp-rescu/emp-rescu.h")
static EmpMode _emp_mode_ = EmpMode::OUTSOURCED;
#elif  __has_include("emp-sh2pc/emp-sh2pc.h")
static EmpMode _emp_mode_ = EmpMode::SH2PC;
#elif __has_include("emp-zk/emp-zk.h")
static EmpMode _emp_mode_ = EmpMode::ZK;
#else
    static EmpMode _emp_mode_ = EmpMode::PLAIN;
#endif



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

    PlainTable *input = DataUtilities::getQueryResults(db_name_, inputQuery, false);

    vector<int8_t> table_data = input->serialize();
    uint32_t expected_size = input->getSchema().size() / 8 * 10 + 1;
    ASSERT_EQ(table_data.size(), expected_size);

    PlainTable *deserialized = QueryTable<bool>::deserialize(input->getSchema(), table_data);
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

     PlainTable *deserialized = QueryTable<bool>::deserialize(schema, serialized);

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

    PlainTable *deserialized = QueryTable<bool>::deserialize(targetSchema, serialized);

    ASSERT_EQ(*input, *deserialized);

    delete deserialized;
    delete input;


}



TEST_F(SerializationTest, capricorn_deserialization) {
if(_emp_mode_ == EmpMode::SH2PC) {

    QuerySchema target_schema = SharedSchema::getInputSchema();
    string cwd = Utilities::getCurrentWorkingDirectory();
    string alice_file = cwd + "/pilot/test/output/chi-patient-multisite.alice";
    string bob_file = cwd + "/pilot/test/output/chi-patient-multisite.bob";

    vector<int8_t> alice_bits = DataUtilities::readFile(alice_file);
    vector<int8_t> bob_bits = DataUtilities::readFile(bob_file);

    vector<int8_t> serialized = alice_bits;

    auto write_pos = serialized.begin();
    auto read_pos = bob_bits.begin();

    while (write_pos != serialized.end()) {
        *write_pos ^= *read_pos;
        ++write_pos;
        ++read_pos;
    }

    PlainTable *deserialized = QueryTable<bool>::deserialize(target_schema, serialized);

    std::string expected_query = "SELECT  study_year, pat_id, age_strata, sex, ethnicity, race, numerator, denominator, denom_excl FROM patient WHERE site_id=3 AND multisite ORDER BY study_year, pat_id";
    std::string db_name = "enrich_htn_unioned_3pc";

    PlainTable *expected = DataUtilities::getQueryResults(db_name, expected_query, false);

    ASSERT_EQ(*expected, *deserialized);

    delete expected;
    delete deserialized;
}

}



int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    gflags::ParseCommandLineFlags(&argc, &argv, false);

    return RUN_ALL_TESTS();
}









