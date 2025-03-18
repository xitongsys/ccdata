#pragma once

#include <optional>

#include "pandas/visitor.h"

namespace pandas {

template <class T>
class Range : public Visitor<int> {
public:
    int bgn, end, step, cur;

    Range()
    {
        bgn = 0;
        end = 0;
        step = 1;
        cur = 0;
    }

    Range(int bgn, int end, int step)
    {
        this->bgn = bgn;
        this->end = end;
        this->step = step;
        this->cur = bgn;
    }

    bool has() const
    {
        if (step > 0 and cur < end) {
            return true;
        }
        if (step < 0 and cur > end) {
            return true;
        }
        return false;
    }

    int next()
    {
        if (!has()) {
            throw std::format("no value left");
        }
        int res = cur;
        cur += step;
        return res;
    }

    size_t size() const
    {
        int s = (end - bgn) / step;
        s = std::max(0, s);
        return s;
    }

    void reset()
    {
        cur = bgn;
    }
};
}