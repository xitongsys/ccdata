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
class SingleIndex : public Index<T> {
public:
    Array<T> values;
    std::map<T, int> value2iid;

    SingleIndex()
    {
    }

    SingleIndex(const Index<T>& idx)
    {
        values.name = idx.name();
        for (int i = 0; i < idx.size(); i++) {
            _append(idx.iloc(i));
        }
    }

    SingleIndex(size_t n, const T& init_val)
        : values(n, init_val)
    {
        _update_index();
    }

    SingleIndex(const SingleIndex& si)
    {
        values = si.values;
        value2iid = si.value2iid;
    }

    SingleIndex(SingleIndex&& ir)
    {
        values = std::move(ir.values);
        value2iid = std::move(ir.value2iid);
    }

    SingleIndex& operator=(SingleIndex&& ir)
    {
        values = std::move(ir.values);
        value2iid = std::move(ir.value2iid);
        _update_index();
        return *this;
    }

    template <class T2>
    SingleIndex(const Array<T2>& ar)
    {
        values = ar;
        _update_index();
    }

    std::string name() const
    {
        return values.name;
    }

    void rename(const std::string& nm)
    {
        values.name = nm;
    }

    size_t size() const
    {
        return values.size();
    }

    void _clear()
    {
        values._clear();
        value2iid.clear();
    }

    void _update_index()
    {
        value2iid.clear();
        for (int i = 0; i < this->size(); i++) {
            T v = this->iloc(i);
            if (value2iid.count(v)) {
                throw std::format("index '{}' has duplicated value {}", this->name(), v);
            }
            value2iid[v] = i;
        }
    }

    bool has(const T& v) const
    {
        return value2iid.count(v) > 0;
    }

    int _append(const T& v)
    {
        if (has(v)) {
            return -1;
        }
        int n = value2iid.size();
        value2iid[v] = n;
        values._append(v);
        return 0;
    }

    int loc(const T& v)
    {
        if (value2iid.count(v)) {
            int i = value2iid[v];
            return i;
        } else {
            throw std::format("key not found");
        }
    }

    std::vector<int> loc(const T& bgn, const T& end)
    {
        std::vector<int> iids;
        if (end < bgn) {
            return iids;
        }
        auto upper = value2iid.upper_bound(end);
        for (auto it = value2iid.lower_bound(bgn); it != upper; it++) {
            iids.push_back(it->second);
        }
        return iids;
    }

    T iloc(int i) const
    {
        return values.iloc(i);
    }

    T& iloc(int i)
    {
        return values.iloc(i);
    }

    std::vector<T> iloc(int bgn, int end, int step = 1) const
    {
        std::vector<T> vs;
        for (int i = bgn; i < end; i += step) {
            vs.push_back(values.iloc(i));
        }
        return vs;
    }

    template <class T2>
    SingleIndex<T2> astype()
    {
        SingleIndex<T2> si2;
        for (int i = 0; i < size(); i++) {
            si2.append(iloc(i));
        }
        return si2;
    }

    Index<T> sort(bool ascending = true) const
    {
        SingleIndex<T> si;
        if (ascending) {
            for (auto it = value2iid.begin(); it != value2iid.end(); it++) {
                si.append(it->first);
            }

        } else {
            for (auto it = value2iid.rbegin(); it != value2iid.rend(); it++) {
                si.append(it->first);
            }
        }
        return si;
    }

    std::shared_ptr<Index<T>> new_clone() const
    {
        auto ptr = std::make_shared<SingleIndex>(*this);
        return ptr;
    }

    std::shared_ptr<Index<T>> new_index() const
    {
        return std::make_shared<SingleIndex<T>>();
    }

    std::string to_string() const
    {
        std::stringstream ss;
        ss << "SingleIndex: {" << values.to_string() << "}";
        return ss.str();
    }

    friend std::ostream& operator<<(std::ostream& os, const SingleIndex<T>& idx)
    {
        os << idx.to_string();
        return os;
    }
};

template <class IT1, class IT2>
SingleIndex<IT1> concat(const SingleIndex<IT1>& idx1, const SingleIndex<IT2>& idx2)
{
    SingleIndex<IT1> idx_merge = idx1;
    for (int i = 0; i < idx2.size(); i++) {
        const IT2& id = idx2.iloc(i);
        idx_merge.append(id);
    }
    return idx_merge;
}

}