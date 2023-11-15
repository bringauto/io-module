#include <bringauto/io_module_utils/SerializationUtils.hpp>
#include <bringauto/modules/io_module/io_module.h>
#include <general_error_codes.h>

#include <nlohmann/json.hpp>
#include <memory>
#include <string>
#include <iostream>

namespace bringauto::io_module_utils {

using json = nlohmann::ordered_json;

int SerializationUtils::json_to_buffer(json json, buffer *buffer) {
    std::string tmp = to_string(json);

    if(allocate(buffer, tmp.size()) == NOT_OK) {
        return NOT_OK;
    }

    std::memcpy(buffer->data, tmp.c_str(), tmp.size());

    return OK;
}

int SerializationUtils::serialize_error(buffer *error_buffer, int aggregated_button_presses) {
    json j;
    j["aggregatedButtonPresses"] = aggregated_button_presses;

    if(json_to_buffer(j, error_buffer) == NOT_OK) {
        return NOT_OK;
    }

    return OK;
}

int SerializationUtils::deserialize_error(char* error_buffer, size_t buffer_size) {
    json error_json;
    int aggregated_button_presses = 0;

    try {
        error_json = json::parse(error_buffer, error_buffer + buffer_size);
    }
    catch(const json::parse_error&) {
        return -1;
    }

    try {
        aggregated_button_presses = error_json.at("aggregatedButtonPresses");
    }
    catch(const json::out_of_range&) {
        return -1;
    }

    return aggregated_button_presses;

}

DeviceID SerializationUtils::deserialize_device_id(char* device_buffer, size_t buffer_size) {
    json device_json;

    int module;
    unsigned int device_type, priority;
    std::string device_role, device_name;

    try {
        device_json = json::parse(device_buffer, device_buffer + buffer_size);
    }
    catch(const json::parse_error&) {
        return DeviceID(0, 0, 0, "", "");
    }

    try {
        module = device_json.at("module");
        device_type = device_json.at("device_type");
        priority = device_json.at("priority");
        device_role = device_json.at("device_role");
        device_name = device_json.at("device_name");
    }
    catch(const json::out_of_range&) {
        return DeviceID(0, 0, 0, "", "");
    }

    return DeviceID(module, device_type, priority, device_role.c_str(), device_name.c_str());
}

}
