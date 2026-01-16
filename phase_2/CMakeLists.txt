cmake_minimum_required(VERSION 3.10)
project(telementry VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Define build type variable (set via command line: cmake -DBUILD_TYPE=MAIN)
# Valid options: MAIN, TEST_LOGMANAGER, TEST_LOGMESSAGE
set(BUILD_TYPE "MAIN" CACHE STRING "Build type: MAIN, TEST_LOGMANAGER, or TEST_LOGMESSAGE")

# Find packages
find_package(GTest REQUIRED)
find_package(Threads REQUIRED)

# Include directories
include_directories(${PROJECT_SOURCE_DIR}/include)

# Static library with correct lowercase filenames
add_library(mylib STATIC
    src/logmessage.cpp
    src/filesink.cpp
    src/consolesink.cpp
    src/logmanager.cpp
    src/FileTelemetrySourceImpl.cpp
    src/safefile.cpp
)

target_include_directories(mylib PUBLIC
    ${PROJECT_SOURCE_DIR}/include
)

# Conditional executable creation based on BUILD_TYPE
if(BUILD_TYPE STREQUAL "MAIN")
    message(STATUS "Building main program")
    add_executable(program src/main.cpp)
    target_link_libraries(program mylib)

elseif(BUILD_TYPE STREQUAL "TEST_LOGMANAGER")
    message(STATUS "Building logmanager tests")
    add_executable(program test/logmanagertest.cpp)
    target_link_libraries(program
        mylib
        GTest::gtest_main
        GTest::gmock_main
        GTest::gmock
        GTest::gtest
        Threads::Threads
    )
    
    enable_testing()
    include(GoogleTest)
    gtest_discover_tests(program)

elseif(BUILD_TYPE STREQUAL "TEST_LOGMESSAGE")
    message(STATUS "Building logmessage tests")
    add_executable(program test/logmessagetest.cpp)
    target_link_libraries(program
        mylib
        GTest::gtest_main
        GTest::gmock_main
        GTest::gmock
        GTest::gtest
        Threads::Threads
    )
    
    enable_testing()
    include(GoogleTest)
    gtest_discover_tests(program)

else()
    message(FATAL_ERROR "Invalid BUILD_TYPE: ${BUILD_TYPE}. Must be MAIN, TEST_LOGMANAGER, or TEST_LOGMESSAGE")
endif()
