#include <gflags/gflags.h>
#include <gtest/gtest.h>
#include <emp-tool/emp-tool.h>
#include <stdexcept>
#include <test/mpc/emp_base_test.h>
#include <query_table/field/field.h>




using namespace emp;
using namespace vaultdb;


DEFINE_int32(party, 1, "party for EMP execution");
DEFINE_int32(port, 54323, "port for EMP execution");
DEFINE_string(alice_host, "127.0.0.1", "alice hostname for execution");
DEFINE_string(unioned_db, "tpch_unioned_150", "unioned db name");
DEFINE_string(alice_db, "tpch_alice_150", "alice db name");
DEFINE_string(bob_db, "tpch_bob_150", "bob db name");
DEFINE_string(storage, "row", "storage model for tables (row or column)");
DEFINE_int32(ctrl_port, 65482, "port for managing EMP control flow by passing public values");
DEFINE_bool(validation, true, "run reveal for validation, turn this off for benchmarking experiments (default true)");
DEFINE_string(filter, "*", "run only the tests passing this filter");


class SecureFieldExpressionTest : public EmpBaseTest {

};



// test overload of assignment operator
TEST_F(SecureFieldExpressionTest, test_value_assignment) {
    SecureField src(emp::Bit(true));
    SecureField dst = src;

    ASSERT_EQ(src.getValue<emp::Bit>().reveal(), dst.getValue<emp::Bit>().reveal());


}

TEST_F(SecureFieldExpressionTest, test_string_compare) {

    std::string lhs_str = "EGYPT                    ";
    std::string rhs_str = "ARGENTINA                ";

    PlainField lhs(FieldType::STRING, lhs_str, lhs_str.size());
    PlainField rhs(FieldType::STRING, rhs_str, rhs_str.size());

    SecureField  lhs_shared, rhs_shared;
    if(FLAGS_party == emp::ALICE) {
        lhs_shared = PlainField::secret_share_send(lhs, QueryFieldDesc(0, "anon", "test_table", FieldType::SECURE_STRING, lhs_str.size()), emp::ALICE);
        rhs_shared = PlainField::secret_share_send(rhs, QueryFieldDesc(0, "anon", "test_table", FieldType::SECURE_STRING, rhs_str.size()), emp::ALICE);
    }
    else {
        lhs_shared = PlainField::secret_share_recv(
                QueryFieldDesc(0, "anon", "test_table", FieldType::SECURE_STRING, lhs_str.size()), emp::ALICE);
        rhs_shared = PlainField::secret_share_recv(
                QueryFieldDesc(0, "anon", "test_table", FieldType::SECURE_STRING, rhs_str.size()), emp::ALICE);
    }

    emp::Bit gt_shared = (lhs_shared > rhs_shared);
    if(FLAGS_validation) {
        bool gt =  gt_shared.reveal();
        ASSERT_TRUE(gt);
    }
}


TEST_F(SecureFieldExpressionTest, test_emp_int_math) {

    SecureField lhs, rhs;
    if(emp_mode_ == EmpMode::SH2PC) {
        // alice inputs 7, bob inputs 12
        emp::Integer alice_shared = emp::Integer(32, FLAGS_party == emp::ALICE ? 7 : 0, emp::ALICE);
        emp::Integer bob_shared = emp::Integer(32, FLAGS_party == emp::ALICE ? 0 : 12, emp::BOB);

        lhs = SecureField(FieldType::SECURE_INT, alice_shared);
        rhs = SecureField(FieldType::SECURE_INT, bob_shared);
    }
    else if(emp_mode_ == EmpMode::ZK) {
        Integer lhs_shared(32, FLAGS_party == emp::ALICE ? 7 : 0, emp::ALICE);
        Integer rhs_shared(32, FLAGS_party == emp::ALICE ? 12 : 0, emp::ALICE);

        lhs = SecureField(FieldType::SECURE_INT, lhs_shared);
        rhs = SecureField(FieldType::SECURE_INT, rhs_shared);
    }
    else { // outsourced
        Integer lhs_shared(32, FLAGS_party == emp::TP ? 7 : 0, emp::TP);
        Integer rhs_shared(32, FLAGS_party == emp::TP ? 12 : 0, emp::TP);

        lhs = SecureField(FieldType::SECURE_INT, lhs_shared);
        rhs = SecureField(FieldType::SECURE_INT, rhs_shared);

    }

    emp::Integer multiplier(32, 2); // set multiplier to two
    SecureField multiplier_field(FieldType::SECURE_INT, multiplier);



    SecureField result = (lhs + rhs) * multiplier_field;
    if(FLAGS_validation) {
        PlainField revealed = result.reveal(emp::PUBLIC);
        ASSERT_EQ(revealed.getValue<int32_t>(), 19 * 2);
    }


}




TEST_F(SecureFieldExpressionTest, test_millionaires) {

    SecureField lhs, rhs;
    if(emp_mode_ == EmpMode::SH2PC) {
        // alice inputs 7, bob inputs 12
        emp::Integer alice_shared = emp::Integer(32, FLAGS_party == emp::ALICE ? 7 : 0, emp::ALICE);
        emp::Integer bob_shared = emp::Integer(32, FLAGS_party == emp::ALICE ? 0 : 12, emp::BOB);

        lhs = SecureField(FieldType::SECURE_INT, alice_shared);
        rhs = SecureField(FieldType::SECURE_INT, bob_shared);
    }
    else if(emp_mode_ == EmpMode::ZK) {
        Integer lhs_shared(32, FLAGS_party == emp::ALICE ? 7 : 0, emp::ALICE);
        Integer rhs_shared(32, FLAGS_party == emp::ALICE ? 12 : 0, emp::ALICE);

        lhs = SecureField(FieldType::SECURE_INT, lhs_shared);
        rhs = SecureField(FieldType::SECURE_INT, rhs_shared);
    }
    else { // outsourced
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



TEST_F(SecureFieldExpressionTest, test_char_comparison) {


    char lhs = 'O';
    char rhs = 'F';

    std::string lhs_str("O"), rhs_str("F");

    bool sending_party = false;
    int sender_id = ALICE;

    if((emp_mode_ == EmpMode::OUTSOURCED && FLAGS_party == TP) || (emp_mode_ != EmpMode::OUTSOURCED && FLAGS_party == ALICE))
        sending_party = true;
    if(emp_mode_ == EmpMode::OUTSOURCED)
        sender_id = TP;



    PlainField lhs_field(FieldType::STRING, lhs_str, 1);
    PlainField rhs_field(FieldType::STRING, rhs_str, 1);

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
    return RUN_ALL_TESTS();
}





