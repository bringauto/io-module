#include <bringauto/io_module_utils/DeviceError.hpp>

namespace bringauto::io_module_utils {

DeviceError::DeviceError(bool error_occured) {
    errorOccurred = error_occured;
}

}
