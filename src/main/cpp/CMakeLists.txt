#based on emp-tool pipeline
# https://github.com/emp-toolkit/emp-tool
cmake_minimum_required (VERSION 3.14)
set(CMAKE_CXX_STANDARD 17)
project (vaultdb-emp)
set(NAME "vaultdb-emp")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(cmake/common.cmake)

include(FetchContent)
FetchContent_Declare(
		googletest
		GIT_REPOSITORY https://github.com/google/googletest.git
		GIT_TAG        v1.13.0
)

FetchContent_GetProperties(googletest)
if(NOT googletest_POPULATED)
	FetchContent_Populate(googletest)
	add_subdirectory(${googletest_SOURCE_DIR} ${googletest_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()

enable_testing()

find_package(OpenSSL REQUIRED)
#find_package(GMP REQUIRED)
find_package(EMP REQUIRED)
#find_package(Gperftools REQUIRED)

find_library(EMP_LIBRARIES NAMES emp-tool)
find_library(EMP_ZK_LIBRARY NAMES emp-zk)

find_library(GFLAGS_LIBRARY gflags)
find_library(DL dl)

#depends on pqxx v6.2.5
find_library(PQXX_LIBRARY pqxx)
find_package(Boost REQUIRED COMPONENTS date_time system program_options)

find_path(EMP-TOOL_INCLUDE_DIR NAMES emp-tool/emp-tool.h)

include_directories(${OPENSSL_INCLUDE_DIR} ${Boost_INCLUDE_DIRS} ${GMP_INCLUDE_DIR} ${EMP_INCLUDE_DIR})


add_subdirectory(test)
if(EXISTS "/usr/local/include/emp-sh2pc/emp-sh2pc.h")
	add_subdirectory(pilot)
endif()

set(sources
		query_table/field/field_type.h
		query_table/field/field.cpp
		query_table/field/field_factory.cpp
		query_table/query_tuple.cpp
		query_table/plain_tuple.cpp
		query_table/secure_tuple.cpp
		query_table/query_field_desc.cpp
		query_table/query_schema.cpp
		query_table/query_table.cpp
		query_table/column_table.cpp
		query_table/buffered_column_table.cpp
		query_table/packed_column_table.cpp
		query_table/input_party_packed_column_table.h
		query_table/computing_party_packed_column_table.h
		query_table/compression/compressed_table.cpp
		query_table/compression/column_encoding.cpp
		query_table/compression/uncompressed.cpp
		query_table/compression/bit_packed_encoding.cpp
		query_table/compression/plain_dictionary_encoding.cpp

		util/utilities.cpp
		util/type_utilities.cpp
		util/data_utilities.cpp
		util/field_utilities.cpp
		util/operator_utilities.cpp
		util/table_manager.cpp
		util/logger.cpp
		util/system_configuration.cpp
		util/buffer_pool/buffer_pool_manager.cpp
		util/emp_manager/emp_manager.h
		util/emp_manager/sh2pc_manager.cpp
		util/emp_manager/zk_manager.cpp
		util/emp_manager/outsourced_mpc_manager.cpp
		util/emp_manager/sh2pc_outsourced_manager.cpp

		data/psql_data_provider.cpp
		data/csv_reader.cpp

		operators/operator.cpp
		operators/sql_input.cpp
		operators/secure_sql_input.cpp
		operators/table_scan.cpp
		operators/packed_table_scan.h
		operators/zk_sql_input.cpp
		operators/filter.cpp
		operators/project.cpp
		operators/sort.cpp
		operators/join.cpp
		operators/basic_join.cpp
		operators/keyed_join.cpp
		operators/left_keyed_join.cpp
		operators/block_nested_loop_join.cpp
		operators/merge_input.cpp
		operators/merge_join.cpp
		operators/support/scalar_aggregate_impl.cpp
		operators/scalar_aggregate.cpp
		operators/union.cpp
		operators/stored_table_scan.cpp
		operators/sort_merge_aggregate.cpp
		operators/support/group_by_aggregate_impl.h
		operators/csv_input.cpp
		operators/shrinkwrap.cpp
		operators/sort_merge_join.cpp
		operators/keyed_sort_merge_join.cpp
		operators/nested_loop_aggregate.cpp
		operators/support/unsorted_aggregate_impl.cpp
		operators/support/normalize_fields.h

		expression/function_expression.cpp
		expression/generic_expression.cpp
		expression/expression_node.cpp
		expression/case_node.h
		expression/comparator_expression_nodes.cpp
		expression/connector_expression_nodes.h
		expression/math_expression_nodes.h

		parser/plan_parser.cpp
		parser/expression_parser.cpp
		parser/plan_deparser.cpp
		parser/expression_deparser.cpp

		expression/expression_kind.h
		expression/expression_factory.cpp
		expression/visitor/expression_visitor.h
		expression/visitor/join_equality_condition_visitor.cpp
		expression/visitor/print_expression_visitor.cpp
		expression/visitor/type_inference_visitor.cpp
		expression/visitor/plain_to_secure_visitor.cpp
		expression/visitor/type_validation_visitor.cpp
		expression/visitor/to_packed_expression_visitor.cpp

		opt/plan_cost.h
		opt/operator_cost_model.cpp
		opt/expression_cost_model.cpp
		opt/plan_optimizer.cpp
		opt/join_graph.h

		enumerator/bushy_plan_enumerator.cpp
)

# #########################################
# "make check-lint"
# #########################################
file(GLOB_RECURSE VAULTDB_LINT_FILES ${sources}
		"${CMAKE_CURRENT_SOURCE_DIR}/operators/*.h"
		"${CMAKE_CURRENT_SOURCE_DIR}/operators/*.cpp"
		"${CMAKE_CURRENT_SOURCE_DIR}/data/*.h"
		"${CMAKE_CURRENT_SOURCE_DIR}/data/*.cpp"
		"${CMAKE_CURRENT_SOURCE_DIR}/common/*.h"
		"${CMAKE_CURRENT_SOURCE_DIR}/expression/*.h"
		"${CMAKE_CURRENT_SOURCE_DIR}/expression/*.cpp"
		"${CMAKE_CURRENT_SOURCE_DIR}/opt/*.h"
		"${CMAKE_CURRENT_SOURCE_DIR}/opt/*.cpp"
		"${CMAKE_CURRENT_SOURCE_DIR}/parser/*.h"
		"${CMAKE_CURRENT_SOURCE_DIR}/parser/*.cpp"
		"${CMAKE_CURRENT_SOURCE_DIR}/query_table/*.h"
		"${CMAKE_CURRENT_SOURCE_DIR}/query_table/*.cpp"
		"${CMAKE_CURRENT_SOURCE_DIR}/util/*.h"
		"${CMAKE_CURRENT_SOURCE_DIR}/util/*.cpp"
)

#based on https://github.com/cmu-db/bustub/blob/master/CMakeLists.txt
# Balancing act: cpplint.py takes a non-trivial time to launch,
# so process 8 files per invocation, while still ensuring parallelism
# TODO: gradually pare down the # of filters we use here as we fix up the code, starting with whitespace
add_custom_target(check-lint echo '${VAULTDB_LINT_FILES}' | xargs -n8 -P8
		cmake/cpplint.py
		--verbose=2 --quiet
		--filter=-legal/copyright,-build/header_guard,-runtime/references,-whitespace,-build/include_order,-build/namespaces,-build/c++11 # https://github.com/cpplint/cpplint/issues/148
)

#message("verify ${OPENSSL_LIBRARIES}:${PQXX_LIBRARY}:${EMP_ZK_LIBRARY}  ${EMP_LIBRARIES} ${EMP_INCLUDE_DIR}")

add_library(${NAME} SHARED ${sources})

if(APPLE)
	target_compile_definitions(${NAME} PUBLIC "-D_GNU_SOURCE")
endif()

add_executable(generate_enrich_data test/support/generate_enrich_data.cpp)

#shouldn't need this last one, should be in Boost_LIBRARIES var, debug this later
target_link_libraries(${NAME} ${OPENSSL_LIBRARIES}  ${EMP_LIBRARIES}  ${EMP_ZK_LIBRARY}  ${PQXX_LIBRARY}  ${GFLAGS_LIBRARY}   ${Boost_LIBRARIES} Boost::date_time )
#add_link_options("SHELL:-Wl,-no-as-needed -lprofiler -Wl,-as-needed")

install(TARGETS ${NAME} DESTINATION ${CMAKE_SOURCE_DIR}/lib)


add_executable(run_query util/cli/run_query.cpp)
target_link_libraries(run_query vaultdb-emp)


add_executable(reveal_xor_shared_table util/cli/reveal_xor_shared_table.cpp)
target_link_libraries(reveal_xor_shared_table vaultdb-emp)

add_executable(parse_plan util/cli/parse_plan.cpp)
target_link_libraries(parse_plan vaultdb-emp)

add_executable(secret_share_tpch_instance util/cli/secret_share_tpch_instance.cpp)
target_link_libraries(secret_share_tpch_instance vaultdb-emp)

add_executable(xor_secret_share_tpch_instance util/cli/xor_secret_share_tpch_instance.cpp)
target_link_libraries(xor_secret_share_tpch_instance vaultdb-emp)

add_executable(secret_share_mpc_table_from_query util/cli/secret_share_mpc_table_from_query.cpp)
target_link_libraries(secret_share_mpc_table_from_query vaultdb-emp)

add_executable(wire_pack_tpch_instance util/cli/wire_pack_tpch_instance.cpp)
target_link_libraries(wire_pack_tpch_instance vaultdb-emp)

add_executable(wire_pack_table_from_query util/cli/wire_pack_table_from_query.cpp)
target_link_libraries(wire_pack_table_from_query vaultdb-emp)
