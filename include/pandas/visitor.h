#pragma once

#include <cmath>

namespace pandas {

template <class T>
class Visitor {
public:
    virtual T iloc(int i) const = 0;
    virtual size_t size() const = 0;

    T sum() const
    {
        T s = iloc(0);
        for (int i = 1; i < size(); i++) {
            T v = iloc(i);
            if (!isnan(v)) {
                s += iloc(i);
            }
        }
        return s;
    }

    T max() const
    {
        T s = iloc(0);
        for (int i = 1; i < size(); i++) {
            T v = iloc(i);
            if (v > s) {
                s = v;
            }
        }
        return s;
    }

    T min() const
    {
        T s = iloc(0);
        for (int i = 1; i < size(); i++) {
            T v = iloc(i);
            if (v < s) {
                s = v;
            }
        }
        return s;
    }

    double mean() const
    {
        double cnt = count();
        double s = sum();
        return s / cnt;
    }

    double var() const
    {
        double cnt = count();
        double mn = mean();
        double s = 0;
        for (int i = 0; i < size(); i++) {
            double v = iloc(i);
            if (!isnan(v)) {
                s += (v - mn) * (v - mn);
            }
        }
        return s / cnt;
    }

    double std() const
    {
        double v = var();
        return sqrt(v);
    }

    int count() const
    {
        int cnt = 0;
        for (int i = 0; i < size(); i++) {
            cnt += ~isnan(iloc(i));
        }
        return cnt;
    }
};
}