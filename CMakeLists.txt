cmake_minimum_required(VERSION 3.10)
project(Lab_7)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pthread")

include(FetchContent)

FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.15.0
    TLS_VERIFY false
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

set(SOURCES
    src/npc.cpp
    src/dragon.cpp
    src/elf.cpp
    src/druid.cpp
    src/factory.cpp
    src/arena.cpp
    src/combat_visitor.cpp
)

add_library(${PROJECT_NAME}_lib ${SOURCES})
target_include_directories(${PROJECT_NAME}_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE ${PROJECT_NAME}_lib)

enable_testing()

# тесты для боевой системы
add_executable(${PROJECT_NAME}_test_battle tests/test_battle.cpp)
target_link_libraries(${PROJECT_NAME}_test_battle 
    PRIVATE 
    ${PROJECT_NAME}_lib 
    gtest_main
)
target_include_directories(${PROJECT_NAME}_test_battle 
    PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/tests
)
add_test(NAME ${PROJECT_NAME}_test_battle COMMAND ${PROJECT_NAME}_test_battle)

# тесты для многопоточности
add_executable(${PROJECT_NAME}_test_threads tests/test_threads.cpp)
target_link_libraries(${PROJECT_NAME}_test_threads 
    PRIVATE 
    ${PROJECT_NAME}_lib 
    gtest_main
    pthread
)
target_include_directories(${PROJECT_NAME}_test_threads 
    PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/tests
)
add_test(NAME ${PROJECT_NAME}_test_threads COMMAND ${PROJECT_NAME}_test_threads)
