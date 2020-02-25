add_library(query_table SHARED
        query_field_desc.cpp
        query_schema.cpp
        query_field.cpp
        query_tuple.cpp
        query_table.cpp
        dummy_flag.cpp
        dummy_flag.h
        table_manager.cpp
        table_manager.h
        private_share_utility.cpp)
target_link_libraries(query_table types gmp protobuf)
add_subdirectory(types)
install(TARGETS query_table DESTINATION ${CMAKE_SOURCE_DIR}/lib/lib)

