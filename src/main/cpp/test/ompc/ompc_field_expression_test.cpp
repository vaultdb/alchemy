#include <gflags/gflags.h>
#include <gtest/gtest.h>
#include <emp-tool/emp-tool.h>
#include <stdexcept>
#include <test/ompc/ompc_base_test.h>
#include <query_table/field/field.h>


#if __has_include("emp-rescu/emp-rescu.h")


using namespace emp;
using namespace vaultdb;


DEFINE_int32(party, 1, "party for EMP execution");
DEFINE_int32(port, 54323, "port for EMP execution");
DEFINE_string(unioned_db, "tpch_unioned_150", "unioned db name");
DEFINE_int32(ctrl_port, 65482, "port for managing EMP control flow by passing public values");
DEFINE_bool(validation, true, "run reveal for validation, turn this off for benchmarking experiments (default true)");
DEFINE_string(filter, "*", "run only the tests passing this filter");
DEFINE_string(storage, "wire_packed", "storage model for columns (column or wire_packed)");
DEFINE_string(empty_db, "tpch_empty", "empty db name for schemas");
DEFINE_string(wire_path, "wires", "local path to wire files");
DEFINE_int32(input_party, 10086, "party for input data");
DEFINE_int32(unpacked_page_size_bits, 2048, "unpacked page size in bits");
DEFINE_int32(page_cnt, 50, "number of pages in buffer pool");

// TODO: Xiling, please port test_string_compare, test_emp_int_math from secure_field_expression_test


class OMPCFieldExpressionTest : public OmpcBaseTest {

};



// test overload of assignment operator
TEST_F(OMPCFieldExpressionTest, ompc_test_value_assignment) {
    SecureField src(emp::Bit(true));
    SecureField dst = src;

    ASSERT_EQ(src.getValue<emp::Bit>().reveal(), dst.getValue<emp::Bit>().reveal());


}

TEST_F(OMPCFieldExpressionTest, ompc_test_string_compare) {

    std::string lhs_str = "EGYPT                    ";
    std::string rhs_str = "ARGENTINA                ";

    PlainField lhs(FieldType::STRING, lhs_str);
    PlainField rhs(FieldType::STRING, rhs_str);

    int sending_party = manager_->sendingParty();
    if(sending_party == 0) sending_party = 1; // SH2PC has zero to mean "both send"

        SecureField  lhs_shared, rhs_shared;
        if(FLAGS_party == sending_party) {
        lhs_shared = PlainField::secret_share_send(lhs, QueryFieldDesc(0, "anon", "test_table", FieldType::SECURE_STRING, lhs_str.size()), sending_party);
        rhs_shared = PlainField::secret_share_send(rhs, QueryFieldDesc(0, "anon", "test_table", FieldType::SECURE_STRING, rhs_str.size()), sending_party);
    }
    else {
        lhs_shared = PlainField::secret_share_recv(
                QueryFieldDesc(0, "anon", "test_table", FieldType::SECURE_STRING, lhs_str.size()), sending_party);
        rhs_shared = PlainField::secret_share_recv(
                QueryFieldDesc(0, "anon", "test_table", FieldType::SECURE_STRING, rhs_str.size()), sending_party);
    }

    emp::Bit gt_shared = (lhs_shared > rhs_shared);
    if(FLAGS_validation) {
        bool gt =  gt_shared.reveal();
        ASSERT_TRUE(gt);
    }
}


