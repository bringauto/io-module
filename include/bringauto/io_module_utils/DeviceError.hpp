#pragma once

#include <memory_management.h>

namespace bringauto::io_module_utils {

/**
 * @brief Class, which represents error of the IO module's device.
*/
class DeviceError {
	bool errorOccurred;

    public:
        /**
         * @brief Sets the errorOccurred attribute based on given value.  
        */
        DeviceError(bool error_occured);
        
        const bool &getErrorOccured() const { return errorOccurred; }
};

}
