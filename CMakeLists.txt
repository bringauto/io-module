CMAKE_MINIMUM_REQUIRED(VERSION 3.21)
PROJECT(io_module CXX)

SET(CMAKE_CXX_STANDARD 20)

OPTION(BRINGAUTO_SAMPLES "Build samples" OFF)
OPTION(BRINGAUTO_SYSTEM_DEP "System dependencies used if switched to ON, packager used if ON" OFF)

FIND_PACKAGE(CMLIB
    COMPONENTS CMDEF CMUTIL STORAGE
    REQUIRED
    )

IF (NOT BRINGAUTO_SYSTEM_DEP)
    INCLUDE(cmake/Dependencies.cmake)
ENDIF ()

FIND_PACKAGE(nlohmann_json 3.10.5 REQUIRED)

IF (NOT TARGET common_headers)
    ADD_SUBDIRECTORY("lib/fleet-protocol")
ENDIF ()

ADD_SUBDIRECTORY("lib/fleet-http-client")
ADD_SUBDIRECTORY("lib/fleet-protocol-cpp")

FILE(GLOB_RECURSE HEADERS "include/*")

ADD_LIBRARY(io_module_hpp INTERFACE ${HEADERS})
TARGET_INCLUDE_DIRECTORIES(io_module_hpp INTERFACE "include/")

FILE(GLOB_RECURSE SOURCES "source/bringauto/*")

ADD_LIBRARY(io_module_sources STATIC ${SOURCES})
SET_PROPERTY(TARGET io_module_sources PROPERTY POSITION_INDEPENDENT_CODE ON)
TARGET_LINK_LIBRARIES(io_module_sources PUBLIC io_module_hpp module_gateway_maintainer external_server nlohmann_json::nlohmann_json
                      fleet_http_client fleet-protocol::cxx_helpers)

ADD_LIBRARY(io_module_gateway SHARED "source/module_manager.cpp" "source/device_management.cpp" "source/memory_management.cpp")
TARGET_LINK_LIBRARIES(io_module_gateway PRIVATE io_module_hpp io_module_sources external_server common_headers
                      module_gateway_maintainer nlohmann_json::nlohmann_json)

ADD_LIBRARY(io_external_server SHARED "source/external_server_api.cpp" "source/device_management.cpp" "source/memory_management.cpp")
TARGET_LINK_LIBRARIES(io_external_server PRIVATE io_module_hpp io_module_sources external_server common_headers
                      module_gateway_maintainer nlohmann_json::nlohmann_json fleet_http_client fleet-protocol::cxx_helpers)
