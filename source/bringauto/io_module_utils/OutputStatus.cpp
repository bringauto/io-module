#include <bringauto/io_module_utils/OutputStatus.hpp>

namespace bringauto::io_module_utils {

void OutputStatus::setValues(bool output_error, bool output_state) {
    outputError = output_error;
    outputState = output_state;
}



}
