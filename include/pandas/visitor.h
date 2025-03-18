#pragma once

#include <cmath>

namespace pandas {

template <class T>
class Visitor {
public:
    virtual T next()
    {
        throw std::format("next not supported");
    };

    virtual T& next_ref()
    {
        throw std::format("next_ref not supported");
    };

    virtual bool has()
    {
        return false;
    }

    virtual T iloc(int i) const
    {
        throw std::format("iloc not supported");
    };

    virtual T& iloc(int i)
    {
        throw std::format("iloc not supported");
    };

    virtual size_t size()
    {
        size_t s = 0;
        while (has()) {
            s++;
            next();
        }
        return s;
    };

    virtual void reset()
    {
        throw std::format("reset not supported");
    }

    std::vector<T> to_vec()
    {
        std::vector<T> vs;
        for (;;) {
            std::optional<T> ov = next();
            if (ov.has_value()) {
                vs.push_back(ov.value());
            } else {
                break;
            }
        }
        return vs;
    }

    T sum()
    {
        T s = iloc(0);
        for (int i = 1; i < size(); i++) {
            T v = iloc(i);
            if (!isnan(v)) {
                s += v;
            }
        }
        return s;
    }

    T max()
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

    T min()
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

    double mean()
    {
        double cnt = count();
        double s = sum();
        return s / cnt;
    }

    double var()
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

    double std()
    {
        double v = var();
        return sqrt(v);
    }

    int count()
    {
        int cnt = 0;
        for (int i = 0; i < size(); i++) {
            cnt += ~isnan(iloc(i));
        }
        return cnt;
    }
};
}