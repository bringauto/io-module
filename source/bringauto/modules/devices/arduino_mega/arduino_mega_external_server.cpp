#include <bringauto/modules/io_module/devices/arduino_mega/arduino_mega_external_server.hpp>
#include <module_manager.h>
#include <bringauto/modules/io_module/io_module.h>
#include <bringauto/io_module_utils/DeviceStatus.hpp>
#include <bringauto/io_module_utils/DeviceCommand.hpp>
#include <bringauto/fleet_protocol/cxx/BufferAsString.hpp>
//#include <google/protobuf/util/json_util.h>

namespace bringauto::modules::io_module::devices::arduino_mega {

static bringauto::io_module_utils::DeviceCommand arduino_mega_generate_command(bringauto::io_module_utils::DeviceStatus<bringauto::modules::io_module::ARDUINO_MEGA_INPUTS_COUNT,
                                                                  bringauto::modules::io_module::ARDUINO_MEGA_OUTPUTS_COUNT> status) {
    bringauto::io_module_utils::DeviceCommand command;

    for(int i = bringauto::modules::io_module::InputIndexes::INPUT_1; i < bringauto::modules::io_module::ARDUINO_MEGA_INPUTS_COUNT; i++) {
        if(status.getInputStates()[i]) {
            command.addOutputAction(bringauto::io_module_utils::OutputAction(i, bringauto::io_module_utils::ActionType::SET_ON));
        }
        else {
            command.addOutputAction(bringauto::io_module_utils::OutputAction(i, bringauto::io_module_utils::ActionType::SET_OFF));
        }
    }
    return command;
}

int arduino_mega_forward_status(const buffer device_status, const device_identification device, io_module_utils::context *context) {
    bringauto::io_module_utils::DeviceStatus<bringauto::modules::io_module::ARDUINO_MEGA_INPUTS_COUNT,
                                             bringauto::modules::io_module::ARDUINO_MEGA_OUTPUTS_COUNT> status;
    
    if(status.deserializeFromBuffer(device_status) == NOT_OK) {
        return NOT_OK;
    }

    bringauto::fleet_protocol::cxx::BufferAsString device_role(&device.device_role);
    bringauto::fleet_protocol::cxx::BufferAsString device_name(&device.device_name);

    auto command = arduino_mega_generate_command(status);
    {
        std::lock_guard lock(context->mutex);
        context->command_vector.emplace_back(command, bringauto::fleet_protocol::cxx::DeviceID(
            device.module, device.device_type, device.priority,
            device_role.getStringView(), device_name.getStringView()
        ));
    }

    std::string device_status_str;
    //TODO parse device status (io module not compiled in protobuf?)
    //auto device_status_parsed = bringauto::protobuf::ProtobufHelper::parseAutonomyStatus(device_status);
    //google::protobuf::util::MessageToJsonString(device_status_parsed, &device_status_str);
    
    context->fleet_api_client->setDeviceIdentification(
        device.module,
        device.device_type,
        std::string(device_role.getStringView()),
        std::string(device_name.getStringView())
    );

    try {
        context->fleet_api_client->sendStatus(device_status_str);
    } catch (std::exception& e) {
        return NOT_OK;
    }

    context->con_variable.notify_one();

    return OK;
}

}
