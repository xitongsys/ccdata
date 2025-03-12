#pragma once

#include <functional>
#include <iostream>
#include <list>
#include <map>
#include <optional>
#include <vector>

#include "pandas/array.h"
#include "pandas/pandastype.h"
#include "pandas/util.h"

namespace pandas {

template <size_t, class... Ts>
class _MultiIndex;

template <size_t level>
class _MultiIndex<level> { };

template <size_t level, class T, class... Ts>
class _MultiIndex<level, T, Ts...> : public _MultiIndex<level + 1, Ts...> {
public:
    _MultiIndex(const Array<T>& vs, const Array<Ts>&... vss)
        : values(vs)
        , _MultiIndex<level + 1, Ts...>(vss...)
    {
    }

    std::tuple<T, Ts...> iloc(int i)
    {
        const size_t n_tail = sizeof...(Ts);
        std::tuple<T> t(values.iloc(i));
        if constexpr (n_tail > 0) {
            std::tuple<Ts...> ts = _MultiIndex<level + 1, Ts...>::iloc(i);
            return std::tuple_cat(t, ts);
        } else {
            return t;
        }
    }

    Array<T> values;
};

template <class... Ts>
class MultiIndex : public _MultiIndex<0, Ts...> {
public:
    MultiIndex(const Array<Ts>&... vss)
        : _MultiIndex<0, Ts...>(vss...)
    {
    }
};

}