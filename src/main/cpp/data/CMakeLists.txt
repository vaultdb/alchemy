add_library(pqxx_compat
        pqxx_compat.cpp
        pqxx_compat.h
        proto_converter.cpp
        proto_converter.h
        )
target_link_libraries(pqxx_compat krpc pqxx pq)

add_library(dataprovider DataProvider.cpp PQDataProvider.cpp ../querytable/query_table.cpp ../querytable/query_table.h)
target_link_libraries(dataprovider pqxx_compat)

add_executable(pqxx_compat_test pqxx_compat_test.cpp)
target_link_libraries(pqxx_compat_test pqxx_compat gtest_main profiler tcmalloc)
add_test(NAME pqxx_compat COMMAND pqxx_compat_test)

add_executable(proto_converter_test proto_converter_test.cpp)
target_link_libraries(proto_converter_test pqxx_compat gtest_main profiler tcmalloc emp-tool query_table)
add_test(NAME proto_converter COMMAND proto_converter_test)
