#pragma once

#include <vector>

namespace pandas {
std::vector<int> range(int b, int e, int d);

template <class T>
bool isna(const T& v)
{
    return v.is_nan;
}

} // namespace pandas
