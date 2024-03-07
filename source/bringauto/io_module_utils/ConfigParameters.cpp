#include <bringauto/io_module_utils/ConfigParameters.hpp>
#include <fleet_protocol/module_maintainer/external_server/external_server_structures.h>

#include <vector>
#include <string>

namespace bringauto::io_module_utils {

ConfigParameters::ConfigParameters(config config_data) {
    for(int i = 0; i < config_data.size; i++) {
        std::string_view key(static_cast<char*> (config_data.parameters[i].key.data));
        std::string_view value(static_cast<char*> (config_data.parameters[i].key.data));
        parameters.emplace_back(key, value);
    }
}

const std::string_view ConfigParameters::getValue(const char* key) const {
    for(auto i : parameters){
        if(i.first == key) {
            return i.second;
        }
    }
    return "";
}

}