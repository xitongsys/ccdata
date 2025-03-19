#pragma once

// template <class T, class NT = std::string>
// class SingleIndex {

class SingleIndexRange : public Iterator<int> {
public:
    SingleIndex& si;
    T bgn, end;
    std::map<T, int>::iterator it;
    std::map<T, int>::iterator it_end;

    SingleIndexRange(SingleIndex& si_, const T& bgn_, const T& end_)
        : si(si_)
        , bgn(bgn_)
        , end(end_)
    {
        it = si.value2iid.lower_bound(bgn);
        it_end = si.value2iid.upper_bound(end);
    }

    bool has_left()
    {
        return !(it == si.value2iid.end() or it == it_end);
    }

    int next()
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
