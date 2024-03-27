#include <fleet_protocol/module_maintainer/module_gateway/module_manager.h>
#include <bringauto/modules/io_module/io_module.h>
#include <bringauto/io_module_utils/SerializationUtils.hpp>
#include <bringauto/io_module_utils/DeviceStatus.hpp>
#include <bringauto/io_module_utils/DeviceCommand.hpp>
#include <bringauto/modules/io_module/devices/arduino_opta/arduino_opta_module_manager.hpp>
#include <bringauto/modules/io_module/devices/arduino_mega/arduino_mega_module_manager.hpp>
#include <bringauto/modules/io_module/devices/arduino_uno/arduino_uno_module_manager.hpp>
#include <bringauto/modules/io_module/devices/button/button_module_manager.hpp>

#include <cstring>

int send_status_condition(const buffer current_status, const buffer new_status, unsigned int device_type) {
    switch (device_type) {
        case bringauto::modules::io_module::ARDUINO_OPTA_DEVICE_TYPE:
            return bringauto::modules::io_module::devices::arduino_opta::arduino_opta_send_status_condition(current_status, new_status);
        case bringauto::modules::io_module::ARDUINO_MEGA_DEVICE_TYPE:
            return bringauto::modules::io_module::devices::arduino_mega::arduino_mega_send_status_condition(current_status, new_status);
        case bringauto::modules::io_module::ARDUINO_UNO_DEVICE_TYPE:
            return bringauto::modules::io_module::devices::arduino_uno::arduino_uno_send_status_condition(current_status, new_status);
        case bringauto::modules::io_module::BUTTON_DEVICE_TYPE:
            return bringauto::modules::io_module::devices::button::button_send_status_condition(current_status, new_status);
        default:
            return NOT_OK;
    }
}

int generate_command(buffer *generated_command, const buffer new_status, const buffer current_status, const buffer current_command, unsigned int device_type) {
    switch(device_type) {
        case bringauto::modules::io_module::ARDUINO_OPTA_DEVICE_TYPE:
            return bringauto::modules::io_module::devices::arduino_opta::arduino_opta_generate_command(generated_command, new_status, current_status, current_command);
        case bringauto::modules::io_module::ARDUINO_MEGA_DEVICE_TYPE:
            return bringauto::modules::io_module::devices::arduino_mega::arduino_mega_generate_command(generated_command, new_status, current_status, current_command);
        case bringauto::modules::io_module::ARDUINO_UNO_DEVICE_TYPE:
            return bringauto::modules::io_module::devices::arduino_uno::arduino_uno_generate_command(generated_command, new_status, current_status, current_command);
        case bringauto::modules::io_module::BUTTON_DEVICE_TYPE:
            return bringauto::modules::io_module::devices::button::button_generate_command(generated_command, new_status, current_status, current_command);
        default:
            return NOT_OK;
    }
}

int aggregate_status(buffer *aggregated_status, const buffer current_status, const buffer new_status, unsigned int device_type) {
    switch(device_type) {
        case bringauto::modules::io_module::ARDUINO_OPTA_DEVICE_TYPE:
            return bringauto::modules::io_module::devices::arduino_opta::arduino_opta_aggregate_status(aggregated_status, current_status, new_status);
        case bringauto::modules::io_module::ARDUINO_MEGA_DEVICE_TYPE:
            return bringauto::modules::io_module::devices::arduino_mega::arduino_mega_aggregate_status(aggregated_status, current_status, new_status);
        case bringauto::modules::io_module::ARDUINO_UNO_DEVICE_TYPE:
            return bringauto::modules::io_module::devices::arduino_uno::arduino_uno_aggregate_status(aggregated_status, current_status, new_status);
        case bringauto::modules::io_module::BUTTON_DEVICE_TYPE:
            return bringauto::modules::io_module::devices::button::button_aggregate_status(aggregated_status, current_status, new_status);
        default:
            return NOT_OK;
    }
}

int aggregate_error(buffer *error_message, const buffer current_error_message, const buffer status, unsigned int device_type) {
    switch(device_type) {
        case bringauto::modules::io_module::ARDUINO_OPTA_DEVICE_TYPE:
            return bringauto::modules::io_module::devices::arduino_opta::arduino_opta_aggregate_error(error_message, current_error_message, status);
        case bringauto::modules::io_module::ARDUINO_MEGA_DEVICE_TYPE:
            return bringauto::modules::io_module::devices::arduino_mega::arduino_mega_aggregate_error(error_message, current_error_message, status);
        case bringauto::modules::io_module::ARDUINO_UNO_DEVICE_TYPE:
            return bringauto::modules::io_module::devices::arduino_uno::arduino_uno_aggregate_error(error_message, current_error_message, status);
        case bringauto::modules::io_module::BUTTON_DEVICE_TYPE:
            return bringauto::modules::io_module::devices::button::button_aggregate_error(error_message, current_error_message, status);
        default:
            return NOT_OK;
    }
}

int generate_first_command(buffer *default_command, unsigned int device_type) {
    switch(device_type) {
        case bringauto::modules::io_module::ARDUINO_OPTA_DEVICE_TYPE:
            return bringauto::modules::io_module::devices::arduino_opta::arduino_opta_generate_first_command(default_command);
        case bringauto::modules::io_module::ARDUINO_MEGA_DEVICE_TYPE:
            return bringauto::modules::io_module::devices::arduino_mega::arduino_mega_generate_first_command(default_command);
        case bringauto::modules::io_module::ARDUINO_UNO_DEVICE_TYPE:
            return bringauto::modules::io_module::devices::arduino_uno::arduino_uno_generate_first_command(default_command);
        case bringauto::modules::io_module::BUTTON_DEVICE_TYPE:
            return bringauto::modules::io_module::devices::button::button_generate_first_command(default_command);
        default:
            return NOT_OK;
    }
}

int status_data_valid(const buffer status, unsigned int device_type) {
    switch(device_type) {
        case bringauto::modules::io_module::ARDUINO_OPTA_DEVICE_TYPE:
            return bringauto::modules::io_module::devices::arduino_opta::arduino_opta_status_data_valid(status);
        case bringauto::modules::io_module::ARDUINO_MEGA_DEVICE_TYPE:
            return bringauto::modules::io_module::devices::arduino_mega::arduino_mega_status_data_valid(status);
        case bringauto::modules::io_module::ARDUINO_UNO_DEVICE_TYPE:
            return bringauto::modules::io_module::devices::arduino_uno::arduino_uno_status_data_valid(status);
        case bringauto::modules::io_module::BUTTON_DEVICE_TYPE:
            return bringauto::modules::io_module::devices::button::button_status_data_valid(status);
        default:
            return NOT_OK;
    }
}

int command_data_valid(const buffer command, unsigned int device_type) {
     switch(device_type) {
        case bringauto::modules::io_module::ARDUINO_OPTA_DEVICE_TYPE:
            return bringauto::modules::io_module::devices::arduino_opta::arduino_opta_command_data_valid(command);
        case bringauto::modules::io_module::ARDUINO_MEGA_DEVICE_TYPE:
            return bringauto::modules::io_module::devices::arduino_mega::arduino_mega_command_data_valid(command);
        case bringauto::modules::io_module::ARDUINO_UNO_DEVICE_TYPE:
            return bringauto::modules::io_module::devices::arduino_uno::arduino_uno_command_data_valid(command);
        case bringauto::modules::io_module::BUTTON_DEVICE_TYPE:
            return bringauto::modules::io_module::devices::button::button_command_data_valid(command);
        default:
            return NOT_OK;
    }
}
