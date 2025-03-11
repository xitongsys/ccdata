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
        : Array<T>()
    {
    }

    Index(const Index& ir)
        : Array<T>(ir)
    {
        update_index();
    }

    Index(Index&& ir)
        : Array<T>(ir)
    {
        value2iid = std::move(ir.value2iid);
    }

    template <class T2>
    Index& operator=(const Index<T2>& ir)
    {
        Array<T>::operator=(ir);
        update_index();
        return *this;
    }

    Index& operator=(const Index& ir)
    {
        Array<T>::operator=(ir);
        update_index();
        return *this;
    }

    Index& operator=(Index&& ir)
    {
        update_index();
        return *this;
    }

    template <class T2>
    Index(const Array<T2>& ar)
        : Array<T>(ar)
    {
        update_index();
    }

    void update_index()
    {
        value2iid.clear();
        for (int i = 0; i < this->size(); i++) {
            T& v = this->iloc(i);
            if (value2iid.count(v)) {
                throw std::format("index '{}' has duplicated value {}", this->name, v.to_string());
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
            return;
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
            throw std::format("key not found");
        }
    }
};

}