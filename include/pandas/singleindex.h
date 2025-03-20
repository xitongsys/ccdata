#pragma once

#include <functional>
#include <iostream>
#include <list>
#include <map>
#include <optional>
#include <vector>

#include "pandas/array.h"
#include "pandas/iterator.h"
#include "pandas/pandastype.h"
#include "pandas/range.h"
#include "pandas/util.h"

namespace pandas {

template <class T, class NT = std::string>
class Index {
public:
    Array<T, NT> values;
    std::map<T, int> value2iid;

#include "pandas/singleindex_range.tcc"
#include "pandas/singleindex_visitor.tcc"

    Index()
    {
    }

    Index(size_t n, const T& init_val)
        : values(n, init_val)
    {
        _update_index();
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

    Index& operator=(Index&& ir)
    {
        values = std::move(ir.values);
        value2iid = std::move(ir.value2iid);
        _update_index();
        return *this;
    }

    template <class T2, class NT2>
    Index(const Array<T2, NT2>& ar)
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
            throw std::format("duplicated key: {}", pandas::to_string(v));
        }
        int n = value2iid.size();
        value2iid[v] = n;
        values._append(v);
        return 0;
    }

    inline int loc_i(const T& v) const
    {
        if (value2iid.count(v)) {
            int i = value2iid.at(v);
            return i;
        } else {
            throw std::format("key not found: {}", pandas::to_string(v));
        }
    }

    inline T loc(const T& key) const
    {
        if (!has(key)) {
            throw std::format("key not found: {}", pandas::to_string(key));
        }
        return key;
    }

    inline T& loc_ref(const T& key)
    {
        if (!has(key)) {
            throw std::format("key not found: {}", pandas::to_string(key));
        }
        return iloc_ref(value2iid[key]);
    }

    inline T iloc(int i) const
    {
        return values.iloc(i);
    }

    inline T& iloc_ref(int i)
    {
        return values.iloc_ref(i);
    }

    IndexVisitor<Range<int>> iloc(int bgn, int end, int step = 1)
    {
        return IndexVisitor<Range<int>>(*this, Range<int>(bgn, end, step));
    }

    IndexVisitor<IndexRange> loc(const T& bgn, const T& end)
    {
        return IndexVisitor(*this, IndexRange(*this, bgn, end));
    }

    IndexRange range(const T& bgn, const T& end)
    {
        return IndexRange(*this, bgn, end);
    }

    template <class T2, class NT2>
    Index<T2, NT2> astype()
    {
        Index<T2, NT2> si2;
        for (int i = 0; i < size(); i++) {
            si2._append(iloc(i));
        }
        si2._rename(get_name());
        return si2;
    }

    Index<T, NT> sort(bool ascending = true) const
    {
        Index<T> si;
        if (ascending) {
            for (auto it = value2iid.begin(); it != value2iid.end(); it++) {
                si._append(it->first);
            }

        } else {
            for (auto it = value2iid.rbegin(); it != value2iid.rend(); it++) {
                si._append(it->first);
            }
        }
        return si;
    }

    std::string to_string() const
    {
        std::stringstream ss;
        ss << "Index: {" << values.to_string() << "}";
        return ss.str();
    }

    friend std::ostream& operator<<(std::ostream& os, const Index<T>& idx)
    {
        os << idx.to_string();
        return os;
    }
};

template <class IT, class NT, class IT2, class NT2>
Index<IT, NT> concat_0(const Index<IT, NT>& idx1, const Index<IT2, NT2>& idx2)
{
    Index<IT, NT> idx_merge = idx1;
    for (int i = 0; i < idx2.size(); i++) {
        const IT& id = idx2.iloc(i);
        idx_merge._append(id);
    }
    return idx_merge;
}

}