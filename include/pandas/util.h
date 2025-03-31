#pragma once

#include <format>
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

/// @brief  remove specified level
/// @tparam T
/// @tparam remove_level
/// @tparam current_level
/// @param t
/// @return
template <int remove_level, int current_level, typename T>
auto _remove_element(const std::tuple<T>& t)
{
    return t;
}

template <int remove_level, int current_level, typename T, typename... Args>
auto _remove_element(const std::tuple<T, Args...>& t)
{
    if constexpr (remove_level == current_level) {
        return _remove_element<remove_level, current_level + 1, Args...>(remove_first_element(t));

    } else if constexpr (sizeof...(Args) == 1 && remove_level == current_level + 1) {
        return std::tuple(std::get<0>(t));

    } else {
        T v = std::get<0>(t);
        auto t_tail = _remove_element<remove_level, current_level + 1, Args...>(remove_first_element(t));
        return add_first_element(v, t_tail);
    }
}

template <int remove_level, typename... Args>
auto remove_element(const std::tuple<Args...>& t)
{
    if constexpr (remove_level + 1 > sizeof...(Args) || remove_level < 0) {
        PANDAS_THROW(std::format("illegal remove_level: {}", remove_level));
    } else if constexpr (sizeof...(Args) == 2) {
        return std::get<(1 ^ remove_level)>(t);
    } else {
        return _remove_element<remove_level, 0, Args...>(t);
    }
}

template <typename T>
struct is_tuple : std::false_type { };

template <typename... Args>
struct is_tuple<std::tuple<Args...>> : std::true_type { };

template <typename T>
inline constexpr bool is_tuple_v = is_tuple<T>::value;

} // namespace pandas
