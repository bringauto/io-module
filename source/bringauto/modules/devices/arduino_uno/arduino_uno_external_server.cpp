#include <bringauto/modules/io_module/devices/arduino_uno/arduino_uno_external_server.hpp>
#include <module_manager.h>
#include <bringauto/modules/io_module/io_module.h>
#include <bringauto/io_module_utils/DeviceStatus.hpp>
#include <bringauto/io_module_utils/DeviceCommand.hpp>

namespace bringauto::modules::io_module::devices::arduino_uno {

static bringauto::io_module_utils::DeviceCommand arduino_uno_generate_command(bringauto::io_module_utils::DeviceStatus<bringauto::modules::io_module::ARDUINO_UNO_INPUTS_COUNT,
                                                                  bringauto::modules::io_module::ARDUINO_UNO_OUTPUTS_COUNT> status) {
    bringauto::io_module_utils::DeviceCommand command;

    for(int i = bringauto::modules::io_module::InputIndexes::INPUT_1; i < bringauto::modules::io_module::ARDUINO_UNO_INPUTS_COUNT; i++) {
        if(status.getInputStates()[i]) {
            command.addOutputAction(bringauto::io_module_utils::OutputAction(i, bringauto::io_module_utils::ActionType::SET_ON));
        }
        else {
            command.addOutputAction(bringauto::io_module_utils::OutputAction(i, bringauto::io_module_utils::ActionType::SET_OFF));
        }
    }
    return command;
}

int arduino_uno_forward_status(const buffer device_status, const device_identification device, io_module_utils::context *context) {
    bringauto::io_module_utils::DeviceStatus<bringauto::modules::io_module::ARDUINO_UNO_INPUTS_COUNT,
                                             bringauto::modules::io_module::ARDUINO_UNO_OUTPUTS_COUNT> status;
    
    if(status.deserializeFromBuffer(device_status) == NOT_OK) {
        return NOT_OK;
    }

    auto command = arduino_uno_generate_command(status);

    {
        std::lock_guard lock(context->mutex);
        context->command_vector.emplace_back(command, bringauto::io_module_utils::DeviceID(device.module, device.device_type, device.priority, device.device_role, device.device_name));
    }
    context->con_variable.notify_one();

    return OK;
}

}
