#pragma once

#include <bringauto/io_module_utils/DeviceID.hpp>
#include <bringauto/io_module_utils/DeviceCommand.hpp>

#include <vector>
#include <condition_variable>
#include <thread>

namespace bringauto::io_module_utils {

struct context {
    std::vector<device_identification> devices;
    std::vector<std::pair<bringauto::io_module_utils::DeviceCommand, bringauto::io_module_utils::DeviceID>> command_vector;
    std::mutex mutex;
    std::condition_variable con_variable;
};

}