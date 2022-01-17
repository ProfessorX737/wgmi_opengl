#include <stdexcept>

#include <chicken3421/error_utils.hpp>

namespace chicken3421 {
    void expect(bool condition, const std::string &on_error) {
        if (!condition) {
            throw std::runtime_error(on_error);
        } else {
            // life goes on
        }
    }
}
