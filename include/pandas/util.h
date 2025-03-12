#pragma once

#include <vector>

namespace pandas {
std::vector<int> range(int b, int e, int d);

template <class T>
bool isna(const T& v)
{
    return v.isnan();
}

template <typename T, typename... Args>
auto remove_first_element(const std::tuple<T, Args...>& t)
{
    return std::apply([](const T&, const Args&... args) {
        return std::make_tuple(args...);
    },
        t);
}

} // namespace pandas
