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
class MultiIndex;

template <size_t level>
class MultiIndex<level> { };

template <size_t level, class T, class... Ts>
class MultiIndex<level, T, Ts...> : public MultiIndex<level + 1, Ts...> {
public:
    MultiIndex(const Array<T>& vs, const Array<Ts>&... vss)
        : values(vs)
        , MultiIndex<level + 1, Ts...>(vss...)
    {
    }

    std::tuple<T, Ts...> iloc(int i)
    {
        const size_t n_tail = sizeof...(Ts);
        std::tuple<T> t(values.iloc(i));
        if constexpr (n_tail > 0) {
            std::tuple<Ts...> ts = MultiIndex<level + 1, Ts...>::iloc(i);
            return std::tuple_cat(t, ts);
        } else {
            return t;
        }
    }

    template <size_t>
    Array<T>& get_level_values() { }

    template <>
    Array<T>& get_level_values<level>()
    {
        return values;
    }

    Array<T> values;
};

template <class... Ts>
class MultiIndexA : public MultiIndex<0, Ts...> { };

}