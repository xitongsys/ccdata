#pragma once

#include <vector>

namespace pandas {
std::vector<int> range(int b, int e, int d);

template <typename T, typename... Args>
auto remove_first_element(const std::tuple<T, Args...>& t)
{
    return std::apply([](const T&, const Args&... args) {
        return std::make_tuple(args...);
    },
        t);
}

} // namespace pandas