TEST_F(OMPCFieldExpressionTest, ompc_test_emp_int_math) {

    SecureField lhs, rhs;
    QueryFieldDesc desc(0, "", "", FieldType::SECURE_INT, 0);

    if(emp_mode_ == EmpMode::OUTSOURCED) { // outsourced  or sh2pc_outsourced
        int sending_party = manager_->sendingParty();
        Integer lhs_shared(32, FLAGS_party == sending_party ? 7 : 0, sending_party);
        Integer rhs_shared(32, FLAGS_party == sending_party ? 12 : 0, sending_party);

        lhs = SecureField(FieldType::SECURE_INT, lhs_shared);
        rhs = SecureField(FieldType::SECURE_INT, rhs_shared);

    }

    emp::Integer multiplier(32, 2); // set multiplier to two
    SecureField multiplier_field(FieldType::SECURE_INT, multiplier);

    SecureField result = (lhs + rhs) * multiplier_field;
    if(FLAGS_validation) {
        PlainField revealed = result.reveal(desc, emp::PUBLIC);
        ASSERT_EQ(revealed.getValue<int32_t>(), 19 * 2);
    }


}




TEST_F(OMPCFieldExpressionTest, ompc_test_millionaires) {

    SecureField lhs, rhs;

    if(emp_mode_ == EmpMode::OUTSOURCED) { // outsourced
        Integer lhs_shared(32, FLAGS_party == emp::TP ? 7 : 0, emp::TP);
        Integer rhs_shared(32, FLAGS_party == emp::TP ? 12 : 0, emp::TP);

        lhs = SecureField(FieldType::SECURE_INT, lhs_shared);
        rhs = SecureField(FieldType::SECURE_INT, rhs_shared);

    }

    Bit result = (lhs > rhs);
    if(FLAGS_validation) {

        PlainField revealed = result.reveal(emp::PUBLIC);
        ASSERT_FALSE(revealed.getValue<bool>());
    }

}



TEST_F(OMPCFieldExpressionTest, ompc_test_char_comparison) {


    char lhs = 'O';
    char rhs = 'F';

    std::string lhs_str("O"), rhs_str("F");

    bool sending_party = false;
    int sender_id = ALICE;

    if((emp_mode_ == EmpMode::OUTSOURCED && FLAGS_party == TP) || (emp_mode_ != EmpMode::OUTSOURCED && FLAGS_party == ALICE))
        sending_party = true;
    if(emp_mode_ == EmpMode::OUTSOURCED)
        sender_id = TP;



    PlainField lhs_field(FieldType::STRING, lhs_str);
    PlainField rhs_field(FieldType::STRING, rhs_str);

    SecureField  lhs_shared_field, rhs_shared_field;
    if(sending_party) {
    lhs_shared_field = PlainField::secret_share_send(lhs_field, QueryFieldDesc(0, "test_col", "test_table", FieldType::SECURE_STRING, 1), sender_id);
    rhs_shared_field = PlainField::secret_share_send(rhs_field, QueryFieldDesc(0, "test_col", "test_table", FieldType::SECURE_STRING, 1), sender_id);
    }
    else {
    lhs_shared_field = PlainField::secret_share_recv(
            QueryFieldDesc(0, "anon", "test_table", FieldType::SECURE_STRING, 1), sender_id);
    rhs_shared_field = PlainField::secret_share_recv(
            QueryFieldDesc(0, "anon", "test_table", FieldType::SECURE_STRING, 1), sender_id);
    }


    bool public_eq = (lhs == rhs);
    bool secure_eq = (lhs_shared_field == rhs_shared_field).reveal();
    bool public_geq = (lhs >= rhs);
    bool secure_geq = (lhs_shared_field >= rhs_shared_field).reveal();
    bool public_gt = (lhs_str > rhs_str);
    bool secure_gt = (lhs_shared_field >= rhs_shared_field).reveal();


    ASSERT_EQ(public_eq, secure_eq);
    ASSERT_EQ(public_geq, secure_geq);
    ASSERT_EQ(public_gt, secure_gt);

}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    gflags::ParseCommandLineFlags(&argc, &argv, false);

    ::testing::GTEST_FLAG(filter)=FLAGS_filter;
    int i = RUN_ALL_TESTS();
    google::ShutDownCommandLineFlags();
    return i;

}

#else
int main(int argc, char **argv) {
    std::cout << "emp-rescu backend not found!" << std::endl;
}

#endif


