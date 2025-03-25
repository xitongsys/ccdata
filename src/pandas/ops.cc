#include "pandas/ops.h"

namespace pandas {

/// @std::string specify
/// @param s 
/// @return 
template <>
bool isnan(const std::string& s)
{
    return false;
}
template <>
std::string to_string(const std::string& s)
{
    return s;
}

}