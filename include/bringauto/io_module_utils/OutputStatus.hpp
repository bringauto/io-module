#pragma once

namespace bringauto::io_module_utils {

/**
 * @brief Class, which represents status of the output.
*/
class OutputStatus {
    /// @brief true, if error on output has occurred
    bool outputError = false;

	/// @brief represents state of output (true, if output is turned on)
	bool outputState = false;
    
    public:
        OutputStatus() = default;

        /**
         * @brief Sets the outputError and outputState attributes based on given values.  
        */
        void setValues(bool output_error, bool output_state);

        const bool &getOutputError() const { return outputError; }

        const bool &getOutputState() const { return outputState; }
};

}
