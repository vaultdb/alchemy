project(capricorn-pilot)
include(../cmake/common.cmake)
cmake_minimum_required (VERSION 3.11)
set(CMAKE_CXX_STANDARD 17)
set(NAME "capricorn-pilot")



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



macro(package_add_binary TESTNAME)
    # create an exectuable in which the tests will be stored
    add_executable(${TESTNAME} ${ARGN})
    # link the Google test infrastructure, mocking library, and a default main fuction to
    # the test executable.  Remove g_test_main if writing your own main function.
    target_link_libraries(${TESTNAME}  vaultdb-emp)

    set_target_properties(${TESTNAME} PROPERTIES FOLDER pilot)
endmacro()

package_add_binary(secret_share_csv src/secret_share_csv.cpp)
package_add_binary(generate_enrich_data_three_parties src/generate_enrich_data_three_parties.cpp)

