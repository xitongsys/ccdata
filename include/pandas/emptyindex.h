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
class EmptyIndex : public Index<T> {
public:
    EmptyIndex()
    {
    }

    std::string name() const
    {
        return "EmptyIndex";
    }

    size_t size() const
    {
        return 0;
    }

    void clear()
    {
    }

    void update_index()
    {
    }

    bool has(const T& v) const
    {
        return false;
    }

    int append(const T& v)
    {
        return 0;
    }

    int loc(const T& v)
    {
        throw std::format("EmptyIndex not support");
    }

    std::vector<int> loc(const T& bgn, const T& end)
    {
        throw std::format("EmptyIndex not support");
        return {};
    }

    T iloc(int i)
    {
        return T {};
    }

    std::vector<T> iloc(int bgn, int end, int step = 1)
    {
        throw std::format("EmptyIndex not support");
        return {};
    }

    std::shared_ptr<Index<T>> new_clone() const
    {
        auto ptr = std::make_shared<EmptyIndex<T>>(*this);
        return ptr;
    }

    std::shared_ptr<Index<T>> new_index() const
    {
        return std::make_shared<EmptyIndex<T>>();
    }

    std::string to_string() const
    {
        return "EmptyIndex:{}";
    }

    friend std::ostream& operator<<(std::ostream& os, const EmptyIndex<T>& idx)
    {
        os << idx.to_string();
        return os;
    }
};



}