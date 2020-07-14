link_directories(/usr/local/lib)
set(CMAKE_BUILD_TYPE Debug)
LIST(APPEND CMAKE_PROGRAM_PATH ${CMAKE_SOURCE_DIR}/lib/bin)
find_program(PROTOC protoc ${CMAKE_SOURCE_DIR}/lib/bin)
find_program(GRPC_PROTO_PLUGIN grpc_cpp_plugin ${CMAKE_SOURCE_DIR}/lib/bin)

find_library(GRPC++ grpc++)
find_library(GRPC grpc)
find_library(GFLAGS gflags)
find_library(DL dl)
find_library(PROTOBUF protobuf)



get_filename_component(vdb_proto "../../proto/dbquery.proto" ABSOLUTE)
get_filename_component(vdb_proto_path "${vdb_proto}" PATH)

set(vdb_proto_srcs "${CMAKE_CURRENT_BINARY_DIR}/dbquery.pb.cc")
set(vdb_proto_hdrs "${CMAKE_CURRENT_BINARY_DIR}/dbquery.pb.h")
set(vdb_grpc_srcs "${CMAKE_CURRENT_BINARY_DIR}/dbquery.grpc.pb.cc")
set(vdb_grpc_hdrs "${CMAKE_CURRENT_BINARY_DIR}/dbquery.grpc.pb.h")



add_custom_command(
		OUTPUT "${vdb_proto_srcs}" "${vdb_proto_hdrs}" #"${vdb_grpc_srcs}" "${vdb_grpc_hdrs}"
		COMMAND ${PROTOC}
		ARGS
		--cpp_out "${CMAKE_CURRENT_BINARY_DIR}"
		-I "${vdb_proto_path}"
		"${vdb_proto}"
		DEPENDS "${vdb_proto}")
add_custom_target(rpc_proto_gen ALL DEPENDS ${vdb_proto} ${vdb_proto_srcs} ${vdb_proto_headers}) 


add_library(krpc STATIC ${vdb_proto_srcs})

target_link_libraries(krpc protobuf pthread)
install(TARGETS krpc DESTINATION ${CMAKE_SOURCE_DIR}/lib/lib)
