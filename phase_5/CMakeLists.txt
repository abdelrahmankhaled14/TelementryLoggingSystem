# cmake_minimum_required(VERSION 3.10)
# project(telementry VERSION 1.0 LANGUAGES CXX)

# # ============================================================
# # C++ standard
# # ============================================================
# set(CMAKE_CXX_STANDARD 17)
# set(CMAKE_CXX_STANDARD_REQUIRED ON)

# # ============================================================
# # Build type selection
# # ============================================================
# set(
# BUILD_TYPE
# "MAIN"
# CACHE STRING
# "Build type: MAIN, TEST_LOGMANAGER, or TEST_LOGMESSAGE"
# )

# # ============================================================
# # Dependencies
# # ============================================================
# find_package(GTest REQUIRED)
# find_package(Threads REQUIRED)
# find_package(CommonAPI REQUIRED)
# find_package(CommonAPI-SomeIP REQUIRED)

# # ============================================================
# # Include directories
# # ============================================================
# include_directories(${PROJECT_SOURCE_DIR}/include)
# include_directories(${PROJECT_SOURCE_DIR}/src-gen)

# # ============================================================
# # Library sources
# # ============================================================
# file(GLOB MYLIB_SOURCES
# ${PROJECT_SOURCE_DIR}/src/*.cpp
# )

# # Exclude main.cpp safely
# list(FILTER MYLIB_SOURCES EXCLUDE REGEX ".*/main\\.cpp$")

# # Add generated files
# file(GLOB GENERATED_SOURCES
# ${PROJECT_SOURCE_DIR}/src-gen/v1/logger/methods/*.cpp
# )

# add_library(mylib STATIC
# ${MYLIB_SOURCES}
# ${GENERATED_SOURCES}
# )

# target_include_directories(mylib PUBLIC
# ${PROJECT_SOURCE_DIR}/include
# ${PROJECT_SOURCE_DIR}/src-gen
# ${COMMONAPI_INCLUDE_DIRS}
# ${COMMONAPI_SOMEIP_INCLUDE_DIRS}
# )

# target_link_libraries(mylib PUBLIC
# Threads::Threads
# ${COMMONAPI_LIBRARIES}
# ${COMMONAPI_SOMEIP_LIBRARIES}
# )

# # ============================================================
# # Executables
# # ============================================================
# message(STATUS "Building MAIN executable")
# add_executable(program
# src/main.cpp
# )
# target_include_directories(program PUBLIC
# ${PROJECT_SOURCE_DIR}/include
# ${PROJECT_SOURCE_DIR}/src-gen
# ${COMMONAPI_INCLUDE_DIRS}
# ${COMMONAPI_SOMEIP_INCLUDE_DIRS}
# )
# target_link_libraries(program
# mylib
# ${COMMONAPI_LIBRARIES}
# ${COMMONAPI_SOMEIP_LIBRARIES}
# )

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
set(
BUILD_TYPE
"MAIN"
CACHE STRING
"Build type: MAIN, TEST_LOGMANAGER, TEST_LOGMESSAGE, or SERVER"
)

# ============================================================
# Dependencies
# ============================================================
find_package(GTest REQUIRED)
find_package(Threads REQUIRED)

# Try to find CommonAPI
find_package(CommonAPI QUIET)
find_package(CommonAPI-SomeIP QUIET)

# ============================================================
# Include directories
# ============================================================
include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR}/src-gen)
include_directories(/usr/local/include)
include_directories(/usr/include)

if(COMMONAPI_FOUND)
    include_directories(${COMMONAPI_INCLUDE_DIRS})
endif()

if(COMMONAPI_SOMEIP_FOUND)
    include_directories(${COMMONAPI_SOMEIP_INCLUDE_DIRS})
endif()

# ============================================================
# Link directories
# ============================================================
link_directories(/usr/local/lib)
link_directories(/usr/lib)
link_directories(/usr/lib/x86_64-linux-gnu)

# ============================================================
# Library sources
# ============================================================
file(GLOB MYLIB_SOURCES
${PROJECT_SOURCE_DIR}/src/*.cpp
)

# Exclude main.cpp and server.cpp
list(FILTER MYLIB_SOURCES EXCLUDE REGEX ".*/main\\.cpp$")
list(FILTER MYLIB_SOURCES EXCLUDE REGEX ".*/server\\.cpp$")

# Add generated files
file(GLOB GENERATED_SOURCES
${PROJECT_SOURCE_DIR}/src-gen/v1/logger/methods/*.cpp
)

add_library(mylib STATIC
${MYLIB_SOURCES}
${GENERATED_SOURCES}
)

target_include_directories(mylib PUBLIC
${PROJECT_SOURCE_DIR}/include
${PROJECT_SOURCE_DIR}/src-gen
)

target_link_libraries(mylib PUBLIC
Threads::Threads
)

# ============================================================
# Executables
# ============================================================
if(BUILD_TYPE STREQUAL "MAIN")
    message(STATUS "Building MAIN executable (Client)")
    add_executable(program
        src/main.cpp
    )
    target_link_libraries(program PRIVATE
        mylib
        Threads::Threads
        CommonAPI
        CommonAPI-SomeIP
    )

elseif(BUILD_TYPE STREQUAL "SERVER")
    message(STATUS "Building SERVER executable")
    add_executable(server
        src/server.cpp
    )
    target_link_libraries(server PRIVATE
        mylib
        Threads::Threads
        CommonAPI
        CommonAPI-SomeIP
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
"Must be MAIN, TEST_LOGMANAGER, TEST_LOGMESSAGE, or SERVER"
    )
endif()