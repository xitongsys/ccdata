#pragma once

#include <algorithm>
#include <format>
#include <functional>
#include <iostream>
#include <list>
#include <map>
#include <optional>
#include <vector>

#include "pandas/array.h"
#include "pandas/iterator.h"
#include "pandas/ops.h"
#include "pandas/range.h"
#include "pandas/util.h"

namespace pandas {

template <class T, class NT = std::string>
class Index {
public:
    Array<T, NT> values;
    std::vector<std::pair<T, int>> value2iid;

#include "pandas/index_range.tcc"

    Index()
    {
    }

    Index(const NT& name)
        : values(name)
    {
    }

    Index(const Index& si)
    {
        values = si.values;
        value2iid = si.value2iid;
    }

    Index(Index&& ir)
    {
        values = std::move(ir.values);
        value2iid = std::move(ir.value2iid);
    }

    Index(const Array<T, NT>& ar, const NT& name = NT {})
        : Index(ar.values, name)
    {
    }

    Index(Array<T, NT>&& ar, const NT& name = NT {})
        : values(ar)
    {
        value2iid.reserve(ar.size());
        _rename(name);
        for (int i = 0; i < ar.size(); i++) {
            value2iid.emplace_back(std::pair { ar.iloc(i), i });
        }
        std::sort(value2iid.begin(), value2iid.end());
    }

    Index(Range<T> rg, const NT& name = {})
        : Index(rg.to_vec(), name)
    {
    }

    Index& operator=(Index&& ir)
    {
        values = std::move(ir.values);
        value2iid = std::move(ir.value2iid);
        return *this;
    }

    NT get_name() const
    {
        return values.name;
    }

    Index& _rename(const NT& nm)
    {
        values._rename(nm);
        return *this;
    }
    Index rename(const NT& nm)
    {
        Index idx = *this;
        idx._rename(nm);
        return idx;
    }

    size_t size() const
    {
        return values.size();
    }

    int lower_bound(const T& v) const
    {
        int l = 0, r = size() - 1;
        if (size() == 0 || value2iid[r].first < v || value2iid[0].first > v) {
            return -1;
        }

        while (l <= r) {
            int m = l + (r - l) / 2;
            if (value2iid[m].first >= v) {
                r = m - 1;
            } else {
                l = m + 1;
            }
        }
        return l;
    }

    int upper_bound(const T& v) const
    {
        int l = 0, r = size() - 1;
        if (size() == 0 || value2iid[r].first < v || value2iid[0].first > v) {
            return -1;
        }

        while (l <= r) {
            int m = l + (r - l) / 2;
            if (value2iid[m].first <= v) {
                l = m + 1;
            } else {
                r = m - 1;
            }
        }
        return l;
    }

    inline bool has(const T& v) const
    {
        int i = lower_bound(v);
        if (i < 0 || values.iloc(value2iid[i].second) != v) {
            return false;
        }
        return true;
    }

    int count(const T& v) const
    {
        int lower = lower_bound(v);
        int upper = upper_bound(v);
        if (lower < 0) {
            return 0;
        }
        if (values[value2iid[lower].second] != v) {
            return 0;
        }
        return upper - lower;
    }

    Array<char, NT> duplicated(const std::string& keep)
    {
        return values.duplicated(keep);
    }

    Index drop_duplicates(const std::string& keep)
    {
        Array<T, NT> ar_idx(get_name());
        Array<char, NT> dup = duplicated(keep);
        for (int i = 0; i < size(); i++) {
            if (!dup.iloc(i)) {
                ar_idx._append(iloc(i));
            }
        }
        return Index<T, NT>(std::move(ar_idx));
    }

    int _append(const T& v, bool flush_index = true)
    {
        int i = size();
        values._append(v);
        value2iid.push_back({ v, i });
        if (flush_index) {
            std::sort(value2iid.begin(), value2iid.end());
        }
        return size();
    }

    void _flush_index()
    {
        value2iid.clear();
        for (int i = 0; i < size(); i++) {
            value2iid.push_back({ values.iloc(i), i });
        }
        std::sort(value2iid.begin(), value2iid.end());
    }

    inline int loc_i(const T& key) const
    {
        int i = lower_bound(key);
        if (i >= 0 && values.iloc(value2iid[i].second) == key) {
            if (i + 1 < size() && values.iloc(value2iid[i + 1].second) == key) {
                PANDAS_THROW(std::format("duplicated key: {}", pandas::to_string(key)));
            }
            return value2iid[i].second;
        } else {
            PANDAS_THROW(std::format("key not found: {}", pandas::to_string(key)));
        }
    }

    inline T loc(const T& key) const
    {
        if (!has(key)) {
            PANDAS_THROW(std::format("key not found: {}", pandas::to_string(key)));
        }
        return key;
    }

    inline T& loc_ref(const T& key)
    {
        return iloc_ref(loc_i(key));
    }

    inline T iloc(int i) const
    {
        return values.iloc(i);
    }

    inline T& iloc_ref(int i)
    {
        return values.iloc_ref(i);
    }

    IndexRange range(const T& bgn, const T& end)
    {
        return IndexRange(*this, bgn, end);
    }

    template <class T2, class NT2>
    Index<T2, NT2> astype()
    {
        Array<T2, NT2> ar;
        for (int i = 0; i < size(); i++) {
            ar._append(values.iloc(i));
        }
        return Index<T2, NT2>(std::move(ar), get_name());
    }

    Index<T, NT>& _sort(bool ascending = true)
    {
        values._sort(ascending);
        value2iid.clear();
        for (int i = 0; i < values.size(); i++) {
            value2iid.push_back({ values.iloc(i), i });
        }
        std::sort(value2iid.begin(), value2iid.end());
        return *this;
    }
    Index<T, NT> sort(bool ascending = true) const
    {
        Index<T, NT> idx = *this;
        idx._sort();
        return idx;
    }

    std::string to_string(int mx_cnt = 10, bool tail = true) const
    {
        return values.to_string(mx_cnt, tail);
    }

    friend std::ostream& operator<<(std::ostream& os, const Index<T>& idx)
    {
        os << idx.to_string();
        return os;
    }

    template <int level>
    auto get_level_values() const
    {
        Array<typename std::remove_reference<decltype(std::get<level>(iloc(0)))>::type, NT> ar(get_name());
        ar._reserve(size());
        for (int i = 0; i < size(); i++) {
            ar._append(std::get<level>(iloc(i)));
        }
        return ar;
    }

    template <int level>
    auto droplevel() const
    {
        Array<typename std::remove_reference<decltype(remove_element<level>(iloc(0)))>::type, NT> ar(get_name());
        ar._reserve(size());
        for (int i = 0; i < size(); i++) {
            ar._append(remove_element<level>(iloc(i)));
        }
        return ar;
    }
};

}