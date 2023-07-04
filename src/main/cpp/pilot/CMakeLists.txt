project(capricorn-pilot)
include(../cmake/common.cmake)
cmake_minimum_required (VERSION 3.11)
set(CMAKE_CXX_STANDARD 17)
set(NAME "capricorn-pilot")




include_directories(${OPENSSL_INCLUDE_DIR} ${Boost_INCLUDE_DIRS} ${GMP_INCLUDE_DIR} ${EMP_INCLUDE_DIR})



macro(package_add_binary BINNAME)
    # create an exectuable in which the tests will be stored
    add_executable(${BINNAME} ${ARGN})
    # link the Google test infrastructure, mocking library, and a default main fuction to
    # the test executable.  Remove g_test_main if writing your own main function.
    target_link_libraries(${BINNAME}  vaultdb-emp)

    set_target_properties(${TESTNAME} PROPERTIES FOLDER pilot)
endmacro()

package_add_binary(secret_share_csv src/secret_share_csv.cpp src/common/shared_schema.cpp src/common/pilot_utilities.cpp)
package_add_binary(secret_share_from_query src/secret_share_from_query.cpp  src/common/pilot_utilities.cpp)
package_add_binary(secret_share_batch_from_query src/secret_share_batch_from_query.cpp  src/common/pilot_utilities.cpp)

package_add_binary(generate_enrich_data_three_parties src/generate_enrich_data_three_parties.cpp)
package_add_binary(load_tuples_data_partner src/load_tuples_data_partner.cpp src/common/union_hybrid_data.cpp src/common/shared_schema.cpp src/common/pilot_utilities.cpp)
package_add_binary(run_data_partner src/run_data_partner.cpp src/common/union_hybrid_data.cpp src/common/shared_schema.cpp src/enrich_htn_query.cpp src/common/pilot_utilities.cpp src/common/shared_schema.cpp)
package_add_binary(run_data_partner_aggregation src/run_data_partner_aggregation.cpp src/common/shared_schema.cpp src/common/pilot_utilities.cpp src/common/union_hybrid_data.cpp)
package_add_binary(run_data_partner_batch src/run_data_partner_batch.cpp src/common/union_hybrid_data.cpp src/common/shared_schema.cpp src/enrich_htn_query.cpp src/common/shared_schema.cpp  src/common/pilot_utilities.cpp)
package_add_binary(assemble_secret_shares src/assemble_secret_shares.cpp src/common/shared_schema.cpp)

