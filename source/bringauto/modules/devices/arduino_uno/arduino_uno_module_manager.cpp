#include <bringauto/modules/io_module/devices/arduino_uno/arduino_uno_module_manager.hpp>
#include <fleet_protocol/module_maintainer/module_gateway/module_manager.h>
#include <bringauto/modules/io_module/io_module.h>
#include <bringauto/io_module_utils/DeviceStatus.hpp>
#include <bringauto/io_module_utils/DeviceCommand.hpp>

namespace bringauto::modules::io_module::devices::arduino_uno {

int arduino_uno_send_status_condition(const buffer current_status, const buffer new_status) {
    bringauto::io_module_utils::DeviceStatus<bringauto::modules::io_module::ARDUINO_UNO_INPUTS_COUNT,
                                             bringauto::modules::io_module::ARDUINO_UNO_OUTPUTS_COUNT> c_status;

    int rc1 = c_status.deserializeFromBuffer(current_status);

    bringauto::io_module_utils::DeviceStatus<bringauto::modules::io_module::ARDUINO_UNO_INPUTS_COUNT,
                                             bringauto::modules::io_module::ARDUINO_UNO_OUTPUTS_COUNT> n_status;

    int rc2 = n_status.deserializeFromBuffer(new_status);

    if(rc1 == NOT_OK || rc2 == NOT_OK) {
        return NOT_OK;
    }

    for(int i = 0; i < c_status.getOutputStates().size(); i++) {
        if(c_status.getOutputStates()[i].getOutputError() != n_status.getOutputStates()[i].getOutputError() ||
            c_status.getOutputStates()[i].getOutputState() != n_status.getOutputStates()[i].getOutputState()) {
            return OK;
        }
    }
    for(int i = 0; i < c_status.getInputStates().size(); i++) {
        if(c_status.getInputStates()[i] != n_status.getInputStates()[i]) {
            return OK;
        }
    }

    return CONDITION_NOT_MET;
}

int arduino_uno_generate_command(buffer *generated_command, const buffer new_status, const buffer current_status,
                                const buffer current_command) {
    if(allocate(generated_command, current_command.size_in_bytes) ==  NOT_OK) {
        return NOT_OK;
    }
    std::memcpy(generated_command->data, current_command.data, generated_command->size_in_bytes);
    return OK;
}

int arduino_uno_aggregate_status(buffer *aggregated_status, const buffer current_status, const buffer new_status) {
    bringauto::io_module_utils::DeviceStatus<bringauto::modules::io_module::ARDUINO_UNO_INPUTS_COUNT,
                                             bringauto::modules::io_module::ARDUINO_UNO_OUTPUTS_COUNT> status;

    int rc = status.deserializeFromBuffer(new_status);

    if(rc == NOT_OK) {
        return NOT_OK;
    }

    return status.serializeToBuffer(aggregated_status);
}

int arduino_uno_aggregate_error(buffer *error_message, const buffer current_error_message, const buffer status) {
    if(current_error_message.data == nullptr){ // In this case, there is no current error message in error aggregator - creating default error
        bringauto::io_module_utils::DeviceStatus<bringauto::modules::io_module::ARDUINO_UNO_INPUTS_COUNT,
                                             bringauto::modules::io_module::ARDUINO_UNO_OUTPUTS_COUNT> status_object;

        int rc = status_object.deserializeFromBuffer(status);
        if(rc == NOT_OK) {
            return NOT_OK;
        }
        return bringauto::io_module_utils::SerializationUtils::serialize_error(error_message, status_object.getButtonPresses());
    }

    int aggregated_button_presses = bringauto::io_module_utils::SerializationUtils::deserialize_error(static_cast<char*> (current_error_message.data), current_error_message.size_in_bytes);

    bringauto::io_module_utils::DeviceStatus<bringauto::modules::io_module::ARDUINO_UNO_INPUTS_COUNT,
                                             bringauto::modules::io_module::ARDUINO_UNO_OUTPUTS_COUNT> status_object;

    int rc = status_object.deserializeFromBuffer(status);

    if(aggregated_button_presses < 0 || rc == NOT_OK) {
        return NOT_OK;
    }

    return bringauto::io_module_utils::SerializationUtils::serialize_error(error_message, aggregated_button_presses + status_object.getButtonPresses());
}

int arduino_uno_generate_first_command(buffer *default_command) {
    bringauto::io_module_utils::DeviceCommand command;
    for(int i = bringauto::modules::io_module::OutputIndexes::OUTPUT_1; i <= bringauto::modules::io_module::OutputIndexes::OUTPUT_4; i++) {
        command.addOutputAction(bringauto::io_module_utils::OutputAction(i, bringauto::io_module_utils::ActionType::SET_OFF));
    }

    if(command.serializeToBuffer(default_command) == NOT_OK) {
        return NOT_OK;
    }

    return OK;
}

int arduino_uno_status_data_valid(const buffer status) {
    bringauto::io_module_utils::DeviceStatus<bringauto::modules::io_module::ARDUINO_UNO_INPUTS_COUNT,
                                             bringauto::modules::io_module::ARDUINO_UNO_OUTPUTS_COUNT> status_object;

    return status_object.deserializeFromBuffer(status);
}

int arduino_uno_command_data_valid(const buffer command) {
    bringauto::io_module_utils::DeviceCommand command_object;

    return command_object.deserializeFromBuffer(command);
}

}
