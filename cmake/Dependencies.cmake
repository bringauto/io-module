SET(CMAKE_FIND_USE_CMAKE_SYSTEM_PATH FALSE)

BA_PACKAGE_LIBRARY(nlohmann-json            v3.10.5 NO_DEBUG ON)
BA_PACKAGE_LIBRARY(zlib                     v1.2.11)
BA_PACKAGE_LIBRARY(fleet-protocol-cpp       v1.1.1)
BA_PACKAGE_LIBRARY(fleet-protocol-interface v2.0.0 NO_DEBUG ON)

IF (FLEET_PROTOCOL_BUILD_EXTERNAL_SERVER)
    BA_PACKAGE_LIBRARY(fleet-http-client-shared v1.5.0)
    BA_PACKAGE_LIBRARY(boost                    v1.86.0)
    BA_PACKAGE_LIBRARY(cpprestsdk               v2.10.20)
ENDIF ()
