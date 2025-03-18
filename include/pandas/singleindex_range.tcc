#pragma once

// template <class T, class NT = std::string>
// class SingleIndex : public Index<T, NT> {

class SingleIndexRange : public Visitor<T> {
public:
    SingleIndex& si;
    T bgn, end;
    std::map<T, int>::iterator it;
    std::map<T, int>::iterator it_end;

    SingleIndexRange(const T& bgn_, const T& end_, SingleIndex& si_)
        : si(si_)
        , bgn(bgn_)
        , end(end_)
    {
        it = si.value2iid.lower_bound(bgn);
        it_end = si.value2iid.upper_bound(end);
    }

    bool has()
    {
        return !(it == si.value2iid.end() or it == it_end);
    }

    T next()
    {
        if (!has()) {
            throw std::format("no left data");
        }

        T res = si.iloc(it->second);
        it++;
        return res;
    }

    T& next_ref() const
    {
        if (!has()) {
            throw std::format("no left data");
        }

        T& res = si.iloc(it->second);
        it++;
        return res;
    }

    void reset()
    {
        it = si.value2iid.lower_bound(bgn);
        it_end = si.value2iid.upper_bound(end);
    }
};

class SingleIndexIRange : public Visitor<int> {
public:
    SingleIndex& si;
    T bgn, end;
    std::map<T, int>::iterator it;
    std::map<T, int>::iterator it_end;

    SingleIndexIRange(const T& bgn_, const T& end_, SingleIndex& si_)
        : si(si_)
        , bgn(bgn_)
        , end(end_)
    {
        it = si.value2iid.lower_bound(bgn);
        it_end = si.value2iid.upper_bound(end);
    }

    bool has()
    {
        return !(it == si.value2iid.end() or it == it_end);
    }

    int next()
    {
        if (!has()) {
            throw std::format("no left data");
        }
        int res = it->second;
        it++;
        return res;
    }

    void reset()
    {
        it = si.value2iid.lower_bound(bgn);
        it_end = si.value2iid.upper_bound(end);
    }
};

class SingleIndexILocRange : public Visitor<T> {
public:
    SingleIndex& si;
    Range<int> rng;
    SingleIndexILocRange(int bgn, int end, int step, SingleIndex& si_)
        : si(si_)
        , rng(Range<int>(bgn, end, step))
    {
    }

    bool has()
    {
        return rng.has();
    }

    T next()
    {
        int i = rng.next();
        return i;
    }

    T& next_ref()
    {
        int i = rng.next();
        T& res = si.iloc_ref(i);
        return res;
    }

    void reset()
    {
        rng.reset();
    }
};