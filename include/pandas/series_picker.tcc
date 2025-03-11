#pragma once

// template<class IT, class DT>
// class Series {

template <class IT, class DT>
class SeriesPicker {
public:
    Series<IT, DT>& sr;
    std::vector<int> iids;

    SeriesPicker(Series<IT, DT>& sr_, const std::vector<int>& iids_)
        : sr(sr_)
        , iids(iids_)
    {
    }

    Series<IT, DT> to_series() const
    {
        Series<IT, DT> res;
        for (int i : iids) {
            const IT& id = sr.pidx->iloc(i);
            const DT& val = sr.values.iloc(i);
            res.append(id, val);
        }
        return res;
    }

    size_t size() const
    {
        return iids.size();
    }

    template <class T2>
    void operator=(const T2& v)
    {
        for (int i : iids) {
            sr.iloc(i) = (DT)(v);
        }
    }

    template <class DT2>
    void operator=(const Array<DT2>& ar)
    {
        if (ar.size() != size()) {
            throw std::format("size not match: {} != {}", ar.size(), size());
        }
        for (int i = 0; i < iids.size(); i++) {
            sr.iloc(iids[i]) = (DT)(ar.iloc(i));
        }
    }

    template <class IT2, class DT2>
    void operator=(const Series<IT2, DT2>& sr2)
    {
        if (sr2.size() != size()) {
            throw std::format("size not match: {} != {}", sr2.size(), size());
        }

        for (int i = 0; i < iids.size(); i++) {
            IT& id = sr.pidx->iloc(iids[i]);
            if (!sr2.has(id)) {
                throw std::format("key not found: {}", id);
            }

            sr.iloc(iids[i]) = (DT)(sr2.loc(id));
        }
    }

    template <class IT2, class DT2>
    void operator=(const SeriesPicker<IT2, DT2>& sp2)
    {
        Series<IT2, DT2> ds = sp2.to_series();
        (*this) = ds;
    }

#include "pandas/series_picker_op.tcc"
};
