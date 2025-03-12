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
    using PARENT = _MultiIndex<level + 1, Ts...>;

    _MultiIndex()
        : PARENT()
    {
    }

    _MultiIndex(const Array<T>& vs, const Array<Ts>&... vss)
        : _values(vs)
        , PARENT(vss...)
    {
        if constexpr (level == 0) {
            update_index();
        }
    }

    void update_index()
    {
        value2iid.clear();
        for (int i = 0; i < size(); i++) {
            std::tuple<T, Ts...> key = iloc(i);
            if (value2iid.count(key) > 0) {
                throw std::format("MultiIndex has duplicated key");
            }
            int n = value2iid.size();
            value2iid[key] = n;
        }
    }

    int loc(const std::tuple<T, Ts...>& key)
    {
        if (!has(key)) {
            throw std::format("key not found");
        }
        return value2iid[key];
    }

    int loc(const T& k, const Ts&... ks)
    {
        std::tuple<T, Ts...> key(k, ks...);
        return loc(key);
    }

    bool has(const std::tuple<T, Ts...>& key) const
    {
        return value2iid.count(key) > 0;
    }

    bool has(const T& k, const Ts&... ks) const
    {
        std::tuple<T, Ts...> key(k, ks...);
        return value2iid.count(key) > 0;
    }

    int append_values(const T& k, const Ts&... ks)
    {
        if constexpr (level == 0) {
            if (has(k, ks...)) {
                return -1;
            }
        }

        _values.append(k);

        constexpr size_t N_TAIL = sizeof...(Ts);
        if constexpr (N_TAIL > 0) {
            PARENT::append_values(ks...);
        }

        std::tuple<T, Ts...> key(k, ks...);
        int n = value2iid.size();
        value2iid[key] = n;

        return 0;
    }

    int append_key(const std::tuple<T, Ts...>& key)
    {
        if constexpr (level == 0) {
            if (has(key)) {
                return -1;
            }
        }

        T k = std::get<0>(key);
        _values.append(k);

        constexpr size_t N_TAIL = sizeof...(Ts);
        if constexpr (N_TAIL > 0) {
            PARENT::append_key(remove_first_element(key));
        }

        return 0;
    }

    _MultiIndex<level, T, Ts...> iloc(int bgn, int end, int step = 1)
    {
        _MultiIndex<level, T, Ts...> mi;
        for (int i = bgn; i < end; i += step) {
            std::tuple<T, Ts...> key = iloc(i);
            mi.append_key(key);
        }
        return mi;
    }

    _MultiIndex<level, T, Ts...> loc(const std::tuple<T, Ts...>& bgn, const std::tuple<T, Ts...>& end)
    {
        _MultiIndex<level, T, Ts...> mi;
        auto up = value2iid.upper_bound(end);
        for (auto it = value2iid.lower_bound(bgn); it != up & it != value2iid.end(); it++) {
            std::tuple<T, Ts...> key = iloc(it->second);
            mi.append_key(key);
        }
        return mi;
    }

    std::tuple<T&, Ts&...> iloc(int i)
    {
        constexpr size_t N_TAIL = sizeof...(Ts);
        std::tuple<T&> t(_values.iloc(i));
        if constexpr (N_TAIL > 0) {
            std::tuple<Ts&...> ts = PARENT::iloc(i);
            return std::tuple_cat(t, ts);
        } else {
            return t;
        }
    }

    std::tuple<Array<T>&, Array<Ts>&...> values()
    {
        constexpr size_t N_TAIL = sizeof...(Ts);
        std::tuple<Array<T>&> v(_values);
        if constexpr (N_TAIL > 0) {
            std::tuple<Array<Ts>&...> vs = PARENT::values();
            return std::tuple_cat(v, vs);

        } else {
            return v;
        }
    }

    template <size_t l, class T2>
    Array<T2>& get_level_values()
    {
        auto vs = values();
        return std::get<l>(vs);
    }

    size_t size() const
    {
        return _values.size();
    }

    Array<T> _values;

    std::map<std::tuple<T, Ts...>, int> value2iid;

    std::string to_string() const
    {
        constexpr size_t N_TAIL = sizeof...(Ts);
        std::stringstream ss;

        if constexpr (level == 0) {
            ss << "MultiIndex: {\n";
        }

        ss << _values.to_string() << "\n";
        if constexpr (N_TAIL > 0) {
            ss << PARENT::to_string();
        }

        if constexpr (level == 0) {
            ss << "}";
        }

        return ss.str();
    }
};

template <class... Ts>
class MultiIndex : public _MultiIndex<0, Ts...> {
public:
    using PARENT = _MultiIndex<0, Ts...>;
    MultiIndex()
        : _MultiIndex<0, Ts...>()
    {
    }

    MultiIndex(const PARENT& p)
        : PARENT(p)
    {
    }

    MultiIndex(const Array<Ts>&... vss)
        : _MultiIndex<0, Ts...>(vss...)
    {
    }

    MultiIndex<Ts...> iloc(int bgn, int end, int step = 1)
    {
        _MultiIndex<0, Ts...> mi = PARENT::iloc(bgn, end, step);
        return MultiIndex<Ts...>(mi);
    }

    std::tuple<Ts&...> iloc(int i)
    {
        return PARENT::iloc(i);
    }

    MultiIndex<Ts...> loc(const std::tuple<Ts...>& bgn, const std::tuple<Ts...>& end)
    {
        _MultiIndex<0, Ts...> mi = PARENT::loc(bgn, end);
        return MultiIndex<Ts...>(mi);
    }

    int loc(const std::tuple<Ts...>& key)
    {
        return PARENT::loc(key);
    }

    int loc(const Ts... vs)
    {
        return PARENT::loc(vs...);
    }

    friend std::ostream& operator<<(std::ostream& os, const MultiIndex<Ts...>& mi)
    {
        os << mi.to_string();
        return os;
    }
};

template <class... Ts>
MultiIndex<Ts...> concat(const MultiIndex<Ts...>& ma, const MultiIndex<Ts...>& mb)
{
    MultiIndex<Ts...> mc = ma;
    for (int i = 0; i < mb.size(); i++) {
        std::tuple<Ts...> key = mb.iloc(i);
        if (ma.has(key)) {
            continue;
        }
        mc.append_key(key);
    }
    return mc;
}

}