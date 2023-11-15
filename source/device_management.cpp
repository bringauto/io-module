#include <device_management.h>
#include <bringauto/modules/io_module/io_module.h>
#include <general_error_codes.h>

int get_module_number() {return bringauto::modules::io_module::IO_MODULE_NUMBER;}

int is_device_type_supported(unsigned int device_type) {
    switch (device_type) {
        case bringauto::modules::io_module::ARDUINO_OPTA_DEVICE_TYPE:
        case bringauto::modules::io_module::ARDUINO_MEGA_DEVICE_TYPE:
        case bringauto::modules::io_module::ARDUINO_UNO_DEVICE_TYPE:
            return OK;
        default:
            return NOT_OK;
    }
}
