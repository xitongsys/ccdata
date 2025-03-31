#pragma once

#include <cmath>

namespace pandas {

template <class T>
class Iterator {
public:
    T next()
    {
        PANDAS_THROW(std::format("next not supported"));
    };

    T& next_ref()
    {
        PANDAS_THROW(std::format("next_ref not supported"));
    };

    bool has_left() const
    {
        return false;
    }

    void reset()
    {
        PANDAS_THROW(std::format("reset not supported"));
    }
};
}