cmake_minimum_required(VERSION 3.10)
project(telementry VERSION 1.0 LANGUAGES CXX)

# ============================================================
# C++ standard
# ============================================================
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ============================================================
# Dependencies
# ============================================================
find_package(GTest REQUIRED)
find_package(Threads REQUIRED)

# Try to find CommonAPI
find_package(CommonAPI REQUIRED)
find_package(CommonAPI-SomeIP REQUIRED)
find_package(vsomeip3 REQUIRED)

# ============================================================
# Include directories
# ============================================================
# Your own headers
include_directories(${PROJECT_SOURCE_DIR}/include)

# Generated CommonAPI files
include_directories(${PROJECT_SOURCE_DIR}/common-api/src-gen)
include_directories(${PROJECT_SOURCE_DIR}/common-api/src-gen/v1)
include_directories(${PROJECT_SOURCE_DIR}/common-api/src-gen/v1/v1)

# CommonAPI includes
include_directories(${COMMONAPI_INCLUDE_DIRS})
include_directories(${COMMONAPI_SOMEIP_INCLUDE_DIRS})

# ============================================================
# Sources
# ============================================================
file(GLOB MYLIB_SOURCES
    ${PROJECT_SOURCE_DIR}/src/*.cpp
)

# Add generated files (adjusted to your actual tree)
set(GENERATED_SOURCES
    ${PROJECT_SOURCE_DIR}/common-api/src-gen/v1/v1/logger/methods/loggingSomeIPDeployment.cpp
    ${PROJECT_SOURCE_DIR}/common-api/src-gen/v1/v1/logger/methods/loggingSomeIPProxy.cpp
    ${PROJECT_SOURCE_DIR}/common-api/src-gen/v1/v1/logger/methods/loggingSomeIPStubAdapter.cpp
    ${PROJECT_SOURCE_DIR}/common-api/src-gen/v1/v1/logger/methods/TelemetryTypesSomeIPDeployment.cpp
)

# ============================================================
# Static library (mylib)
# ============================================================
add_library(mylib STATIC
    ${MYLIB_SOURCES}
    ${GENERATED_SOURCES}
)

target_include_directories(mylib PUBLIC
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/common-api/src-gen
)

target_link_libraries(mylib PUBLIC
    Threads::Threads
)

# ============================================================
# Client (program)
# ============================================================
message(STATUS "Building (Client)")
add_executable(program
    src/main.cpp
    ${GENERATED_SOURCES}
)

target_link_libraries(program PRIVATE
    mylib
    Threads::Threads
    CommonAPI
    CommonAPI-SomeIP
)

# ============================================================
# Server
# ============================================================
# NOTE: your server sources are under ServerApp/ in the tree
message(STATUS "Building (Server)")
add_executable(server
    ServerApp/server.cpp
    ServerApp/telemetry_sampler.cpp
    ${GENERATED_SOURCES}
)

target_link_libraries(server PRIVATE
    mylib
    Threads::Threads
    CommonAPI
    CommonAPI-SomeIP
)
