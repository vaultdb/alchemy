#based on emp-tool pipeline
# https://github.com/emp-toolkit/emp-tool
cmake_minimum_required (VERSION 3.14)
set(CMAKE_CXX_STANDARD 17)
project (vaultdb-emp)
set(NAME "vaultdb-emp")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

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

add_subdirectory(test)
add_subdirectory(pilot)


find_package(OpenSSL REQUIRED)
find_package(GMP REQUIRED)
find_package(EMP REQUIRED)

find_library(EMP_LIBRARIES NAMES emp-tool)
find_library(EMP_ZK_LIBRARY NAMES emp-zk)

find_library(GFLAGS_LIBRARY gflags)
find_library(DL dl)

#depends on pqxx v6.2.5
find_library(PQXX_LIBRARY pqxx)
find_package(Boost REQUIRED COMPONENTS date_time system program_options)

find_path(EMP-TOOL_INCLUDE_DIR NAMES emp-tool/emp-tool.h)



include_directories(${OPENSSL_INCLUDE_DIR} ${Boost_INCLUDE_DIRS} ${GMP_INCLUDE_DIR} ${EMP_INCLUDE_DIR})




set(sources
		util/utilities.cpp
		util/type_utilities.cpp
		util/data_utilities.cpp
		util/field_utilities.cpp
		util/logger.cpp
		util/system_configuration.cpp

		query_table/field/field_type.h
		query_table/field/field.cpp
		query_table/field/field_factory.cpp
		query_table/query_tuple.cpp
		query_table/plain_tuple.cpp
		query_table/secure_tuple.cpp
		query_table/query_field_desc.cpp
		query_table/query_schema.cpp
		query_table/query_table.cpp

        data/psql_data_provider.cpp
		data/csv_reader.cpp

		operators/operator.cpp
		operators/sql_input.cpp
        operators/secure_sql_input.cpp
		operators/zk_sql_input.cpp
		operators/filter.cpp
		operators/project.cpp
		operators/sort.cpp
		operators/support/join_equality_predicate.cpp
		operators/join.cpp
		operators/basic_join.cpp
		operators/keyed_join.cpp
		operators/support/scalar_aggregate_impl.cpp
		operators/scalar_aggregate.cpp
		operators/union.cpp
		operators/group_by_aggregate.cpp
		operators/support/group_by_aggregate_impl.cpp
		operators/csv_input.cpp
        operators/shrinkwrap.cpp

		expression/function_expression.cpp
		expression/generic_expression.cpp
        expression/expression_node.cpp
		expression/case_node.h
		expression/comparator_expression_nodes.cpp
		expression/connector_expression_nodes.h
		expression/math_expression_nodes.h

        parser/plan_parser.cpp
		parser/expression_parser.cpp
		expression/expression_kind.h
		expression/expression_factory.cpp
		expression/visitor/expression_visitor.h
		expression/visitor/join_equality_condition_visitor.cpp
		expression/visitor/print_expression_visitor.cpp
		expression/visitor/type_inference_visitor.cpp
		expression/visitor/plain_to_secure_visitor.cpp
		expression/visitor/type_validation_visitor.cpp

		)



#message("verify ${OPENSSL_LIBRARIES}:${PQXX_LIBRARY}:${GFLAGS_LIBRARY}: ${EMP_ZK_LIBRARY}  ${EMP_LIBRARIES} ${EMP-TOOL_INCLUDE_DIR}")

add_library(${NAME} SHARED ${sources})

if(APPLE)
  target_compile_definitions(${NAME} PUBLIC "-D_GNU_SOURCE")
endif()

add_executable(generate_enrich_data test/support/generate_enrich_data.cpp)

#shouldn't need this last one, should be in Boost_LIBRARIES var, debug this later
target_link_libraries(${NAME} ${OPENSSL_LIBRARIES} ${Boost_LIBRARIES} ${GMP_LIBRARIES}  ${EMP_LIBRARIES}  ${EMP_ZK_LIBRARY}  ${PQXX_LIBRARY} ${GFLAGS_LIBRARY} pthread gtest gmock gtest_main Boost::date_time )

install(TARGETS ${NAME} DESTINATION ${CMAKE_SOURCE_DIR}/lib)

