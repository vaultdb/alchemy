
include(../cmake/common.cmake)
include(GoogleTest)
cmake_minimum_required (VERSION 3.11)

#include(FetchContent)
#FetchContent_Declare(
#  googletest
#  GIT_REPOSITORY https://github.com/google/googletest.git
#  GIT_TAG        release-1.8.0
#)

#FetchContent_GetProperties(googletest)
#if(NOT googletest_POPULATED)
#  FetchContent_Populate(googletest)
#  add_subdirectory(${googletest_SOURCE_DIR} ${googletest_BINARY_DIR})
#endif()



include_directories(
    ${gtest_SOURCE_DIR}
    ${gtest_SOURCE_DIR}/include
        ${CMAKE_CURRENT_BINARY_DIR}/../rpc
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
    set_target_properties(${TESTNAME} PROPERTIES FOLDER tests)
endmacro()


#package_add_test(tpch_q1_test tpch_q1_test.cpp)
#non-private tests
package_add_test(query_table_test query_table_test.cpp)
package_add_test(value_expression_test value_expression_test.cpp)
package_add_test(filter_test filter_test.cpp)
package_add_test(sort_test sort_test.cpp)
package_add_test(project_test project_test.cpp)
package_add_test(basic_join_test basic_join_test.cpp)
package_add_test(fkey_pkey_join_test fkey_pkey_join_test.cpp)

#EMP-based tests
package_add_test(emp_test emp_test.cpp support/EmpBaseTest.cpp)  #port 54321-54322
package_add_test(emp_basic_table_test emp_basic_table_test.cpp support/EmpBaseTest.cpp  ) #port 54323
package_add_test(emp_table_test  emp_table_test.cpp support/EmpBaseTest.cpp ) # port 54324-54325
package_add_test(emp_table_types_test  emp_table_types_test.cpp support/EmpBaseTest.cpp support/QueryTableTestQueries.cpp) # port 54326-54327
#package_add_test(secure_value_expression_test secure_value_expression_test.cpp)
#package_add_test(secure_filter_test secure_filter_test.cpp)
#package_add_test(secure_sort_test secure_sort_test.cpp)
#package_add_test(secure_basic_join_test secure_basic_join_test.cpp)
