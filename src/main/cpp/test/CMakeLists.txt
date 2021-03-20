include(../cmake/common.cmake)
include(GoogleTest)
cmake_minimum_required (VERSION 3.11)



include_directories(
    ${gtest_SOURCE_DIR}
    ${gtest_SOURCE_DIR}/include
        ${CMAKE_CURRENT_BINARY_DIR}/../data
        ${CMAKE_CURRENT_BINARY_DIR}/../operators
        ${CMAKE_CURRENT_BINARY_DIR}/../querytable
        ${CMAKE_CURRENT_BINARY_DIR}/../common
		${CMAKE_CURRENT_BINARY_DIR}/support
)


include_directories(${OPENSSL_INCLUDE_DIR} ${Boost_INCLUDE_DIRS} ${GMP_INCLUDE_DIR} ${EMP_INCLUDE_DIR})



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
            )
    set_target_properties(${TESTNAME} PROPERTIES FOLDER test)
endmacro()


#package_add_test(tpch_q1_test tpch_q1_test.cpp)
#non-private tests
package_add_test(field_instance_test field_instance_test.cpp)
package_add_test(value_expression_test value_expression_test.cpp)
package_add_test(csv_reader_test csv_reader_test.cpp)
package_add_test(filter_test filter_test.cpp)
package_add_test(project_test project_test.cpp)
package_add_test(sort_test sort_test.cpp)
package_add_test(basic_join_test basic_join_test.cpp)
package_add_test(fkey_pkey_join_test fkey_pkey_join_test.cpp)

#Tranche 2
#package_add_test(scalar_aggregate_test scalar_aggregate_test.cpp)
#package_add_test(group_by_aggregate_test group_by_aggregate_test.cpp)


#EMP-based tests
#Tranche 1
package_add_test(emp_test emp_test.cpp support/EmpBaseTest.cpp)
package_add_test(emp_table_test  emp_table_test.cpp support/EmpBaseTest.cpp support/QueryTableTestQueries.cpp) # port 54324-54325
package_add_test(emp_test_multiple_starts emp_test_multiple_starts.cpp)
package_add_test(secure_value_expression_test secure_value_expression_test.cpp support/EmpBaseTest.cpp  )
package_add_test(secure_filter_test secure_filter_test.cpp support/EmpBaseTest.cpp )
package_add_test(secure_sort_test secure_sort_test.cpp support/EmpBaseTest.cpp )
#package_add_test(secure_basic_join_test secure_basic_join_test.cpp support/EmpBaseTest.cpp )
#package_add_test(secure_pkey_fkey_join_test secure_pkey_fkey_join_test.cpp support/EmpBaseTest.cpp )

#Tranche 2
#package_add_test(secure_scalar_aggregate_test secure_scalar_aggregate_test.cpp support/EmpBaseTest.cpp)
#package_add_test(secure_group_by_aggregate_test secure_group_by_aggregate_test.cpp support/EmpBaseTest.cpp)
#package_add_test(emp_float_to_int emp_float_to_int.cpp support/EmpBaseTest.cpp)
#package_add_test(enrich_test enrich_test.cpp  support/enrich_test_support.cpp support/EmpBaseTest.cpp)
#package_add_test(secret_share_generator_test secret_share_generator_test.cpp)
#package_add_test(serialization_test serialization_test.cpp)
#package_add_test(secure_serialization_deserializaton_test secure_serialization_deserializaton_test.cpp support/EmpBaseTest.cpp)
