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
class Index {
public:
    virtual std::string name() const = 0;
    virtual size_t size() const = 0;
    virtual void update_index() = 0;
    virtual bool has(const T& key) const = 0;
    virtual int append(const T& key) = 0;
    virtual void clear() = 0;

    virtual int loc(const T& key) = 0;
    virtual std::vector<int> loc(const T& bgn, const T& end) = 0;

    virtual T iloc(int i) = 0;
    virtual std::vector<T> iloc(int bgn, int end, int step = 1) = 0;

    virtual std::string to_string() const = 0;

    virtual std::shared_ptr<Index<T>> clone() const = 0;
    virtual std::shared_ptr<Index<T>> new_empty() const = 0;
};

}
