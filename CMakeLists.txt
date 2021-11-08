cmake_minimum_required(VERSION 3.16)
project(TP_C_HW2)

set(CMAKE_C_STANDARD 11)
set(SOURCE "src/main.c")

option(WITH_COVERAGE "check coverage")
if (WITH_COVERAGE)
    add_compile_options(--coverage)
    add_link_options(--coverage)
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

add_executable(${PROJECT_NAME} ${SOURCE})
target_link_libraries(${PROJECT_NAME} PRIVATE ranking_lib)
target_link_libraries(${PROJECT_NAME} PRIVATE d_ranking_lib)

target_include_directories(${PROJECT_NAME} PUBLIC lib/static_ranking)
target_include_directories(${PROJECT_NAME} PUBLIC lib/dynamic_ranking)

