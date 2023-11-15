#pragma once

#include <bringauto/io_module_utils/SerializationUtils.hpp>
#include <device_management.h>

#include <string>

namespace bringauto::io_module_utils {

/**
 * @brief Class, which encapsulates device_identification c-like struct and replaces c strings in device_identification
 *        with c++ strings to avoid allocating on heap.
*/
struct DeviceID {
    
    /**
     * @brief Constructs DeviceID from device_identification values and c strings for device_name and device_role.
     *        Unsafe to use with non null terminated strings. Do not use when converting from device_identification
     *        structure, use the other constructor.
    */
    DeviceID(int module, unsigned int device_type, unsigned int priority, const char* device_role, const char* device_name);

    /**
     * @brief Constructs DeviceID from device_identification values with buffer structures for strings. 
    */
    DeviceID(int module, unsigned int device_type, unsigned int priority, buffer device_role, buffer device_name);

    /**
     * @brief Move constructor.
    */
    DeviceID(DeviceID&& c);

    // Deleting the default copy constructor
    DeviceID(const DeviceID& c) = delete;

    const device_identification& getDeviceId() const { return id_; }

private:
    device_identification id_;
    const std::string device_role_;
    const std::string device_name_;
};

}
