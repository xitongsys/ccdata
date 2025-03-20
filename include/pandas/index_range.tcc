#pragma once

// template <class T, class NT = std::string>
// class Index {

class IndexRange : public Iterator<int> {
public:
    Index& si;
    T bgn, end;
    std::map<T, int>::iterator it;
    std::map<T, int>::iterator it_end;

    IndexRange(Index& si_, const T& bgn_, const T& end_)
        : si(si_)
        , bgn(bgn_)
        , end(end_)
    {
        it = si.value2iid.lower_bound(bgn);
        it_end = si.value2iid.upper_bound(end);
    }

    inline bool has_left()
    {
        return !(it == si.value2iid.end() or it == it_end);
    }

    inline int next()
    {
        if (!has_left()) {
            throw std::format("no left data");
        }

        int i = it->second;
        it++;
        return i;
    }

    void reset()
    {
        it = si.value2iid.lower_bound(bgn);
        it_end = si.value2iid.upper_bound(end);
    }
};
