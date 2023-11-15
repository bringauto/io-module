#include <bringauto/io_module_utils/DeviceCommand.hpp>
#include <bringauto/io_module_utils/SerializationUtils.hpp>
#include <memory_management.h>
#include <general_error_codes.h>

#include <nlohmann/json.hpp>

namespace bringauto::io_module_utils {

using json = nlohmann::ordered_json;

void DeviceCommand::addOutputAction(OutputAction output_action) {
    outputActionList.emplace_back(output_action);
}

int DeviceCommand::serializeToBuffer(buffer *command_buffer) {
    json j;
    int i = 0;

    for(auto const& action : this->getOutputActionList()) {
        j[i]["outNum"] = action.getOutputNumber();
        j[i]["actType"] = action.getActionType();
        i++;
    }

    if(SerializationUtils::json_to_buffer(j, command_buffer) == NOT_OK) {
        return NOT_OK;
    }

    return OK;
}

int DeviceCommand::deserializeFromBuffer(buffer command_buffer) {
    json command_json;
    char *buffer_data = static_cast<char*> (command_buffer.data);
    size_t buffer_size = command_buffer.size_in_bytes;

    try {
        command_json = json::parse(buffer_data, buffer_data + buffer_size);
    }
    catch(const json::parse_error&) {
        return NOT_OK;
    }

    try {
        for(int i = 0; i < command_json.size(); i++){
            this->addOutputAction(OutputAction(command_json.at(i).at("outNum"), command_json.at(i).at("actType")));
        }
    }
    catch(const json::exception&) {
        return NOT_OK;
    }

    return OK;
}

}

