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

template <class T, class... Ts>
auto add_first_element(const T& v, const std::tuple<Ts...>& t)
{
    return std::apply([&](const Ts&... args) { return std::make_tuple(v, args...); }, t);
}

template <typename T>
struct is_tuple : std::false_type { };

template <typename... Args>
struct is_tuple<std::tuple<Args...>> : std::true_type { };

template <typename T>
inline constexpr bool is_tuple_v = is_tuple<T>::value;

} // namespace pandas
