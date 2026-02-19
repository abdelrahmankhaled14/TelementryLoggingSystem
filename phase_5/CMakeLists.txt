
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
include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR}/src-gen)


include_directories(${COMMONAPI_INCLUDE_DIRS})

include_directories(${COMMONAPI_SOMEIP_INCLUDE_DIRS})


file(GLOB MYLIB_SOURCES
${PROJECT_SOURCE_DIR}/src/*.cpp
)

# Add generated files
set(GENERATED_SOURCES
    ${PROJECT_SOURCE_DIR}/src-gen/v1/logger/methods/loggingSomeIPDeployment.cpp
    ${PROJECT_SOURCE_DIR}/src-gen/v1/logger/methods/loggingSomeIPProxy.cpp
    ${PROJECT_SOURCE_DIR}/src-gen/v1/logger/methods/loggingSomeIPStubAdapter.cpp
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

message(STATUS "Building (Server)")
add_executable(server
    src/server.cpp
    ${GENERATED_SOURCES}
)
target_link_libraries(server PRIVATE
    mylib
    Threads::Threads
    CommonAPI
    CommonAPI-SomeIP
)