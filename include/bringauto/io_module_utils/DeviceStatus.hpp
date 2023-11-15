#pragma once

#include <bringauto/io_module_utils/OutputStatus.hpp>
#include <bringauto/io_module_utils/SerializationUtils.hpp>
#include <bringauto/modules/io_module/io_module.h>
#include <memory_management.h>
#include <general_error_codes.h>

#include <nlohmann/json.hpp>
#include <array>
#include <cstdint>
#include <iostream>

namespace bringauto::io_module_utils {

using json = nlohmann::ordered_json;

/**
 * @brief Class, which represents status of the IO module's device.
*/
template <int INPUTS_NUMBER, int OUTPUTS_NUMBER>class DeviceStatus {
    const int outputsCount = OUTPUTS_NUMBER;
    const int inputsCount = INPUTS_NUMBER;

    /// @brief Array of Device output states
    std::array<OutputStatus, OUTPUTS_NUMBER> outputStates;

    /// @brief number of presses of Device button
    uint32_t buttonPresses;

    /// @brief Array of Device input states
    std::array<bool, INPUTS_NUMBER> inputStates;

    public:
        /**
         * @brief Sets attributes to default values.  
        */
        DeviceStatus() {
            buttonPresses = 0;
            inputStates.fill(false);
        }

        const std::array<OutputStatus, OUTPUTS_NUMBER> &getOutputStates() const { return outputStates; }

        void setOutputState(uint8_t output_number, bool output_error, bool output_state) {
            if(output_number <= outputsCount && output_number != 0) {
                outputStates[output_number - 1].setValues(output_error, output_state);
            }
        }

        const uint32_t &getButtonPresses() const { return buttonPresses; }

        void setButtonPresses(uint32_t button_presses) {
            buttonPresses = button_presses;
        }

        const std::array<bool, INPUTS_NUMBER> &getInputStates() const { return inputStates; }

        void setInputState(uint8_t input_number, bool input_state) {
            if(input_number <= inputsCount && input_number != 0) {
                inputStates[input_number - 1] = input_state;
            }
        }

        /**
         * @brief Serializes this object into given buffer using json.
        */
        int serializeToBuffer(buffer *status_buffer) {
            json j;

            for(int i = 0; i < this->getInputStates().size(); i++) {
                j[modules::io_module::StatusMessageJsonIndex::INPUT_STATES_INDEX][i]["inNum"] = i + 1;
                j[modules::io_module::StatusMessageJsonIndex::INPUT_STATES_INDEX][i]["inSt"] = inputStates[i];
            }

            for(int i = 0; i < this->getOutputStates().size(); i++) {
                j[modules::io_module::StatusMessageJsonIndex::OUTPUT_STATES_INDEX][i]["outNum"] = i + 1;
                j[modules::io_module::StatusMessageJsonIndex::OUTPUT_STATES_INDEX][i]["outErr"] = outputStates[i].getOutputError();
                j[modules::io_module::StatusMessageJsonIndex::OUTPUT_STATES_INDEX][i]["outSt"] = outputStates[i].getOutputState();
            }

            j[modules::io_module::BUTTON_PRESSES_INDEX]["butPr"] = this->getButtonPresses();

            if(SerializationUtils::json_to_buffer(j, status_buffer) == NOT_OK) {
                return NOT_OK;
            }

            return OK;
        }

        /**
         * @brief Deserializes status in buffer (json string in data) and sets this object.
         * @return OK - serialization successful, NOT_OK - serialization failed
        */
        int deserializeFromBuffer(buffer status_buffer) {
            json status_json;
            char *buffer_data = static_cast<char*> (status_buffer.data);
            size_t buffer_size = status_buffer.size_in_bytes;

            try {
                status_json = json::parse(buffer_data, buffer_data + buffer_size);
            }
            catch(const json::parse_error&) {
                return NOT_OK;
            }

            try {
                for(int i = 0; i < this->getInputStates().size(); i++){
                    this->setInputState(status_json.at(modules::io_module::StatusMessageJsonIndex::INPUT_STATES_INDEX).at(i).at("inNum"),
                                        status_json.at(modules::io_module::StatusMessageJsonIndex::INPUT_STATES_INDEX).at(i).at("inSt"));
                }

                for(int i = 0; i < this->getOutputStates().size(); i++){
                    this->setOutputState(status_json.at(modules::io_module::StatusMessageJsonIndex::OUTPUT_STATES_INDEX).at(i).at("outNum"),
                                         status_json.at(modules::io_module::StatusMessageJsonIndex::OUTPUT_STATES_INDEX).at(i).at("outErr"),
                                         status_json.at(modules::io_module::StatusMessageJsonIndex::OUTPUT_STATES_INDEX).at(i).at("outSt"));
                }

                this->setButtonPresses(status_json.at(modules::io_module::StatusMessageJsonIndex::BUTTON_PRESSES_INDEX).at("butPr"));
            }
            catch(const json::exception&) {
                return NOT_OK;
            }

            return OK;
        }
};

}
