#include "pandas/ops.h"

namespace pandas {

template <>
bool isnan(const std::string& v)
{
    return false;
}

template <>
std::string to_string(const std::string& v)
{
    return v;
}
}