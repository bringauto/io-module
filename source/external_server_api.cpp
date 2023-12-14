#include <external_server_interface.h>
#include <bringauto/io_module_utils/SerializationUtils.hpp>
#include <bringauto/modules/io_module/io_module.h>
#include <bringauto/io_module_utils/DeviceCommand.hpp>
#include <bringauto/io_module_utils/DeviceStatus.hpp>
#include <bringauto/io_module_utils/DeviceID.hpp>
#include <bringauto/io_module_utils/ConfigParameters.hpp>
#include <bringauto/io_module_utils/external_server_api_structures.hpp>
#include <bringauto/modules/io_module/devices/arduino_opta/arduino_opta_external_server.hpp>
#include <bringauto/modules/io_module/devices/arduino_mega/arduino_mega_external_server.hpp>
#include <bringauto/modules/io_module/devices/arduino_uno/arduino_uno_external_server.hpp>
 
#include <vector>
#include <cstring>
#include <iostream>

using namespace std::chrono_literals;

void *init(const config config_data) {
    auto *context = new struct bringauto::io_module_utils::context;

    return context;
}

int destroy(void **context) {
    if(*context == nullptr){
        return NOT_OK;
    }
    auto con = reinterpret_cast<struct bringauto::io_module_utils::context **> (context);

    delete *con;
    *con = nullptr;
    return OK;
}

int forward_status(const buffer device_status, const device_identification device, void *context) {
    if(context == nullptr){
        return CONTEXT_INCORRECT;
    }

    auto con = static_cast<struct bringauto::io_module_utils::context *> (context);

    switch(device.device_type) {
        case bringauto::modules::io_module::ARDUINO_OPTA_DEVICE_TYPE:
            return bringauto::modules::io_module::devices::arduino_opta::arduino_opta_forward_status(device_status, device, con);
        case bringauto::modules::io_module::ARDUINO_MEGA_DEVICE_TYPE:
            return bringauto::modules::io_module::devices::arduino_mega::arduino_mega_forward_status(device_status, device, con);
        case bringauto::modules::io_module::ARDUINO_UNO_DEVICE_TYPE:
            return bringauto::modules::io_module::devices::arduino_uno::arduino_uno_forward_status(device_status, device, con);
        default:
            return NOT_OK;     
    }
}

int forward_error_message(const buffer error_msg, const device_identification device, void *context) {
    if(context == nullptr){
        return CONTEXT_INCORRECT;
    }
    
    return OK;
}

int device_disconnected(const int disconnect_type, const device_identification device, void *context) {
    if(context == nullptr) {
        return CONTEXT_INCORRECT;
    }

    auto con = static_cast<struct bringauto::io_module_utils::context *> (context);

    const std::string_view device_device_role(static_cast<char*> (device.device_role.data), device.device_role.size_in_bytes);
    const std::string_view device_device_name(static_cast<char*> (device.device_name.data), device.device_name.size_in_bytes);

    for(auto it = con->devices.begin(); it != con->devices.end(); it++) {
        const std::string_view it_device_role(static_cast<char*> (it->device_role.data), it->device_role.size_in_bytes);
        const std::string_view it_device_name(static_cast<char*> (it->device_name.data), it->device_name.size_in_bytes);

        bool device_is_present = it->device_type == device.device_type && it_device_role == device_device_role && it_device_name == device_device_name
                                 && it->module == device.module && it->priority == device.priority;

        if(device_is_present) {
            deallocate(&it->device_role);
            deallocate(&it->device_name);
            con->devices.erase(it);
            return OK;
        }
    }

    return NOT_OK;
}

int device_connected(const device_identification device, void *context) {
    if(context == nullptr) {
        return CONTEXT_INCORRECT;
    }

    auto con = static_cast<struct bringauto::io_module_utils::context *> (context);

    device_identification new_device;

    new_device.module = device.module;
    new_device.device_type = device.device_type;
    new_device.priority = device.priority;

    if(allocate(&new_device.device_role, device.device_role.size_in_bytes) != OK) {
        return NOT_OK;
    }
    std::memcpy(new_device.device_role.data, device.device_role.data, new_device.device_role.size_in_bytes);

    if(allocate(&new_device.device_name, device.device_name.size_in_bytes) != OK) {
        return NOT_OK;
    }
    std::memcpy(new_device.device_name.data, device.device_name.data, new_device.device_name.size_in_bytes);

    con->devices.emplace_back(new_device);
    return OK;
}

int wait_for_command(int timeout_time_in_ms, void *context) {
    if(context == nullptr) {
        return CONTEXT_INCORRECT;
    }

    auto con = static_cast<struct bringauto::io_module_utils::context *> (context);
    
    std::unique_lock lock(con->mutex);
    bool rc = con->con_variable.wait_for(lock, std::chrono::milliseconds(timeout_time_in_ms), [&]{return !con->command_vector.empty();});
    if(rc) {
        return OK;
    }
    else {
        return TIMEOUT_OCCURRED;
    }
}

int pop_command(buffer* command, device_identification* device, void *context) {
    if(context == nullptr) {
        return CONTEXT_INCORRECT;
    }

    auto con = static_cast<struct bringauto::io_module_utils::context *> (context);
    auto command_object = std::get<0>(con->command_vector.back());

    if(command_object.serializeToBuffer(command) == NOT_OK) {
        return NOT_OK;
    }

    auto& device_id = std::get<1>(con->command_vector.back());

    device->module = device_id.getDeviceId().module;
    device->device_type = device_id.getDeviceId().device_type;
    device->priority = device_id.getDeviceId().priority;

    if(allocate(&device->device_role, device_id.getDeviceId().device_role.size_in_bytes) == NOT_OK) {
        return NOT_OK;
    }
    std::memcpy(device->device_role.data, device_id.getDeviceId().device_role.data, device->device_role.size_in_bytes);

    if(allocate(&device->device_name, device_id.getDeviceId().device_name.size_in_bytes) == NOT_OK) {
        return NOT_OK;
    }
    std::memcpy(device->device_name.data, device_id.getDeviceId().device_name.data, device->device_name.size_in_bytes);

    con->command_vector.pop_back();

    return static_cast<int> (con->command_vector.size());
}

int command_ack(const buffer command, const device_identification device, void *context) {
    return OK;
}
