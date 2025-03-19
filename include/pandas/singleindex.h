#pragma once

#include <functional>
#include <iostream>
#include <list>
#include <map>
#include <optional>
#include <vector>

#include "pandas/array.h"
#include "pandas/index.h"
#include "pandas/iterator.h"
#include "pandas/pandastype.h"
#include "pandas/range.h"
#include "pandas/util.h"
#include "pandas/visitor.h"

namespace pandas {

template <class T, class NT = std::string>
class SingleIndex {
public:
    Array<T, NT> values;
    std::map<T, int> value2iid;

#include "pandas/singleindex_range.tcc"
#include "pandas/singleindex_visitor.tcc"

    SingleIndex()
    {
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

    NT get_name() const
    {
        return values.name;
    }

    void _rename(const NT& nm)
    {
        values._rename(nm);
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
                throw std::format("index '{}' has duplicated value {}", this->get_name(), v);
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

    int loc_i(const T& v) const
    {
        if (value2iid.count(v)) {
            int i = value2iid.at(v);
            return i;
        } else {
            throw std::format("key not found: {}", pandas::to_string(v));
        }
    }

    T loc(const T& key) const
    {
        if (!has(key)) {
            throw std::format("key not found: {}", pandas::to_string(key));
        }
        return key;
    }

    T& loc_ref(const T& key)
    {
        if (!has(key)) {
            throw std::format("key not found: {}", pandas::to_string(key));
        }
        return iloc_ref(value2iid[key]);
    }

    T iloc(int i) const
    {
        return values.iloc(i);
    }

    T& iloc_ref(int i)
    {
        return values.iloc_ref(i);
    }

    SingleIndexVisitor<Range<int>> iloc(int bgn, int end, int step = 1)
    {
        return SingleIndexVisitor<Range<int>>(*this, Range<int>(bgn, end, step));
    }

    SingleIndexVisitor<SingleIndexRange> loc(const T& bgn, const T& end)
    {
        return SingleIndexVisitor(*this, SingleIndexRange(*this, bgn, end));
    }

    SingleIndexRange range(const T& bgn, const T& end)
    {
        return SingleIndexRange(*this, bgn, end);
    }

    template <class T2, class NT2>
    SingleIndex<T2, NT2> astype()
    {
        SingleIndex<T2, NT2> si2;
        for (int i = 0; i < size(); i++) {
            si2.append(iloc(i));
        }
        si2._rename(get_name());
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

template <class IT, class NT, class IT2, class NT2>
SingleIndex<IT, NT> concat_0(const SingleIndex<IT, NT>& idx1, const SingleIndex<IT2, NT2>& idx2)
{
    SingleIndex<IT, NT> idx_merge = idx1;
    for (int i = 0; i < idx2.size(); i++) {
        const IT& id = idx2.iloc(i);
        idx_merge.append(id);
    }
    return idx_merge;
}

}