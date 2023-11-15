#pragma once

#include <bringauto/io_module_utils/OutputAction.hpp>
#include <memory_management.h>

#include <list>
#include <cstdint>

namespace bringauto::io_module_utils {

/**
 * @brief Class, which represents command for IO module's device.
*/
class DeviceCommand {
    /// @brief List of output actions to execute by command
    std::list<OutputAction> outputActionList;

    public:
        DeviceCommand() {}

        const std::list<OutputAction> &getOutputActionList() const { return outputActionList; }

        /**
         * @brief Adds the given output action to the end of list of output actions.
        */
        void addOutputAction(OutputAction output_action);

        /**
         * @brief Serializes this object into given buffer using json.
         * @return OK - serialization successful, NOT_OK - serialization failed
        */
        int serializeToBuffer(buffer *command_buffer);

        /**
         * @brief Deserializes command in buffer (json string in data) and sets this object.
         * @return OK - deserialization successful, NOT_OK - deserialization failed
        */
        int deserializeFromBuffer(buffer command_buffer);
};

}
