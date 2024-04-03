#include <bringauto/modules/io_module/devices/button/button_module_manager.hpp>
#include <fleet_protocol/module_maintainer/module_gateway/module_manager.h>
#include <bringauto/modules/io_module/io_module.h>
#include <bringauto/io_module_utils/DeviceStatus.hpp>
#include <bringauto/io_module_utils/DeviceCommand.hpp>

namespace bringauto::modules::io_module::devices::button {

int button_send_status_condition(const buffer current_status, const buffer new_status){
    bringauto::io_module_utils::DeviceStatus<bringauto::modules::io_module::BUTTON_INPUTS_COUNT,
                                             bringauto::modules::io_module::BUTTON_OUTPUTS_COUNT> c_status;

    int rc1 = c_status.deserializeFromBuffer(current_status);

    bringauto::io_module_utils::DeviceStatus<bringauto::modules::io_module::BUTTON_INPUTS_COUNT,
                                             bringauto::modules::io_module::BUTTON_OUTPUTS_COUNT> n_status;

    int rc2 = n_status.deserializeFromBuffer(new_status);

    if(rc1 == NOT_OK || rc2 == NOT_OK) {
        return NOT_OK;
    }

    for(int i = 0; i < c_status.getInputStates().size(); i++) {
        if(c_status.getInputStates()[i] != n_status.getInputStates()[i]) {
            return OK;
        }
    }

    return CONDITION_NOT_MET;
}

int button_generate_command(buffer *generated_command, const buffer new_status, const buffer current_status, const buffer current_command){
    if(allocate(generated_command, current_command.size_in_bytes) ==  NOT_OK) {
        return NOT_OK;
    }
    std::memcpy(generated_command->data, current_command.data, generated_command->size_in_bytes);
    return OK;
}

int button_aggregate_status(buffer *aggregated_status, const buffer current_status, const buffer new_status){
    bringauto::io_module_utils::DeviceStatus<bringauto::modules::io_module::BUTTON_INPUTS_COUNT,
                                             bringauto::modules::io_module::BUTTON_OUTPUTS_COUNT> status;

    int rc = status.deserializeFromBuffer(new_status);

    if(rc == NOT_OK) {
        return NOT_OK;
    }

    return status.serializeToBuffer(aggregated_status);
}

int button_aggregate_error(buffer *error_message, const buffer current_error_message, const buffer status){
    return bringauto::io_module_utils::SerializationUtils::serialize_error(error_message, 0);
}

int button_generate_first_command(buffer *default_command){
    bringauto::io_module_utils::DeviceCommand command;

    if(command.serializeToBuffer(default_command) == NOT_OK) {
        return NOT_OK;
    }

    return OK;
}

int button_status_data_valid(const buffer status){
    bringauto::io_module_utils::DeviceStatus<bringauto::modules::io_module::BUTTON_INPUTS_COUNT,
                                             bringauto::modules::io_module::BUTTON_OUTPUTS_COUNT> status_object;

    return status_object.deserializeFromBuffer(status);
}

int button_command_data_valid(const buffer command){
    bringauto::io_module_utils::DeviceCommand command_object;

    return command_object.deserializeFromBuffer(command);
}

}
