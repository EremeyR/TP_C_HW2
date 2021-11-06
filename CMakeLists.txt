cmake_minimum_required(VERSION 3.16)
project(TP_C_HW2 C)

set(CMAKE_C_STANDARD 11)
set(SOURCE "main.c")

add_library(ranging_lib STATIC static_lib/ranging.c)
target_include_directories(ranging_lib PUBLIC ${PROJECT_SOURCE_DIR})

add_library(d_ranging_lib SHARED dynamic_lib/d_ranging.c)
target_include_directories(d_ranging_lib PUBLIC ${PROJECT_SOURCE_DIR})

add_executable(${PROJECT_NAME} ${SOURCE})
target_link_libraries(${PROJECT_NAME} PRIVATE ranging_lib)
target_link_libraries(${PROJECT_NAME} PRIVATE d_ranging_lib)

