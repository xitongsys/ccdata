#pragma once

#include <optional>

#include "pandas/Iterator.h"
#include "pandas/index.h"

namespace pandas {

template <class T>
class Range : Iterator<T> {
public:
    int bgn, end, step, cur;

    Range()
    {
        bgn = 0;
        end = 0;
        step = 1;
        cur = 0;
    }

    Range(int bgn, int end, int step = 1)
    {
        this->bgn = bgn;
        this->end = end;
        this->step = step;
        this->cur = bgn;
    }

    inline bool has_left() const
    {
        if (step > 0 and cur < end) {
            return true;
        }
        if (step < 0 and cur > end) {
            return true;
        }
        return false;
    }

    inline int next()
    {
        if (!has_left()) {
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

    std::vector<int> to_vec()
    {
        reset();
        std::vector<int> vs;
        while (has_left()) {
            vs.push_back(next());
        }
        return vs;
    }
};

template <class T>
class RangeVec : Iterator<T> {
public:
    std::vector<T> vs;
    int cur = 0;

    RangeVec() { }

    RangeVec(const std::vector<T>& vs_)
        : vs(vs_)
        , cur(0)
    {
    }

    RangeVec(std::vector<T>&& vs_)
        : vs(vs_)
        , cur(0)
    {
    }

    RangeVec(const RangeVec& rv)
        : vs(rv.vs)
        , cur(rv.cur)
    {
    }

    RangeVec(RangeVec&& rv)
        : vs(std::move(rv.vs))
        , cur(rv.cur)
    {
    }

    inline bool has_left() const
    {
        return cur < vs.size();
    }

    inline T next()
    {
        if (!has_left()) {
            throw std::format("no value left");
        }

        T val = vs[cur];
        cur++;
        return val;
    }

    size_t size() const
    {
        return vs.size();
    }

    void reset()
    {
        cur = 0;
    }
};

}