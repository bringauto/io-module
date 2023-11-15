#pragma once

#include <cstdint>

namespace bringauto::io_module_utils {

/// @brief Type of action for output
enum ActionType : uint32_t {
    // Sets the output off
    SET_OFF,
    /// Sets the output on
    SET_ON,
    // Sets the output in reverse according to its current state
    TOGGLE
};

}
