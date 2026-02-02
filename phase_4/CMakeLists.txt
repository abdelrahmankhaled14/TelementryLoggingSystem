cmake_minimum_required(VERSION 3.10)

project(telementry VERSION 1.0 LANGUAGES CXX)

# ============================================================
# C++ standard
# ============================================================
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ============================================================
# Build type selection
# ============================================================
# Usage:
#   cmake -DBUILD_TYPE=MAIN ..
#   cmake -DBUILD_TYPE=TEST_LOGMANAGER ..
#   cmake -DBUILD_TYPE=TEST_LOGMESSAGE ..
set(
    BUILD_TYPE
    "MAIN"
    CACHE STRING
    "Build type: MAIN, TEST_LOGMANAGER, or TEST_LOGMESSAGE"
)

# ============================================================
# Dependencies
# ============================================================
find_package(GTest REQUIRED)
find_package(Threads REQUIRED)

# ============================================================
# Include directories
# ============================================================
include_directories(${PROJECT_SOURCE_DIR}/include)
# ============================================================
# Library sources
# ============================================================
file(GLOB MYLIB_SOURCES
    ${PROJECT_SOURCE_DIR}/src/*.cpp
)

# Exclude main.cpp safely
list(FILTER MYLIB_SOURCES EXCLUDE REGEX ".*/main\\.cpp$")

add_library(mylib STATIC
    ${MYLIB_SOURCES}
)

target_include_directories(mylib PUBLIC
    ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(mylib PUBLIC
    Threads::Threads
)

# ============================================================
# Executables
# ============================================================
if(BUILD_TYPE STREQUAL "MAIN")

    message(STATUS "Building MAIN executable")

    add_executable(program
        src/main.cpp
    )

    target_link_libraries(program
        mylib
    )

elseif(BUILD_TYPE STREQUAL "TEST_LOGMANAGER")

    message(STATUS "Building TEST_LOGMANAGER")

    add_executable(program
        test/logmanagertest.cpp
    )

    target_link_libraries(program
        mylib
        GTest::gtest
        GTest::gmock
        GTest::gtest_main
        GTest::gmock_main
        Threads::Threads
    )

    enable_testing()
    include(GoogleTest)
    gtest_discover_tests(program)

elseif(BUILD_TYPE STREQUAL "TEST_LOGMESSAGE")

    message(STATUS "Building TEST_LOGMESSAGE")

    add_executable(program
        test/logmessagetest.cpp
    )

    target_link_libraries(program
        mylib
        GTest::gtest
        GTest::gmock
        GTest::gtest_main
        GTest::gmock_main
        Threads::Threads
    )

    enable_testing()
    include(GoogleTest)
    gtest_discover_tests(program)

else()

    message(FATAL_ERROR
        "Invalid BUILD_TYPE: ${BUILD_TYPE}. "
        "Must be MAIN, TEST_LOGMANAGER, or TEST_LOGMESSAGE"
    )

endif()
