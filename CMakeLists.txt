CMAKE_MINIMUM_REQUIRED(VERSION 3.21)
PROJECT(io_module CXX)


SET(CMAKE_BUILD_RPATH_USE_ORIGIN ON)
SET(CMAKE_INSTALL_RPATH "$ORIGIN")
SET(CMAKE_CXX_STANDARD 20)
SET(CMAKE_CXX_EXTENSIONS OFF)   # For increased portability
SET(CMAKE_POSITION_INDEPENDENT_CODE ON)

SET(IO_MODULE_VERSION 1.3.2)

OPTION(BRINGAUTO_INSTALL "Configure install" OFF)
OPTION(BRINGAUTO_PACKAGE "Configure package creation" OFF)
OPTION(BRINGAUTO_SYSTEM_DEP "System dependencies are used if switched to ON, packager used if OFF" OFF)
OPTION(BRINGAUTO_GET_PACKAGES_ONLY "Only download packages for this project" OFF)
OPTION(FLEET_PROTOCOL_BUILD_MODULE_GATEWAY "Build shared library for module gateway " ON)
OPTION(FLEET_PROTOCOL_BUILD_EXTERNAL_SERVER "Build shared library for external server " ON)

IF (BRINGAUTO_PACKAGE)
    IF (NOT BRINGAUTO_INSTALL)
        SET(BRINGAUTO_INSTALL ON
                CACHE BOOL
                "Installation forced by BRINGAUTO_PACKAGE creation"
                FORCE
        )
    ENDIF ()
    IF (NOT FLEET_PROTOCOL_BUILD_MODULE_GATEWAY)
        SET(FLEET_PROTOCOL_BUILD_MODULE_GATEWAY ON
                CACHE BOOL
                "Module gateway module build forced by BRINGAUTO_PACKAGE creation"
                FORCE
        )
    ENDIF ()
    IF (NOT FLEET_PROTOCOL_BUILD_EXTERNAL_SERVER)
        SET(FLEET_PROTOCOL_BUILD_EXTERNAL_SERVER ON
                CACHE BOOL
                "External server module build forced by BRINGAUTO_PACKAGE creation"
                FORCE
        )
    ENDIF ()
ENDIF ()

FIND_PACKAGE(CMLIB
    COMPONENTS CMDEF CMUTIL STORAGE
    REQUIRED
    )

IF (NOT BRINGAUTO_SYSTEM_DEP)
    INCLUDE(cmake/Dependencies.cmake)
    IF (BRINGAUTO_GET_PACKAGES_ONLY)
        RETURN()
    ENDIF ()
ENDIF ()

FIND_PACKAGE(nlohmann_json 3.10.5 REQUIRED)
FIND_PACKAGE(fleet-protocol-cxx-helpers-static 1.1.1 REQUIRED)
FIND_PACKAGE(fleet-protocol-interface 2.0.0 REQUIRED)

IF (FLEET_PROTOCOL_BUILD_EXTERNAL_SERVER)
    FIND_PACKAGE(fleet-http-client-shared 1.5.0 REQUIRED)
    ### fleet-http-client dependencies
    FIND_PACKAGE(Boost CONFIG REQUIRED)
    FIND_PACKAGE(ZLIB REQUIRED)
    FIND_PACKAGE(cpprestsdk REQUIRED)
ENDIF ()

CMDEF_ADD_LIBRARY(
        LIBRARY_GROUP io-module
        TYPE INTERFACE
        VERSION ${IO_MODULE_VERSION}
)

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

IF (FLEET_PROTOCOL_BUILD_MODULE_GATEWAY)
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
    TARGET_LINK_LIBRARIES(io-module-interface INTERFACE
            io-module-gateway-shared
    )
ENDIF ()

IF (FLEET_PROTOCOL_BUILD_EXTERNAL_SERVER)
    CMDEF_ADD_LIBRARY(
            LIBRARY_GROUP io-external-server
            TYPE SHARED
            SOURCES "source/external_server_api.cpp" "source/memory_management.cpp" "source/device_management.cpp"
            VERSION ${IO_MODULE_VERSION}
    )
    # Explicitly add Boost dependencies due to obsolete cpprest cmake export package...
    target_link_libraries(fleet-http-client-shared::fleet-http-client-shared
            INTERFACE
                Boost::regex Boost::date_time Boost::atomic Boost::random Boost::chrono
                Boost::system Boost::filesystem Boost::thread
                ZLIB::ZLIB
    )
    TARGET_LINK_LIBRARIES(io-external-server-shared
            PRIVATE
            io_module_sources
            fleet-http-client-shared::fleet-http-client-shared
            PUBLIC
            fleet-protocol-interface::module-maintainer-external-server-interface
    )
    TARGET_LINK_LIBRARIES(io-module-interface INTERFACE
            io-external-server-shared
    )
ENDIF ()

IF (BRINGAUTO_INSTALL)
    IF (FLEET_PROTOCOL_BUILD_MODULE_GATEWAY)
        CMDEF_INSTALL(
                TARGET io-module-gateway-shared
                NAMESPACE io-module-interface::
        )
    ENDIF ()
    IF (FLEET_PROTOCOL_BUILD_EXTERNAL_SERVER)
        CMDEF_INSTALL(
                TARGET io-external-server-shared
                NAMESPACE io-module-interface::
        )
    ENDIF ()
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
