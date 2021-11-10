cmake_minimum_required(VERSION 3.16)
project(Ranking)

set(CMAKE_C_STANDARD 11)

option(WITH_COVERAGE "check coverage" ON)
if (WITH_COVERAGE)
    add_compile_options(--coverage)
    add_link_options(--coverage)
    set(BUILD_TEST ON)
endif ()

option(WITH_SANITIZER "add sanitizer")
if (WITH_SANITIZER)
    add_compile_options(-fsanitize=address)
    add_link_options(-fsanitize=address)
    set(BUILD_TEST ON)
endif ()

option(WITH_TESTS "build tests" ON)
if (WITH_TESTS)
    set(BUILD_TEST ON)
endif ()

if (BUILD_TEST)
    enable_testing()
endif ()

add_subdirectory(lib/static_ranking)
add_subdirectory(lib/dynamic_ranking)
add_subdirectory(lib/test_utiles)

add_executable(s_${PROJECT_NAME} src/main.c)
target_link_libraries(s_${PROJECT_NAME} PRIVATE ranking_lib)
target_include_directories(s_${PROJECT_NAME} PUBLIC lib/static_ranking/include)
target_compile_options(s_${PROJECT_NAME} PUBLIC -Wall -Werror -Wpedantic)
target_link_options(s_${PROJECT_NAME} PUBLIC -Wall -Werror -Wpedantic)



add_executable(d_${PROJECT_NAME} src/d_main.c)
target_link_libraries(d_${PROJECT_NAME} PRIVATE d_ranking_lib)
target_include_directories(d_${PROJECT_NAME} PUBLIC lib/dynamic_ranking/include)
target_compile_options(d_${PROJECT_NAME} PUBLIC -Wall -Werror -Wpedantic)
target_link_options(d_${PROJECT_NAME} PUBLIC -Wall -Werror -Wpedantic)
