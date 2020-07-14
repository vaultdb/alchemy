include_directories(
    ${gtest_SOURCE_DIR}
    ${gtest_SOURCE_DIR}/include
        ${CMAKE_CURRENT_BINARY_DIR}/../rpc
        ${CMAKE_CURRENT_BINARY_DIR}/../data
        ${CMAKE_CURRENT_BINARY_DIR}/../operators
        ${CMAKE_CURRENT_BINARY_DIR}/../querytable
        ${CMAKE_CURRENT_BINARY_DIR}/../common)


include_directories(${OPENSSL_INCLUDE_DIR} ${Boost_INCLUDE_DIRS} ${GMP_INCLUDE_DIR} ${EMP_INCLUDE_DIR})

link_directories(${CMAKE_SOURCE_DIR}/lib)


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


package_add_test(pqxx_compat_test pqxx_compat_test.cpp)
package_add_test(proto_converter_test proto_converter_test.cpp)
package_add_test(secure_sort_test secure_sort_test.cpp)
package_add_test(secure_join_test secure_join_test.cpp)