#ifndef CHICKEN3421_ERROR_UTILS_HPP
#define CHICKEN3421_ERROR_UTILS_HPP

#include <string>

namespace chicken3421 {
    void expect(bool condition, const std::string &on_error);
}

#endif //CHICKEN3421_ERROR_UTILS_HPP
