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
#include "pandas/visitor.h"

namespace pandas {

template <class T, class NT = std::string>
class Index {
public:
    virtual NT get_name() const = 0;
    virtual size_t size() const = 0;
    virtual bool has(const T& key) const = 0;

    virtual void _rename(const NT& name) = 0;
    virtual void _clear() = 0;
    virtual void _update_index() = 0;
    virtual int _append(const T& key) = 0;

    // by value to pos index
    virtual int loc_i(const T& key) = 0;
    virtual std::shared_ptr<Visitor<int>> loc_i(const T& bgn, const T& end) = 0;

    // by value to value
    virtual T loc(const T& key) = 0;
    virtual T& loc_ref(const T& key) = 0;
    virtual std::shared_ptr<Visitor<T>> loc(const T& bgn, const T& end) = 0;
    

    // by pos index to value
    virtual T iloc(int i) = 0;
    virtual T& iloc_ref(int i) = 0;
    virtual std::shared_ptr<Visitor<T>> iloc(int bgn, int end, int step = 1) = 0;

    virtual std::string to_string() const = 0;

    virtual std::shared_ptr<Index<T>> new_index() const = 0;
    virtual std::shared_ptr<Index<T>> new_clone() const = 0;
};

}
