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
FIND_PACKAGE(fleet-protocol-cxx-helpers-static 1.1.1 REQUIRED)
FIND_PACKAGE(fleet-protocol-interface 2.0.0 REQUIRED)

ADD_SUBDIRECTORY("lib/fleet-http-client")

FILE(GLOB_RECURSE HEADERS "include/*")

ADD_LIBRARY(io_module_hpp INTERFACE ${HEADERS})
TARGET_INCLUDE_DIRECTORIES(io_module_hpp INTERFACE "include/")

FILE(GLOB_RECURSE SOURCES "source/bringauto/*")

ADD_LIBRARY(io_module_sources STATIC ${SOURCES})
SET_PROPERTY(TARGET io_module_sources PROPERTY POSITION_INDEPENDENT_CODE ON)
TARGET_LINK_LIBRARIES(io_module_sources PUBLIC io_module_hpp nlohmann_json::nlohmann_json
                      fleet-http-client-static
                      fleet-protocol-interface::common-headers-interface
                      fleet-protocol-interface::module-maintainer-module-gateway-interface
                      fleet-protocol-interface::module-maintainer-external-server-interface
                      fleet-protocol-cxx-helpers-static::fleet-protocol-cxx-helpers-static)

ADD_LIBRARY(io_module_gateway SHARED "source/module_manager.cpp" "source/device_management.cpp" "source/memory_management.cpp")
TARGET_LINK_LIBRARIES(io_module_gateway PRIVATE io_module_hpp io_module_sources nlohmann_json::nlohmann_json
                      fleet-protocol-interface::common-headers-interface
                      fleet-protocol-interface::module-maintainer-module-gateway-interface)

ADD_LIBRARY(io_external_server SHARED "source/external_server_api.cpp" "source/device_management.cpp" "source/memory_management.cpp")
TARGET_LINK_LIBRARIES(io_external_server PRIVATE io_module_hpp io_module_sources nlohmann_json::nlohmann_json 
                      fleet-http-client-static
                      fleet-protocol-interface::common-headers-interface
                      fleet-protocol-interface::module-maintainer-module-gateway-interface
                      fleet-protocol-interface::module-maintainer-external-server-interface
                      fleet-protocol-cxx-helpers-static::fleet-protocol-cxx-helpers-static)
