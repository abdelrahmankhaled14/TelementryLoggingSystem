cmake_minimum_required(VERSION 3.16)
project(telemetry_gui LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5 REQUIRED COMPONENTS Core Gui Qml Quick)

# ---- CommonAPI libraries ----
find_library(COMMONAPI_LIB
    NAMES CommonAPI
    HINTS /usr/local/lib /usr/lib
)
find_library(COMMONAPI_SOMEIP_LIB
    NAMES CommonAPI-SomeIP
    HINTS /usr/local/lib /usr/lib
)
find_library(VSOMEIP_LIB
    NAMES vsomeip3 vsomeip
    HINTS /usr/local/lib /usr/lib
)

if (NOT COMMONAPI_LIB)
    message(FATAL_ERROR "Could not find libCommonAPI")
endif()
if (NOT COMMONAPI_SOMEIP_LIB)
    message(FATAL_ERROR "Could not find libCommonAPI-SomeIP")
endif()
if (NOT VSOMEIP_LIB)
    message(WARNING "Could not find vsomeip library, proceeding without explicit link")
endif()

add_executable(telemetry_gui
    main.cpp
    LogParser.h
    LogParser.cpp
    qml.qrc
)

# Import your core static library from the main build
add_library(mylib STATIC IMPORTED)
set_target_properties(mylib PROPERTIES
    IMPORTED_LOCATION "${CMAKE_SOURCE_DIR}/../build/libmylib.a"
)

target_include_directories(telemetry_gui PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/../include
    ${CMAKE_SOURCE_DIR}/../src-gen
    /usr/local/include/CommonAPI-3.2
)

target_link_libraries(telemetry_gui PRIVATE
    Qt5::Core
    Qt5::Gui
    Qt5::Qml
    Qt5::Quick
    mylib
    ${COMMONAPI_LIB}
    ${COMMONAPI_SOMEIP_LIB}
    ${VSOMEIP_LIB}
)

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_options(telemetry_gui PRIVATE -Wall -Wextra -Wpedantic)
endif()