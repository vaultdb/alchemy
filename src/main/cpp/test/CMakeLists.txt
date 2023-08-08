#include(../cmake/common.cmake)
include(GoogleTest)
#cmake_minimum_required (VERSION 3.14)


if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
	if(NOT DEFINED GTEST_LIBRARY)
		FetchContent_Declare(
				googletest
				GIT_REPOSITORY https://github.com/google/googletest.git
				GIT_TAG        release-1.8.0
		)

		FetchContent_GetProperties(googletest)
		if(NOT googletest_POPULATED)
			FetchContent_Populate(googletest)
			add_subdirectory(${googletest_SOURCE_DIR} ${googletest_BINARY_DIR})
		endif()

	endif()
endif()


mark_as_advanced(
		BUILD_GMOCK BUILD_GTEST BUILD_SHARED_LIBS
		gmock_build_tests gtest_build_samples gtest_build_tests
		gtest_disable_pthreads gtest_force_shared_crt gtest_hide_internal_symbols
)

set_target_properties(gtest PROPERTIES FOLDER build)
set_target_properties(gtest_main PROPERTIES FOLDER build)
set_target_properties(gmock PROPERTIES FOLDER build)
set_target_properties(gmock_main PROPERTIES FOLDER build)
# back out of test directory for current working directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../bin)

macro(package_add_test TESTNAME)
	# create an exectuable in which the tests will be stored
	add_executable(${TESTNAME} ${ARGN})
	# link the Google test infrastructure, mocking library, and a default main fuction to
	# the test executable.  Remove g_test_main if writing your own main function.
	target_link_libraries(${TESTNAME} gtest gmock gtest_main vaultdb-emp)
	# gtest_discover_tests replaces gtest_add_tests,
	# see https://cmake.org/cmake/help/v3.10/module/GoogleTest.html for more options to pass to it
	gtest_discover_tests(${TESTNAME}
			# set a working directory so your project root so that you can find test data via paths relative to the project root
			WORKING_DIRECTORY ${PROJECT_DIR}
			PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${PROJECT_DIR}"
			DISCOVERY_MODE PRE_TEST)
	set_target_properties(${TESTNAME} PROPERTIES FOLDER bin)
endmacro()


#non-private tests
package_add_test(field_expression_test field_expression_test.cpp plain/plain_base_test.cpp)
package_add_test(csv_reader_test csv_reader_test.cpp plain/plain_base_test.cpp)
package_add_test(filter_test plain/filter_test.cpp plain/plain_base_test.cpp)
package_add_test(project_test plain/project_test.cpp plain/plain_base_test.cpp)
package_add_test(sort_test plain/sort_test.cpp plain/plain_base_test.cpp)
package_add_test(basic_join_test plain/basic_join_test.cpp plain/plain_base_test.cpp)
package_add_test(keyed_join_test plain/keyed_join_test.cpp plain/plain_base_test.cpp)
package_add_test(scalar_aggregate_test plain/scalar_aggregate_test.cpp plain/plain_base_test.cpp)
package_add_test(group_by_aggregate_test plain/group_by_aggregate_test.cpp plain/plain_base_test.cpp)
package_add_test(sort_merge_join_test plain/sort_merge_join_test.cpp plain/plain_base_test.cpp)
package_add_test(nested_loop_aggregate_test plain/nested_loop_aggregate_test.cpp plain/plain_base_test.cpp)
package_add_test(merge_join_test plain/merge_join_test.cpp plain/plain_base_test.cpp)

package_add_test(serialization_test serialization_test.cpp plain/plain_base_test.cpp ../pilot/src/common/shared_schema.cpp)
package_add_test(secret_share_generator_test secret_share_generator_test.cpp plain/plain_base_test.cpp)
package_add_test(plan_parser_test plan_parser_test.cpp plain/plain_base_test.cpp)
package_add_test(tpch_test tpch_test.cpp plain/plain_base_test.cpp)

