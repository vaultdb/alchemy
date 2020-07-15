#based on emp-tool pipeline
# https://github.com/emp-toolkit/emp-tool
#SET(CMAKE_EXPORT_COMPILE_COMMANDS ON)
cmake_minimum_required (VERSION 3.11)
project (vaultdb-emp)
set(NAME "vaultdb-emp")

#initialize gtest
#execute_process(COMMAND git submodule update --init --recursive WORKING_DIRECTORY ../../..) 
#add_subdirectory("${PROJECT_SOURCE_DIR}/external/googletest" "external/googletest")

include(cmake/common.cmake)
include(GoogleTest)

#link_directories(/usr/local/lib)

find_package(OpenSSL REQUIRED)
find_package(GMP REQUIRED)
find_package(Boost REQUIRED COMPONENTS system)
find_package(EMP REQUIRED)

find_library(EMP_LIBRARIES NAMES emp-tool)
find_library(PROTOBUF_LIBRARY protobuf)
find_library(GRPC++ grpc++)
find_library(GRPC grpc)
find_library(GFLAGS_LIBRARY gflags)
find_library(DL dl)

#depends on pqxx v5 or greater
find_library(PQXX_LIBRARY pqxx)

find_path(EMP-TOOL_INCLUDE_DIR NAMES emp-tool/emp-tool.h)


get_filename_component(vdb_proto "../proto/dbquery.proto" ABSOLUTE)
get_filename_component(vdb_proto_path "${vdb_proto}" PATH)
set(vdb_proto_srcs "rpc/dbquery.pb.cc")
set(vdb_proto_hdrs "rpc/dbquery.pb.h")


execute_process(COMMAND which protoc OUTPUT_VARIABLE PROTOC)
string(REGEX REPLACE "\n$" "" PROTOC "${PROTOC}")  #delete trailing newline
execute_process(COMMAND ${PROTOC} --cpp_out ${CMAKE_CURRENT_BINARY_DIR}/rpc -I${vdb_proto_path} ${vdb_proto})



option(PACKAGE_TESTS "Build the tests" ON)
if(PACKAGE_TESTS)
	enable_testing()
	include(GoogleTest)
	add_subdirectory(test)
endif()

include_directories(${CMAKE_CURRENT_BINARY_DIR}/rpc
		${CMAKE_CURRENT_BINARY_DIR}/data
        ${CMAKE_CURRENT_BINARY_DIR}/operators
        ${CMAKE_CURRENT_BINARY_DIR}/querytable
        ${CMAKE_CURRENT_BINARY_DIR}/common)

include_directories(${OPENSSL_INCLUDE_DIR} ${Boost_INCLUDE_DIRS} ${GMP_INCLUDE_DIR} ${EMP_INCLUDE_DIR} ${GRPC_INCLUDE_DIR})

link_directories(${CMAKE_SOURCE_DIR}/lib)








set(sources
		${vdb_proto_srcs}
		operators/secure_aggregate.cpp
		operators/secure_sort.cpp
		operators/filter.cpp
		operators/secure_join.cpp
		querytable/private_share_utility.cpp
		querytable/query_tuple.cpp
		querytable/types/unencrypted/float_type.cpp
		querytable/types/unencrypted/boolean_type.cpp
		querytable/types/unencrypted/integer_type.cpp
		querytable/types/arithmetic_type.cpp
		querytable/types/bool_result.cpp
		querytable/types/value.cpp
		querytable/types/encrypted/encrypted_boolean_type.cpp
		querytable/types/encrypted/encrypted_integer_type.cpp
		querytable/types/type.cpp
		querytable/table_manager.cpp
		querytable/query_field.cpp
		querytable/dummy_flag.cpp
		querytable/query_field_desc.cpp
		querytable/query_schema.cpp
		querytable/query_table.cpp
		querytable/expression/arithmetic_expression.cpp
		querytable/expression/expression.cpp
		querytable/expression/bool_expression.cpp
		rpc/DBQueryClient.cpp
		data/pqxx_compat.cpp
		data/PQDataProvider.cpp
		data/DataProvider.cpp
		data/proto_converter.cpp
)



add_library(${NAME} SHARED ${sources})
target_link_libraries(${NAME} ${OPENSSL_LIBRARIES} ${Boost_LIBRARIES} ${GMP_LIBRARIES}  ${EMP_LIBRARIES} ${PROTOBUF_LIBRARY} ${PQXX_LIBRARY} ${GFLAGS_LIBRARY} pthread gtest gmock gtest_main )


install(TARGETS ${NAME} DESTINATION ${CMAKE_SOURCE_DIR}/lib)


