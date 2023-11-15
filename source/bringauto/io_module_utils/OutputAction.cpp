#include <bringauto/io_module_utils/OutputAction.hpp>

namespace bringauto::io_module_utils {

OutputAction::OutputAction(uint8_t output_number, ActionType action_type) {
    outputNumber = output_number;
    actionType = action_type;
}

}
