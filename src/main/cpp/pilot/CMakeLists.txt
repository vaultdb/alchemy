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



macro(package_add_binary BINNAME)
    # create an exectuable in which the tests will be stored
    add_executable(${BINNAME} ${ARGN})
    # link the Google test infrastructure, mocking library, and a default main fuction to
    # the test executable.  Remove g_test_main if writing your own main function.
    target_link_libraries(${BINNAME}  vaultdb-emp)

    set_target_properties(${TESTNAME} PROPERTIES FOLDER pilot)
endmacro()

package_add_binary(secret_share_csv src/secret_share_csv.cpp src/common/shared_schema.cpp)
package_add_binary(generate_enrich_data_three_parties src/generate_enrich_data_three_parties.cpp)
package_add_binary(load_tuples_data_partner src/load-tuples-data-partner.cpp src/union_hybrid_data.cpp src/common/shared_schema.cpp)
#package_add_binary(run_data_partner src/run-data-partner.cpp src/union_hybrid_data.cpp src/common/shared_schema.cpp src/enrich_htn_query.cpp)

