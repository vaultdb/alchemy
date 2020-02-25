add_library(secure_operators STATIC secure_join.cpp filter.cpp secure_aggregate.cpp secure_aggregate.h)
target_link_libraries(secure_operators krpc query_table types)

#add_executable(secure_join_test secure_join_test.cpp)
#target_link_libraries(secure_join_test pqxx_compat gtest_main profiler tcmalloc secure_operators)
#add_test(NAME secure_join_test COMMAND secure_join_test)
