#pragma once

namespace bringauto::modules::io_module {

/// @brief Module identification number as described in Fleet protocol
static const int IO_MODULE_NUMBER = 3;

// Arduino Opta specific values

/// @brief Arduino Opta identification as described in Fleet protocol
static const int ARDUINO_OPTA_DEVICE_TYPE = 1;
/// @brief Number of inputs on Arduino Opta
static const int ARDUINO_OPTA_INPUTS_COUNT = 6;
/// @brief Number of outputs on Arduino Opta
static const int ARDUINO_OPTA_OUTPUTS_COUNT = 4;

// Arduino Mega specific values

/// @brief Arduino Mega identification as described in Fleet protocol
static const int ARDUINO_MEGA_DEVICE_TYPE = 2;
/// @brief Number of inputs on Arduino Mega
static const int ARDUINO_MEGA_INPUTS_COUNT = 4;
/// @brief Number of outputs on Arduino Mega
static const int ARDUINO_MEGA_OUTPUTS_COUNT = 4;

// Arduino Uno specific values

/// @brief Arduino Uno identification as described in Fleet protocol
static const int ARDUINO_UNO_DEVICE_TYPE = 3;
/// @brief Number of inputs on Arduino Uno
static const int ARDUINO_UNO_INPUTS_COUNT = 6;
/// @brief Number of outputs on Arduino Uno
static const int ARDUINO_UNO_OUTPUTS_COUNT = 7;

/// @brief Indexes of outputs
enum OutputIndexes { OUTPUT_1, OUTPUT_2, OUTPUT_3, OUTPUT_4, OUTPUT_5, OUTPUT_6, OUTPUT_7, OUTPUT_8,
                     OUTPUT_9, OUTPUT_10, OUTPUT_11, OUTPUT_12, OUTPUT_13, OUTPUT_14, OUTPUT_15, OUTPUT_16 };

/// @brief Indexes of inputs
enum InputIndexes { INPUT_1, INPUT_2, INPUT_3, INPUT_4, INPUT_5, INPUT_6, INPUT_7, INPUT_8,
                    INPUT_9, INPUT_10, INPUT_11, INPUT_12, INPUT_13, INPUT_14, INPUT_15, INPUT_16 };

/// @brief Indexes of json arrays in Status message
enum StatusMessageJsonIndex { INPUT_STATES_INDEX, OUTPUT_STATES_INDEX, BUTTON_PRESSES_INDEX };

}
