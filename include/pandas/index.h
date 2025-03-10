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

template <class T>
class Index : public Array<T> {
public:
    std::map<T, int> value2iid;

    Index()
        : Array()
    {
    }

    Index(const Array<T>& ar)
        : Array<T>(ar)
    {
        update_index();
    }

    void update_index()
    {
        for (int i = 0; i < this->size(); i++) {
            T& v = this->iloc(i);
            if (value2iid.count(v)) {
                throw std::format("index {} has duplicated value {}", this->name, i);
            }
            value2iid[v] = i;
        }
    }

    bool has(const T& v)
    {
        return value2iid.count(v) > 0;
    }

    template <class T2>
    void append(const T2& v)
    {
        if (has((T)(v))) {
            continue;
        }
        value2iid[(T)(v)] = this->size();
        Array<T>::append((T)(v));
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
};

}