#pragma once

#include <fleet_protocol/common_headers/memory_management.h>
#include <bringauto/fleet_protocol/cxx/DeviceID.hpp>

#include <memory>
#include <nlohmann/json.hpp>

namespace bringauto::io_module_utils {

using json = nlohmann::ordered_json;

class SerializationUtils {

public:
    int static json_to_buffer(json json, buffer *buffer);

    int static serialize_error(buffer *error_buffer, int aggregated_button_presses);

    int static deserialize_error(char* error_buffer, size_t buffer_size);

    struct bringauto::fleet_protocol::cxx::DeviceID static deserialize_device_id(char* device_buffer, size_t buffer_size);
};

}
