CMAKE_MINIMUM_REQUIRED(VERSION 3.21)
PROJECT(io_module CXX)

SET(CMAKE_BUILD_RPATH_USE_ORIGIN ON)
SET(CMAKE_INSTALL_RPATH "$ORIGIN")
SET(CMAKE_CXX_STANDARD 20)
SET(CMAKE_CXX_EXTENSIONS OFF)   # For increased portability
SET(CMAKE_POSITION_INDEPENDENT_CODE ON)

SET(IO_MODULE_VERSION 1.2.0)

OPTION(BRINGAUTO_INSTALL "Configure install" OFF)
OPTION(BRINGAUTO_PACKAGE "Configure package creation" OFF)
OPTION(BRINGAUTO_SYSTEM_DEP "System dependencies are used if switched to ON, packager used if OFF" OFF)


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
FIND_PACKAGE(fleet-http-client-shared 1.2.2 REQUIRED)

### fleet-http-client dependencies
FIND_PACKAGE(Boost REQUIRED)
FIND_PACKAGE(cpprestsdk REQUIRED)
FIND_PACKAGE(libbringauto_logger 1.2.0 REQUIRED)
### fleet-http-client dependencies


FILE(GLOB_RECURSE HEADERS "include/*")
ADD_LIBRARY(io_module_hpp INTERFACE ${HEADERS})
TARGET_INCLUDE_DIRECTORIES(io_module_hpp INTERFACE "include/")

FILE(GLOB_RECURSE SOURCES "source/bringauto/*")
ADD_LIBRARY(io_module_sources STATIC ${SOURCES})
SET_PROPERTY(TARGET io_module_sources PROPERTY POSITION_INDEPENDENT_CODE ON)
TARGET_LINK_LIBRARIES(io_module_sources PUBLIC io_module_hpp nlohmann_json::nlohmann_json
                      fleet-protocol-interface::common-headers-interface
                      fleet-protocol-cxx-helpers-static::fleet-protocol-cxx-helpers-static
)

CMDEF_ADD_LIBRARY(
        LIBRARY_GROUP io-module-gateway
        TYPE SHARED
        SOURCES "source/module_manager.cpp" "source/memory_management.cpp" "source/device_management.cpp"
        VERSION ${IO_MODULE_VERSION}
)
TARGET_LINK_LIBRARIES(io-module-gateway-shared
        PRIVATE
        io_module_sources
        PUBLIC
        fleet-protocol-interface::common-headers-interface
        fleet-protocol-interface::module-maintainer-module-gateway-interface
)

CMDEF_ADD_LIBRARY(
        LIBRARY_GROUP io-external-server
        TYPE SHARED
        SOURCES "source/external_server_api.cpp" "source/memory_management.cpp" "source/device_management.cpp"
        VERSION ${IO_MODULE_VERSION}
)
TARGET_LINK_LIBRARIES(io-external-server-shared
        PRIVATE
        io_module_sources
        fleet-http-client-shared::fleet-http-client-shared
        PUBLIC
        fleet-protocol-interface::module-maintainer-external-server-interface
)

CMDEF_ADD_LIBRARY(
        LIBRARY_GROUP io-module
        TYPE INTERFACE
        VERSION ${IO_MODULE_VERSION}
)
TARGET_LINK_LIBRARIES(io-module-interface INTERFACE
        io-module-gateway-shared
        io-external-server-shared
)
IF (BRINGAUTO_INSTALL)
    CMDEF_INSTALL(
            TARGET io-module-gateway-shared
            NAMESPACE io-module-interface::
    )
    CMDEF_INSTALL(
            TARGET io-external-server-shared
            NAMESPACE io-module-interface::
    )
    CMDEF_INSTALL(
            TARGET io-module-interface
            NAMESPACE io-module-interface::
    )
    BA_PACKAGE_DEPS_IMPORTED(io-module-interface)
ENDIF ()

IF (BRINGAUTO_PACKAGE)
    CMDEF_PACKAGE(
            MAIN_TARGET io-module-interface
            VERSION ${IO_MODULE_VERSION}
    )
    SET(CPACK_GENERATOR ZIP)
    SET(CPACK_PACKAGE_CONTACT "BringAuto s.r.o. <maintainers@bringauto.com>")
    INCLUDE(CPack)
ENDIF ()
