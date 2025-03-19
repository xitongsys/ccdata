#pragma once

#include <cmath>

namespace pandas {

template <class T>
class Iterator {
public:
    T next()
    {
        throw std::format("next not supported");
    };

    T& next_ref()
    {
        throw std::format("next_ref not supported");
    };

    bool has_left() const
    {
        return false;
    }

    void reset()
    {
        throw std::format("reset not supported");
    }
};
}