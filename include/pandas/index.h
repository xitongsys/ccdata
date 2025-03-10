#pragma once

#include <functional>
#include <iostream>
#include <list>
#include <map>
#include <optional>
#include <vector>

#include "pandas/pandastype.h"
#include "pandas/util.h"

namespace pandas {

template <class T>
class Index : public Array<T> {
public:
    std::map<T, int> value2iid;

    Index(const Array& ar)
        : Array(ar)
    {
        update_index();
    }

    void update_index()
    {
        for (int i = 0; i < size(); i++) {
            if (value2iid.count(iloc(i))) {
                throw std::format("index {} has duplicated value {}", ar.name, ar[i]);
            }
            value2iid[ar[i]] = i;
        }
    }

    bool has(const T& v)
    {
        return value2iid.count(v) > 0;
    }

    void append(const T& v)
    {
        if (has(v)) {
            continue;
        }
        value2iid[v] = size();
        Array<T>::append(v);
    }

    int loc(const T& v)
    {
        if (value2iid.count(v)) {
            int i = value2iid[v];
            return i;
        } else {
            throw std::format("key not found {}", v);
        }
    }

    ArrayPick loc(const T& bgn, const T& end)
    {
        std::vector<int> ids;
        for (auto it = value2iid.lower_bound(bgn); it < value2iid.upper_bound(end); it++) {
            ids.push_back(it->second);
        }
        return ArrayPick(*this, ids);
    }
};

}