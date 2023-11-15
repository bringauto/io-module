#include <bringauto/modules/io_module/devices/arduino_opta/arduino_opta_external_server.hpp>
#include <module_manager.h>
#include <bringauto/modules/io_module/io_module.h>
#include <bringauto/io_module_utils/DeviceStatus.hpp>
#include <bringauto/io_module_utils/DeviceCommand.hpp>

namespace bringauto::modules::io_module::devices::arduino_opta {

/**
 * @brief Implementation of command logic based on status from device. The logic:
 *          - if any input is set --> all output are set on
 *          - else if all inputs are set off and button wasn't pressed (no input on device) --> all outputs are set off
 *          - else if all inputs are set off and button on device was pressed --> the output of number of presses is set on 
 *            (or output 4 if there is more presses than 4)
 * 
 * @param status status, on which the logic is based on
*/
static bringauto::io_module_utils::DeviceCommand arduino_opta_generate_command(bringauto::io_module_utils::DeviceStatus<bringauto::modules::io_module::ARDUINO_OPTA_INPUTS_COUNT,
                                                                  bringauto::modules::io_module::ARDUINO_OPTA_OUTPUTS_COUNT> status) {
    bringauto::io_module_utils::DeviceCommand command;

    bool input_states_all_false = true;

    for(auto input : status.getInputStates()) {
        if(input) {
            input_states_all_false = false;
            break;
        }
    }

    bool no_button_presses = status.getButtonPresses() == 0;

    if(!input_states_all_false) {
        command.addOutputAction(bringauto::io_module_utils::OutputAction(bringauto::modules::io_module::OutputIndexes::OUTPUT_1, bringauto::io_module_utils::ActionType::SET_ON));
        command.addOutputAction(bringauto::io_module_utils::OutputAction(bringauto::modules::io_module::OutputIndexes::OUTPUT_2, bringauto::io_module_utils::ActionType::SET_ON));
        command.addOutputAction(bringauto::io_module_utils::OutputAction(bringauto::modules::io_module::OutputIndexes::OUTPUT_3, bringauto::io_module_utils::ActionType::SET_ON));
        command.addOutputAction(bringauto::io_module_utils::OutputAction(bringauto::modules::io_module::OutputIndexes::OUTPUT_4, bringauto::io_module_utils::ActionType::SET_ON));
    }
    else if (input_states_all_false && no_button_presses) {
        command.addOutputAction(bringauto::io_module_utils::OutputAction(bringauto::modules::io_module::OutputIndexes::OUTPUT_1, bringauto::io_module_utils::ActionType::SET_OFF));
        command.addOutputAction(bringauto::io_module_utils::OutputAction(bringauto::modules::io_module::OutputIndexes::OUTPUT_2, bringauto::io_module_utils::ActionType::SET_OFF));
        command.addOutputAction(bringauto::io_module_utils::OutputAction(bringauto::modules::io_module::OutputIndexes::OUTPUT_3, bringauto::io_module_utils::ActionType::SET_OFF));
        command.addOutputAction(bringauto::io_module_utils::OutputAction(bringauto::modules::io_module::OutputIndexes::OUTPUT_4, bringauto::io_module_utils::ActionType::SET_OFF));
    }
    else if(input_states_all_false && !no_button_presses) {
        if(status.getButtonPresses() < 4) {
            command.addOutputAction(bringauto::io_module_utils::OutputAction(bringauto::modules::io_module::OutputIndexes::OUTPUT_1 + (status.getButtonPresses() - 1),
                                   bringauto::io_module_utils::ActionType::SET_ON));
        }
        else {
            command.addOutputAction(bringauto::io_module_utils::OutputAction(bringauto::modules::io_module::OutputIndexes::OUTPUT_4, bringauto::io_module_utils::ActionType::SET_ON));
        }
    }

    return command;
}

int arduino_opta_forward_status(const buffer device_status, const device_identification device, io_module_utils::context *context) {
    bringauto::io_module_utils::DeviceStatus<bringauto::modules::io_module::ARDUINO_OPTA_INPUTS_COUNT,
                                             bringauto::modules::io_module::ARDUINO_OPTA_OUTPUTS_COUNT> status;
    
    if(status.deserializeFromBuffer(device_status) == NOT_OK) {
        return NOT_OK;
    }

    auto command = arduino_opta_generate_command(status);
    {
        std::lock_guard lock(context->mutex);
        context->command_vector.emplace_back(command, bringauto::io_module_utils::DeviceID(device.module, device.device_type, device.priority, device.device_role, device.device_name));
    }
    context->con_variable.notify_one();

    return OK;
}

}
