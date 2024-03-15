#pragma once

#include <fleet_protocol/common_headers/memory_management.h>

namespace bringauto::modules::io_module::devices::arduino_opta {

// These functions are implementing function defined in module_manager.h specifically for Arduino Opta device

int arduino_opta_send_status_condition(const buffer current_status, const buffer new_status);

int arduino_opta_generate_command(buffer *generated_command, const buffer new_status, const buffer current_status, const buffer current_command);

int arduino_opta_aggregate_status(buffer *aggregated_status, const buffer current_status, const buffer new_status);

int arduino_opta_aggregate_error(buffer *error_message, const buffer current_error_message, const buffer status);

int arduino_opta_generate_first_command(buffer *default_command);

int arduino_opta_status_data_valid(const buffer status);

int arduino_opta_command_data_valid(const buffer command);

}