#EMP-based tests
package_add_test(emp_test mpc/emp_test.cpp mpc/emp_base_test.cpp)
package_add_test(secure_field_expression_test mpc/secure_value_expression_test.cpp mpc/emp_base_test.cpp)
package_add_test(emp_table_test  mpc/emp_table_test.cpp mpc/emp_base_test.cpp) # port 54324-54325
#package_add_test(emp_test_multiple_starts mpc/emp_test_multiple_starts.cpp)
package_add_test(secure_filter_test mpc/secure_filter_test.cpp mpc/emp_base_test.cpp)
package_add_test(secure_sort_test mpc/secure_sort_test.cpp mpc/emp_base_test.cpp)
package_add_test(secure_basic_join_test mpc/secure_basic_join_test.cpp mpc/emp_base_test.cpp)
package_add_test(secure_merge_join_test mpc/secure_merge_join_test.cpp mpc/emp_base_test.cpp)
package_add_test(secure_keyed_join_test mpc/secure_keyed_join_test.cpp mpc/emp_base_test.cpp)
package_add_test(secure_sort_merge_join_test mpc/secure_sort_merge_join_test.cpp mpc/emp_base_test.cpp)
package_add_test(secure_scalar_aggregate_test mpc/secure_scalar_aggregate_test.cpp mpc/emp_base_test.cpp)
package_add_test(secure_group_by_aggregate_test mpc/secure_group_by_aggregate_test.cpp mpc/emp_base_test.cpp)
package_add_test(secure_nested_loop_aggregate_test mpc/secure_nested_loop_aggregate_test.cpp mpc/emp_base_test.cpp)
package_add_test(enrich_test enrich_test.cpp  support/enrich_test_support.cpp mpc/emp_base_test.cpp)
package_add_test(secure_tpch_test mpc/secure_tpch_test.cpp mpc/emp_base_test.h mpc/emp_base_test.cpp)
package_add_test(baseline_comparison_test mpc/baseline_comparison_test.cpp mpc/emp_base_test.h mpc/emp_base_test.cpp)
package_add_test(secure_bit_packing_test mpc/secure_bit_packing_test.cpp mpc/emp_base_test.h mpc/emp_base_test.cpp)
package_add_test(secure_aggregate_comparison_test mpc/secure_aggregate_comparison_test.cpp mpc/emp_base_test.h mpc/emp_base_test.cpp)
package_add_test(true_cardinality_test mpc/true_cardinality_test.cpp mpc/emp_base_test.h mpc/emp_base_test.cpp)
package_add_test(expression_cost_model_test mpc/expression_cost_model_test.cpp mpc/emp_base_test.h mpc/emp_base_test.cpp)
package_add_test(operator_cost_model_test mpc/operator_cost_model_test.cpp mpc/emp_base_test.h mpc/emp_base_test.cpp)
package_add_test(plan_cost_test mpc/plan_cost_test.cpp  mpc/emp_base_test.h mpc/emp_base_test.cpp)
package_add_test(fully_optimized_test mpc/fully_optimized_test.cpp  mpc/emp_base_test.h mpc/emp_base_test.cpp)
package_add_test(auto_optimized_test mpc/auto_optimized_test.cpp  mpc/emp_base_test.h mpc/emp_base_test.cpp)
package_add_test(join_cost_comparison_test mpc/join_cost_comparison_test.cpp mpc/emp_base_test.h mpc/emp_base_test.cpp)


#ZK experiments - optional
package_add_test(zk_test zk/zk_test.cpp zk/zk_base_test.cpp)
package_add_test(zk_filter_test zk/zk_filter_test.cpp zk/zk_base_test.cpp)
package_add_test(zk_sort_test zk/zk_sort_test.cpp zk/zk_base_test.cpp)
package_add_test(zk_basic_join_test zk/zk_basic_join_test.cpp zk/zk_base_test.cpp)
package_add_test(zk_keyed_join_test zk/zk_keyed_join.cpp zk/zk_base_test.cpp)
package_add_test(zk_scalar_aggregate_test zk/zk_scalar_aggregate_test.cpp zk/zk_base_test.cpp)
package_add_test(zk_group_by_aggregate_test zk/zk_group_by_aggregate_test.cpp zk/zk_base_test.cpp)
package_add_test(zk_tpch_test zk/zk_tpch_test.cpp zk/zk_base_test.cpp)
