#pragma once

#include <cmath>
#include <format>
#include <sstream>
#include <string>
#include <tuple>
#include <type_traits>

#include "pandas/util.h"

namespace pandas {

template <class T>
bool isnan(const T& v)
{
    return std::isnan(v);
}

template <class T>
T nan()
{
    if constexpr (std::is_same_v<T, double>) {
        return std::nan("");
    }
    if constexpr (std::is_same_v<T, float>) {
        return std::nanf();
    }

    throw std::format("type has no nan");
    return T {};
}

template <class T>
std::string to_string(const T& v)
{
    if constexpr (std::is_arithmetic_v<T>) {
        return std::to_string(v);

    } else {
        std::stringstream ss;
        ss << v;
        return ss.str();
    }
}

template <size_t id, class T>
std::string _to_string(const std::tuple<T>& t)
{
    return to_string(std::get<0>(t)) + ")";
}

template <size_t id, class T, class... Ts>
std::string _to_string(const std::tuple<T, Ts...>& t)
{
    std::stringstream ss;
    if constexpr (id == 0) {
        ss << "(";
    }
    ss << to_string(std::get<0>(t)) << "," << _to_string<id + 1, Ts...>(remove_first_element(t));
    return ss.str();
}

template <class... Ts>
std::string to_string(const std::tuple<Ts...>& t)
{
    return _to_string<0, Ts...>(t);
}

} // namespace pandas
