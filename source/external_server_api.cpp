#include <fleet_protocol/module_maintainer/external_server/external_server_interface.h>
#include <bringauto/io_module_utils/SerializationUtils.hpp>
#include <bringauto/modules/io_module/io_module.h>
#include <bringauto/io_module_utils/DeviceCommand.hpp>
#include <bringauto/io_module_utils/DeviceStatus.hpp>
#include <bringauto/io_module_utils/ConfigParameters.hpp>
#include <bringauto/io_module_utils/external_server_api_structures.hpp>
#include <bringauto/fleet_protocol/http_client/FleetApiClient.hpp>
#include <bringauto/fleet_protocol/cxx/KeyValueConfig.hpp>
#include <bringauto/fleet_protocol/cxx/StringAsBuffer.hpp>
 
#include <vector>
#include <cstring>
#include <iostream>
#include <regex>

using namespace std::chrono_literals;

void *init(const config config_data) {
    auto *context = new struct bringauto::io_module_utils::context;
    bringauto::fleet_protocol::cxx::KeyValueConfig config(config_data);
    std::string api_url;
    std::string api_key;
    std::string company_name;
    std::string car_name;
    int max_requests_threshold_count;
    int max_requests_threshold_period_ms;
    int delay_after_threshold_reached_ms;
    int retry_requests_delay_ms;

    for (auto i = config.cbegin(); i != config.cend(); i++) {
        if (i->first == "api_url") {
            if (!std::regex_match(i->second, std::regex(R"(^(http|https)://([\w-]+\.)?+[\w-]+(:[0-9]+)?(/[\w-]*)?+$)"))) {
                delete context;
                return nullptr;
            }
            api_url = i->second;
        }
        else if (i->first == "api_key") {
            if (i->second.empty()) {
                delete context;
                return nullptr;
            }
            api_key = i->second;
        }
        else if (i->first == "company_name") {
            if (!std::regex_match(i->second, std::regex("^[a-z0-9_]*$")) || i->second.empty()) {
                delete context;
                return nullptr;
            }
            company_name = i->second;
        }
        else if (i->first == "car_name") {
            if (!std::regex_match(i->second, std::regex("^[a-z0-9_]*$")) || i->second.empty()) {
                delete context;
                return nullptr;
            }
            car_name = i->second;
        }
        else if (i->first == "max_requests_threshold_count") {
            try {
                max_requests_threshold_count = std::stoi(i->second);
                if (max_requests_threshold_count < 0 || i->second.empty()) {
                    throw std::exception();
                }
            } catch (std::exception& e) {
                delete context;
                return nullptr;
            }
        }
        else if (i->first == "max_requests_threshold_period_ms") {
            try {
                max_requests_threshold_period_ms = std::stoi(i->second);
                if (max_requests_threshold_period_ms < 0 || i->second.empty()) {
                    throw std::exception();
                }
            } catch (std::exception& e) {
                delete context;
                return nullptr;
            }
        }
        else if (i->first == "delay_after_threshold_reached_ms") {
            try {
                delay_after_threshold_reached_ms = std::stoi(i->second);
                if (delay_after_threshold_reached_ms < 0 || i->second.empty()) {
                    throw std::exception();
                }
            } catch (std::exception& e) {
                delete context;
                return nullptr;
            }
        }
        else if (i->first == "retry_requests_delay_ms") {
            try {
                retry_requests_delay_ms = std::stoi(i->second);
                if (retry_requests_delay_ms < 0 || i->second.empty()) {
                    throw std::exception();
                }
            } catch (std::exception& e) {
                delete context;
                return nullptr;
            }
        }
    }

    bringauto::fleet_protocol::http_client::FleetApiClient::FleetApiClientConfig fleet_api_config {
        .apiUrl = api_url,
        .apiKey = api_key,
        .companyName = company_name,
        .carName = car_name
    };

    bringauto::fleet_protocol::http_client::RequestFrequencyGuard::RequestFrequencyGuardConfig request_frequency_guard_config {
        .maxRequestsThresholdCount = max_requests_threshold_count,
        .maxRequestsThresholdPeriodMs = std::chrono::milliseconds(max_requests_threshold_period_ms),
        .delayAfterThresholdReachedMs = std::chrono::milliseconds(delay_after_threshold_reached_ms),
        .retryRequestsDelayMs = std::chrono::milliseconds(retry_requests_delay_ms)
    };

    context->fleet_api_client = std::make_shared<bringauto::fleet_protocol::http_client::FleetApiClient>(
        fleet_api_config, request_frequency_guard_config
    );

    context->last_command_timestamp = 0;
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

    bringauto::fleet_protocol::cxx::BufferAsString device_role(&device.device_role);
    bringauto::fleet_protocol::cxx::BufferAsString device_name(&device.device_name);
    bringauto::fleet_protocol::cxx::BufferAsString device_status_str(&device_status);
    
    con->fleet_api_client->setDeviceIdentification(
        bringauto::fleet_protocol::cxx::DeviceID(
            device.module,
            device.device_type,
            0, //priority
            std::string(device_role.getStringView()),
            std::string(device_name.getStringView())
        )
    );

    try {
        auto str = std::string(device_status_str.getStringView());
        con->fleet_api_client->sendStatus(str);
    } catch (std::exception& e) {
        return NOT_OK;
    }

    con->con_variable.notify_one();

    return OK;
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
    std::vector<std::shared_ptr<org::openapitools::client::model::Message>> commands;
    bool parse_commands = con->last_command_timestamp != 0;
    
    try {
        commands = con->fleet_api_client->getCommands(con->last_command_timestamp + 1, true);
    } catch (std::exception& e) {
        return TIMEOUT_OCCURRED;
    }

    for(auto command : commands) {
        if(command->getTimestamp() > con->last_command_timestamp) {
            con->last_command_timestamp = command->getTimestamp();
        }

        if(parse_commands) {
            auto received_device_id = command->getDeviceId();
            std::string command_str = command->getPayload()->getData()->getJson().serialize();

            bringauto::io_module_utils::DeviceCommand command_obj;
            buffer command_buff {nullptr, 0};
            bringauto::fleet_protocol::cxx::StringAsBuffer::createBufferAndCopyData(&command_buff, command_str);

            if(command_obj.deserializeFromBuffer(command_buff) == NOT_OK) {
                return NOT_OK;
            }

            con->command_vector.emplace_back(command_obj, bringauto::fleet_protocol::cxx::DeviceID(
                received_device_id->getModuleId(),
                received_device_id->getType(),
                0, // priority not returned from HTTP Api
                received_device_id->getRole(),
                received_device_id->getName()
            ));
        }
    }

    if(commands.empty() || !parse_commands) {
        return TIMEOUT_OCCURRED;
    }
    else {
        return OK;
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
