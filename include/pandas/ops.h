#pragma once

#include <string>
#include <vector>

namespace pandas {

template <class T>
bool isnan(const T& v)
{
    return std::isnan(v);
}

template <class T>
std::string to_string(const T& v)
{
    if (isnan(v)) {
        return "nan";
    }
    return std::to_string(v);
}

} // namespace pandas
