#based on emp-tool pipeline
# https://github.com/emp-toolkit/emp-tool
#SET(CMAKE_EXPORT_COMPILE_COMMANDS ON)
cmake_minimum_required (VERSION 2.8.11)
project (vaultdb-emp)
set(NAME "vaultdb-emp")



include(cmake/common.cmake)

find_package(OpenSSL REQUIRED)
find_package(GMP REQUIRED)
find_package(Boost REQUIRED COMPONENTS system)
find_package(Emp REQUIRED)
find_library(EMP_LIBRARIES NAMES emp-tool)
find_library(PROTOBUF protobuf)
find_library(GRPC++ grpc++)
find_library(GRPC grpc)
find_library(GFLAGS gflags)
find_library(DL dl)
find_path(EMP-TOOL_INCLUDE_DIR NAMES emp-tool/emp-tool.h)

include_directories(${CMAKE_CURRENT_BINARY_DIR}/rpc)
include_directories(${OPENSSL_INCLUDE_DIR} ${Boost_INCLUDE_DIRS} ${GMP_INCLUDE_DIR} ${EMP_INCLUDE_DIR})


get_filename_component(vdb_proto "../proto/dbquery.proto" ABSOLUTE)
get_filename_component(vdb_proto_path "${vdb_proto}" PATH)
set(vdb_proto_srcs "rpc/dbquery.pb.cc")
set(vdb_proto_hdrs "rpc/dbquery.pb.h")
set(vdb_grpc_srcs "rpc/dbquery.grpc.pb.cc")
set(vdb_grpc_hdrs "rpc/dbquery.grpc.pb.h")


add_custom_command(
		OUTPUT "${vdb_proto_srcs}" "${vdb_proto_hdrs}"
		COMMAND ${PROTOC}
		ARGS
		--cpp_out "${CMAKE_CURRENT_BINARY_DIR}/rpc"
		-I "${vdb_proto_path}"
		"${vdb_proto}"
		DEPENDS "${vdb_proto}")

#add_custom_target(${vdb_proto} ALL DEPENDS )
add_custom_target(rpc_proto_gen ALL DEPENDS ${vdb_proto} ${vdb_proto_srcs} ${vdb_proto_headers})


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



message("verify ${OPENSSL_LIBRARIES}: ${Boost_LIBRARIES}")
add_library(${NAME} SHARED ${sources})
target_link_libraries(${NAME} ${OPENSSL_LIBRARIES} ${Boost_LIBRARIES} ${GMP_LIBRARIES}  ${EMP_LIBRARIES} ${PROTOBUF_LIBRARIES} pthread)


install(TARGETS ${NAME} DESTINATION ${CMAKE_SOURCE_DIR}/lib)


# Test cases
macro (add_test _name)
	add_test_with_lib(${_name} "${NAME}")
endmacro()


add_test(secure_sort_test)
add_test(secure_join_test)
add_test(pqxx_compat_test)
add_test(proto_converter_test)

