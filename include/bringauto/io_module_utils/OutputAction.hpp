#pragma once

#include <bringauto/io_module_utils/ActionType.hpp>

#include <cstdint>

namespace bringauto::io_module_utils {

/**
 * @brief Class, which represents command action for an output.
*/
class OutputAction {
    /// Number of affected output
    uint8_t outputNumber;
    /// Type of action
	ActionType actionType;

    public:
        /**
         * @brief Sets the outputNumber and actionType attributes based on given values.
        */
        OutputAction(uint8_t output_number, ActionType action_type);

        const uint8_t &getOutputNumber() const { return outputNumber; }

        const ActionType &getActionType() const { return actionType; }
};

}
