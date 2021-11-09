add_executable(dynamic_ranking_test src/dynamic_ranking_test.cpp)

find_package(GTest)
target_link_libraries(dynamic_ranking_test PRIVATE d_ranking_lib test_utiles_lib GTest::Main)

add_test(NAME dynamic_ranking_test COMMAND dynamic_ranking_test)