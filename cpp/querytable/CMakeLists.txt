add_library(query_table
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
target_link_libraries(query_table types gmp)
add_subdirectory(types)

