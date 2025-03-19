#pragma once

// template<class IT, class DT, class INT, class DNT>
// class Series {

template <class IIT>
class SeriesVisitor {
public:
    Series<IT, DT, INT, DNT>& sr;
    IIT it;
    std::vector<int> iids;

    SeriesVisitor(Series<IT, DT, INT, DNT>& sr_, const IIT& it_)
        : sr(sr_)
        , it(it_)
    {
    }

    bool has_left()
    {
        return it.has_left();
    }

    DT next()
    {
        int i = it.next();
        return sr.iloc(i);
    }

    DT& next_ref()
    {
        int i = it.next();
        return sr.iloc_ref(i);
    }

    Series<IT, DT, INT, DNT> to_series() const
    {
        it.reset();
        Series<IT, DT, INT, DNT> res;
        while (it.has_left()) {
            int i = it.next();
            IT id = sr.pidx->iloc(i);
            DT val = sr.iloc(i);
            res._append(id, val);
        }
        return res;
    }

    template <class T2>
    void operator=(const T2& v)
    {
        it.reset();
        while (it.has_left()) {
            int i = it.next();
            sr.iloc_ref(i) = v;
        }
    }

    template <class DT2, class NT2>
    void operator=(const Array<DT2, NT2>& ar)
    {
        it.reset();
        for (int i = 0; i < ar.size() && it.has_left(); i++) {
            int j = it->next();
            sr.iloc_ref(j) = ar.iloc(i);
        }
    }

    template <class IT2, class DT2, class INT2, class DNT2>
    void operator=(const Series<IT2, DT2, INT2, DNT2>& sr2)
    {
        it.reset();
        while (it.has_left()) {
            int i = it.next();
            IT id = sr.pidx->iloc(i);
            sr.iloc_ref(i) = sr2.loc(id);
        }
    }

    template <class IT2, class DT2, class INT2, class DNT2, class IIT2>
    void operator=(const typename Series<IT2, DT2, INT2, DNT2>::template SeriesVisitor<IIT2>& sp2)
    {
        Series ds = sp2.to_series();
        (*this) = ds;
    }

    // functions that need iids

    void extract_iids()
    {
        if (iids.size() == 0) {
            it.reset();
            while (it.has_left()) {
                int i = it.next();
                iids.push_back(i);
            }
        }
    }

    size_t size()
    {
        extract_iids();
        return iids.size();
    }

    DT iloc(int i)
    {
        extract_iids();
        if (i > iids.size()) {
            throw std::format("i = {} overflow size = {}", i, size());
        }
        i = iids[i];
        return sr.iloc(i);
    }

    DT& iloc_ref(int i)
    {
        extract_iids();
        if (i > iids.size()) {
            throw std::format("i = {} overflow size = {}", i, size());
        }
        i = iids[i];
        return sr.iloc_ref(i);
    }

#include "pandas/series_visitor_op.tcc"
};
