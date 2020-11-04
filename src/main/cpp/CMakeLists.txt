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
  add_subdirectory(${googletest_SOURCE_DIR} ${googletest_BINARY_DIR})
endif()


enable_testing()
add_subdirectory(test)


find_package(OpenSSL REQUIRED)
find_package(GMP REQUIRED)
find_package(Boost REQUIRED COMPONENTS system)
find_package(EMP REQUIRED)

find_library(EMP_LIBRARIES NAMES emp-tool)
find_library(GFLAGS_LIBRARY gflags)
find_library(DL dl)

#depends on pqxx v6.2.5
find_library(PQXX_LIBRARY pqxx)

find_path(EMP-TOOL_INCLUDE_DIR NAMES emp-tool/emp-tool.h)




include_directories(${CMAKE_CURRENT_BINARY_DIR}/rpc
		${CMAKE_CURRENT_BINARY_DIR}/data
        ${CMAKE_CURRENT_BINARY_DIR}/operators
        ${CMAKE_CURRENT_BINARY_DIR}/query_table
        ${CMAKE_CURRENT_BINARY_DIR}/common)

include_directories(${OPENSSL_INCLUDE_DIR} ${Boost_INCLUDE_DIRS} ${GMP_INCLUDE_DIR} ${EMP_INCLUDE_DIR} ${GRPC_INCLUDE_DIR})

#link_directories(${CMAKE_SOURCE_DIR}/lib)


set(sources
		operators/secure_aggregate.cpp
        operators/sort.cpp
		operators/filter.cpp
		util/emp_manager.cpp
		query_table/query_tuple.cpp
		query_table/types/value.cpp
		query_table/query_field.cpp
		query_table/query_field_desc.cpp
		query_table/query_schema.cpp
		query_table/query_table.cpp
		data/PsqlDataProvider.cpp
        util/type_utilities.cpp
		util/data_utilities.cpp
		query_table/types/value_comparators.cpp
		query_table/types/value_arithmetic.cpp
		operators/operator.cpp
		operators/sql_input.cpp
        operators/secure_sql_input.cpp
		operators/support/sort_condition.cpp
		operators/support/plain_sort_condition.cpp
		operators/support/secure_sort_condition.cpp
		operators/project.cpp
		operators/support/join_equality_predicate.cpp
		operators/join.cpp
		operators/basic_join.cpp
		operators/common_table_expression_input.cpp
        operators/fkey_pkey_join.cpp
		operators/support/replace_tuple.cpp
		operators/support/secure_replace_tuple.cpp
		data/CsvReader.cpp)



message("verify ${OPENSSL_LIBRARIES}:${Boost_LIBRARIES}:${PQXX_LIBRARY}:${GFLAGS_LIBRARY} ")


add_library(${NAME} SHARED ${sources})
target_link_libraries(${NAME} ${OPENSSL_LIBRARIES} ${Boost_LIBRARIES} ${GMP_LIBRARIES}  ${EMP_LIBRARIES}  ${PQXX_LIBRARY} ${GFLAGS_LIBRARY} pthread gtest gmock gtest_main )


install(TARGETS ${NAME} DESTINATION ${CMAKE_SOURCE_DIR}/lib)


