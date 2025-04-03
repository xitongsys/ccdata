#pragma once

// template <class T, class NT = std::string>
// class Index {

class IndexRange : public Iterator<int> {
public:
    Index& si;
    T bgn, end;
    int curr, lower, upper;

    IndexRange(Index& si_, const T& bgn_, const T& end_)
        : si(si_)
        , bgn(bgn_)
        , end(end_)
    {
        lower = si.lower_bound(bgn);
        upper = si.upper_bound(end);
        curr = lower;
    }

    inline bool has_left()
    {
        return (curr < upper) && (curr >= 0);
    }

    inline int next()
    {
        if (!has_left()) {
            PANDAS_THROW(std::format("no left data"));
        }

        return si.value2iid[curr++].second;
    }

    void reset()
    {
        curr = lower;
    }
};
