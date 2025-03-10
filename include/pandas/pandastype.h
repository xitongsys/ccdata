#pragma once

#include <cmath>
#include <ostream>
#include <string>

#include "pandas/datetime.h"

namespace pandas {

template <class T>
struct PandasType {
    bool is_nan = true;
    T value;

    template <class T2>
    PandasType(const T2& v)
    {
        value = v;
        is_nan = false;
    }

    template <class T2>
    PandasType(const PandasType<T2>& v)
    {
        value = v.value;
        is_nan = v.is_nan;
    }

    PandasType(T v)
    {
        value = v;
        is_nan = false;
    }

    PandasType()
    {
        is_nan = true;
    }

    PandasType(const PandasType& v)
    {
        is_nan = v.is_nan;
        value = v.value;
    }

    PandasType(PandasType&& v)
    {
        is_nan = v.is_nan;
        value = std::move(v.value);
    }

    PandasType& operator=(const PandasType& v)
    {
        is_nan = v.is_nan;
        value = v.value;
        return *this;
    }

    PandasType& operator=(PandasType&& v)
    {
        is_nan = v.is_nan;
        value = std::move(v.value);
        return *this;
    }

    PandasType pow(double n) const
    {

        if (is_nan) {
            return PandasType<T>();
        }

        return PandasType<T>(std::pow(value, n));
    }

    PandasType abs() const
    {
        if (is_nan) {
            return PandasType<T>();
        }

        return PandasType<T>(abs(value));
    }

    template <class T>
    T astype() const
    {
        T res;
        res.is_nan = is_nan;
        res.value = value;
        return res;
    }

    template <>
    PandasType<std::string> astype() const
    {
        if (is_nan) {
            return PandasType<std::string>();
        }

        return PandasType<std::string>(std::to_string(value));
    }

    std::string to_string() const
    {
        if (is_nan) {
            return "nan";
        }

        return std::to_string(value);
    }

    friend std::ostream& operator<<(std::ostream& os, const PandasType& pt)
    {
        os << pt.to_string();
        return os;
    }

#include "pandas/pandastype_op.tcc"
};

using Bool = PandasType<bool>;
using Int = PandasType<int>;
using Long = PandasType<long long>;
using Float = PandasType<float>;
using Double = PandasType<double>;
using Str = PandasType<std::string>;
using Dt = PandasType<Datetime>;

enum PandasTypeId {
    Obj_,
    Bool_,
    Int_,
    Long_,
    Float_,
    Double_,
    Str_,
    Dt_,
};

template <typename T>
constexpr PandasTypeId pandas_type_to_id()
{
    if constexpr (std::is_same_v<T, Bool>) {
        return Bool_;
    } else if constexpr (std::is_same_v<T, Int>) {
        return Int_;
    } else if constexpr (std::is_same_v<T, Long>) {
        return Long_;
    } else if constexpr (std::is_same_v<T, Float>) {
        return Float_;
    } else if constexpr (std::is_same_v<T, Double>) {
        return Double_;
    } else if constexpr (std::is_same_v<T, Str>) {
        return Str_;
    } else if constexpr (std::is_same_v<T, Dt>) {
        return Dt_;
    } else {
        return Obj_;
    }
}
}