#include <external_server_structures.h>

#include <vector>
#include <string>

namespace bringauto::io_module_utils {

class ConfigParameters {
    std::vector<std::pair<std::string_view, std::string_view>> parameters;

public:
    ConfigParameters(config config_data);

    const size_t getParametersCount() const { return parameters.size(); }

    /**
     * @brief Return value based on key. If there is no parameter with given key, then returns empty string.
    */
    const std::string_view getValue(const char* key) const;
};

}