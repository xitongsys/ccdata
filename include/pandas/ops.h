#pragma once

#include <cmath>
#include <format>
#include <iostream>
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
    if (pandas::isnan<T>(v)) {
        return "NaN";
    }

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
    std::stringstream ss;
    if constexpr (id == 0) {
        ss << "(";
    }
    ss << to_string(std::get<0>(t)) + ")";
    return ss.str();
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

// std::string specify
template <>
bool isnan(const std::string& s);

template <>
std::string to_string(const std::string& s);

// some common operators: sum/max/min...
// IT is Iterator
// these functions are simple and directly

template <class T, class IT>
T sum(IT& it)
{
    if (!it.has_left()) {
        return nan<T>();
    }

    T s = 0;
    while (it.has_left()) {
        s += it.next();
    }
    return s;
}

template <class T, class IT>
T max(IT& it)
{
    if (!it.has_left()) {
        return nan<T>();
    }

    T mx = it.next();
    while (it.has_left()) {
        T v = it.next();
        if (v > mx) {
            mx = v;
        }
    }
    return mx;
}

template <class T, class IT>
T min(IT& it)
{
    if (!it.has_left()) {
        return nan<T>();
    }

    T mi = it.next();
    while (it.has_left()) {
        T v = it.next();
        if (v < mi) {
            mi = v;
        }
    }
    return mi;
}

template <class T, class IT>
int count(IT& it)
{
    if (!it.has_left()) {
        return 0;
    }

    int cnt = 0;
    while (it.has_left()) {
        T v = it.next();
        if (!pandas::isnan(v)) {
            cnt++;
        }
    }
    return cnt;
}

template <class T, class IT>
double mean(IT& it)
{
    double s = sum<T, IT>(it);
    double cnt = count(it);
    return s / cnt;
}

template <class T, class IT>
double var(IT& it)
{
    if (!it.has_left()) {
        return nan<decltype(it.next())>();
    }

    double mn = mean(it);
    double cnt = count(it);
    double s = 0;
    while (it.has_left()) {
        double v = it.next();
        s += (v - mn) * (v - mn);
    }
    return s / cnt;
}

template <class T, class IT>
double std(IT& it)
{
    double v = var(it);
    return std::pow(v, 0.5);
}

template <class T, class IT>
double median(IT& it)
{
    std::vector<T> vs;
    while (it.has_left()) {
        vs.push_back(it.next());
    }
    int n = vs.size();
    if (n == 0) {
        return pandas::nan<double>();
    }
    if (n & 1) {
        return vs[n / 2];
    } else {
        return ((double)vs[n / 2 - 1] + (double)vs[n / 2]) / 2;
    }
}

} // namespace pandas
