#include <bringauto/io_module_utils/DeviceID.hpp>

namespace bringauto::io_module_utils {

DeviceID::DeviceID(int module, unsigned int device_type, unsigned int priority, const char* device_role, const char* device_name):
        device_role_(device_role),
        device_name_(device_name)
    {
        id_.device_type = device_type;
        id_.module = module;
        id_.priority = priority;
        id_.device_role.data          = const_cast<char*> (device_role_.c_str());
        id_.device_role.size_in_bytes = device_role_.size();
        id_.device_name.data          = const_cast<char*> (device_name_.c_str());
        id_.device_name.size_in_bytes = device_name_.size();
}

DeviceID::DeviceID(int module, unsigned int device_type, unsigned int priority, buffer device_role, buffer device_name):
        device_role_(static_cast<const char*> (device_role.data), device_role.size_in_bytes),
        device_name_(static_cast<const char*> (device_name.data), device_name.size_in_bytes)
    {
        id_.device_type = device_type;
        id_.module = module;
        id_.priority = priority;
        id_.device_role.data          = const_cast<char*> (device_role_.c_str());
        id_.device_role.size_in_bytes = device_role_.size();
        id_.device_name.data          = const_cast<char*> (device_name_.c_str());
        id_.device_name.size_in_bytes = device_name_.size();
}

DeviceID::DeviceID(DeviceID&& c):
    device_role_(std::move(c.device_role_)),
    device_name_(std::move(c.device_name_)),
    id_(c.id_)
{
    id_.device_role.data          = const_cast<char*> (device_role_.c_str());
    id_.device_role.size_in_bytes = device_role_.size();
    id_.device_name.data          = const_cast<char*> (device_name_.c_str());
    id_.device_name.size_in_bytes = device_name_.size();
}

}
