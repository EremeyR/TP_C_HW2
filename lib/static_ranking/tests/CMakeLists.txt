add_executable(static_ranking_test src/static_ranking_test.cpp)

find_package(GTest)
target_link_libraries(static_ranking_test PRIVATE ranking_lib GTest::Main)

add_test(NAME static_ranking_test COMMAND static_ranking_test)