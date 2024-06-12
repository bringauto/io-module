#pragma once

#include <fleet_protocol/common_headers/memory_management.h>

namespace bringauto::modules::io_module::devices::button {

// These functions are implementing function defined in module_manager.h specifically for Arduino Uno device

int button_send_status_condition(const buffer current_status, const buffer new_status);

int button_generate_command(buffer *generated_command, const buffer new_status, const buffer current_status, const buffer current_command);

int button_aggregate_status(buffer *aggregated_status, const buffer current_status, const buffer new_status);

int button_aggregate_error(buffer *error_message, const buffer current_error_message, const buffer status);

int button_generate_first_command(buffer *default_command);

int button_status_data_valid(const buffer status);

int button_command_data_valid(const buffer command);

}
