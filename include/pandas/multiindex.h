#pragma once

#include <functional>
#include <iostream>
#include <list>
#include <map>
#include <optional>
#include <vector>

#include "pandas/array.h"
#include "pandas/index.h"
#include "pandas/pandastype.h"
#include "pandas/util.h"

namespace pandas {

template <class T>
class Index;

template <size_t, class... Ts>
class _MultiIndex;

template <size_t level>
class _MultiIndex<level> { };

template <size_t level, class T, class... Ts>
class _MultiIndex<level, T, Ts...> : public _MultiIndex<level + 1, Ts...> {
public:
    Array<T> _values;
    std::map<std::tuple<T, Ts...>, int> value2iid;

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

    std::string name() const
    {
        std::string nm = _values.name;
        constexpr size_t N_TAIL = sizeof...(Ts);
        if constexpr (N_TAIL > 0) {
            nm += "," + PARENT::name();
        }
        return nm;
    }

    template <class NM, class... NMS>
    void rename(const NM& nm, const NMS&... nms)
    {
        _values.name = nm;
        constexpr size_t N_TAIL = sizeof...(Ts);
        if constexpr (N_TAIL > 0) {
            PARENT::rename(nms...);
        }
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

    _MultiIndex<level, T, Ts...> sort(bool ascending = true) const
    {
        _MultiIndex<level, T, Ts...> mi;
        if (ascending) {
            for (auto it = value2iid.begin(); it != value2iid.end(); it++) {
                mi.append_key(it->first);
            }

        } else {
            for (auto it = value2iid.rbegin(); it != value2iid.rend(); it++) {
                mi.append_key(it->first);
            }
        }
        return mi;
    }

    int append_values(const T& k, const Ts&... ks)
    {
        if constexpr (level == 0) {
            if (has(k, ks...)) {
                return -1;
            }
            std::tuple<T, Ts...> key(k, ks...);
            int n = value2iid.size();
            value2iid[key] = n;
        }

        _values.append(k);

        constexpr size_t N_TAIL = sizeof...(Ts);
        if constexpr (N_TAIL > 0) {
            PARENT::append_values(ks...);
        }

        return 0;
    }

    int append_key(const std::tuple<T, Ts...>& key)
    {
        if constexpr (level == 0) {
            if (has(key)) {
                return -1;
            }
            int n = value2iid.size();
            value2iid[key] = n;
        }

        T k = std::get<0>(key);
        _values.append(k);

        constexpr size_t N_TAIL = sizeof...(Ts);
        if constexpr (N_TAIL > 0) {
            PARENT::append_key(remove_first_element(key));
        }

        return 0;
    }

    void clear()
    {
        _values.clear();
        value2iid.clear();
        constexpr size_t N_TAIL = sizeof...(Ts);
        if constexpr (N_TAIL > 0) {
            PARENT::clear();
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

    std::vector<int> loc(const std::tuple<T, Ts...>& bgn, const std::tuple<T, Ts...>& end)
    {
        std::vector<int> ids;
        auto up = value2iid.upper_bound(end);
        for (auto it = value2iid.lower_bound(bgn); it != up & it != value2iid.end(); it++) {
            ids.push_back(it->second);
        }
        return ids;
    }

    std::tuple<T, Ts...> iloc(int i)
    {
        constexpr size_t N_TAIL = sizeof...(Ts);
        std::tuple<T> t(_values.iloc(i));
        if constexpr (N_TAIL > 0) {
            std::tuple<Ts...> ts = PARENT::iloc(i);
            return std::tuple_cat(t, ts);
        } else {
            return t;
        }
    }

    std::vector<std::tuple<T, Ts...>> iloc(int bgn, int end, int step = 1)
    {
        std::vector<std::tuple<T, Ts...>> vs;
        for (int i = bgn; i < end; i += step) {
            std::tuple<T, Ts...> key = iloc(i);
            vs.push_back(key);
        }
        return vs;
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

    std::tuple<T, Ts...> min()
    {
        if (value2iid.size() == 0) {
            throw std::format("Empty MultiIndex");
        }
        return value2iid.begin()->first;
    }

    std::tuple<T, Ts...> max()
    {
        if (value2iid.size() == 0) {
            throw std::format("Empty MultiIndex");
        }
        return value2iid.rbegin()->first;
    }
};

template <class... Ts>
class MultiIndex : public _MultiIndex<0, Ts...>, public Index<std::tuple<Ts...>> {
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

    std::string name() const
    {
        return PARENT::name();
    }

    size_t size() const
    {
        return PARENT::size();
    }

    void update_index()
    {
        PARENT::update_index();
    }

    bool has(const std::tuple<Ts...>& key) const
    {
        return PARENT::has(key);
    }

    int append(const std::tuple<Ts...>& key)
    {
        return PARENT::append_key(key);
    }

    void clear()
    {
        PARENT::clear();
    }

    std::tuple<Ts...> iloc(int i)
    {
        return PARENT::iloc(i);
    }

    std::vector<std::tuple<Ts...>> iloc(int bgn, int end, int step = 1)
    {
        return PARENT::iloc(bgn, end, step);
    }

    int loc(const std::tuple<Ts...>& key)
    {
        return PARENT::loc(key);
    }

    int loc(const Ts... vs)
    {
        return PARENT::loc(vs...);
    }

    std::vector<int> loc(const std::tuple<Ts...>& bgn, const std::tuple<Ts...>& end)
    {
        return PARENT::loc(bgn, end);
    }

    std::shared_ptr<Index<std::tuple<Ts...>>> clone() const
    {
        auto ptr = std::make_shared<MultiIndex<Ts...>>(*this);
        return ptr;
    }

    std::shared_ptr<Index<std::tuple<Ts...>>> new_empty() const
    {
        auto ptr = clone();
        ptr->clear();
        return ptr;
    }

    std::string to_string() const
    {
        return PARENT::to_string();
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