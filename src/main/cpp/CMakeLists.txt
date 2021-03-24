#based on emp-tool pipeline
# https://github.com/emp-toolkit/emp-tool
cmake_minimum_required (VERSION 3.11)
set(CMAKE_CXX_STANDARD 17)
project (vaultdb-emp)
set(NAME "vaultdb-emp")


include(cmake/common.cmake)



include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        release-1.8.0
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
find_library(GFLAGS_LIBRARY gflags)
find_library(DL dl)

#depends on pqxx v6.2.5
find_library(PQXX_LIBRARY pqxx)
find_package(Boost REQUIRED COMPONENTS date_time system)

find_path(EMP-TOOL_INCLUDE_DIR NAMES emp-tool/emp-tool.h)



include_directories(${CMAKE_CURRENT_BINARY_DIR}/rpc
		${CMAKE_CURRENT_BINARY_DIR}/data
        ${CMAKE_CURRENT_BINARY_DIR}/operators
        ${CMAKE_CURRENT_BINARY_DIR}/query_table
        ${CMAKE_CURRENT_BINARY_DIR}/common)

include_directories(${OPENSSL_INCLUDE_DIR} ${Boost_INCLUDE_DIRS} ${GMP_INCLUDE_DIR} ${EMP_INCLUDE_DIR} ${GRPC_INCLUDE_DIR})




set(sources
		query_table/field/field_type.h
		query_table/field/field.cpp
		query_table/field/bool_field.cpp
		query_table/field/secure_bool_field.cpp
		query_table/field/int_field.cpp
		query_table/field/secure_int_field.cpp
		query_table/field/long_field.cpp
		query_table/field/secure_long_field.cpp
		query_table/field/float_field.cpp
		query_table/field/secure_float_field.cpp
		query_table/field/string_field.cpp
		query_table/field/secure_string_field.cpp

		query_table/field/instance/bool_instance.cpp
		query_table/field/instance/int_instance.cpp
		query_table/field/instance/long_instance.cpp
		query_table/field/instance/float_instance.cpp
		query_table/field/instance/string_instance.cpp

		query_table/field/instance/secure_bool_instance.cpp
		query_table/field/instance/secure_int_instance.cpp
		query_table/field/instance/secure_long_instance.cpp
		query_table/field/instance/secure_float_instance.cpp
		query_table/field/instance/secure_string_instance.cpp

		query_table/field/field_factory.cpp
		util/utilities.cpp
		util/type_utilities.cpp
		util/data_utilities.cpp
		util/field_utilities.cpp

		query_table/query_tuple.cpp
		query_table/query_field_desc.cpp
		query_table/query_schema.cpp
		query_table/query_table.cpp

		data/PsqlDataProvider.cpp
		data/CsvReader.cpp

		operators/operator.cpp
		operators/sql_input.cpp
        operators/secure_sql_input.cpp
		operators/filter.cpp
		operators/project.cpp
		operators/sort.cpp
		operators/support/join_equality_predicate.cpp
		operators/join.cpp
		operators/basic_join.cpp
        operators/fkey_pkey_join.cpp
		operators/support/scalar_aggregate_impl.cpp
		operators/scalar_aggregate.cpp

		operators/group_by_aggregate.cpp
		operators/support/group_by_aggregate_impl.cpp
		operators/csv_input.cpp

		#operators/support/sort_condition.cpp
		#operators/support/plain_sort_condition.cpp
		#operators/support/secure_sort_condition.cpp
		#operators/common_table_expression.cpp
		#operators/support/replace_tuple.cpp
		#operators/support/secure_replace_tuple.cpp
		#operators/support/secure_scalar_aggregate_impl.cpp
		#operators/support/secure_group_by_aggregate_impl.cpp




		)



message("verify ${OPENSSL_LIBRARIES}:${PQXX_LIBRARY}:${GFLAGS_LIBRARY} ")
message("Boost Libs: ${Boost_LIBRARIES} " )	      


add_library(${NAME} SHARED ${sources})

if(APPLE)
  target_compile_definitions(${NAME} PUBLIC "-D_GNU_SOURCE")
endif()

add_executable(generate_enrich_data test/support/generate_enrich_data.cpp)

#shouldn't need this last one, should be in Boost_LIBRARIES var, debug this later
target_link_libraries(${NAME} ${OPENSSL_LIBRARIES} ${Boost_LIBRARIES} ${GMP_LIBRARIES}  ${EMP_LIBRARIES}  ${PQXX_LIBRARY} ${GFLAGS_LIBRARY} pthread gtest gmock gtest_main Boost::date_time )

install(TARGETS ${NAME} DESTINATION ${CMAKE_SOURCE_DIR}/lib)

