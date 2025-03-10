#pragma once

// template<class IT, class DT>
// class Series
class SeriesPicker {
public:
    Series& sr;
    std::vector<int> iids;

    SeriesPicker(Series& sr_, const std::vector<int>& iids_)
        : sr(sr_)
        , iids(iids_)
    {
    }

    Series to_series()
    {
        Series res;
        for (int i in iids) {
            IT& id = sr.index.iloc(i);
            DT& val = sr.values.iloc(i);
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
            sr.iloc(i) = (T)(v);
        }
    }

    template <class DT2>
    void operator=(const Array<DT2>& ar2)
    {
        if (ar2.size() != size()) {
            throw std::format("size not match: {} != {}", ar2.size(), size());
        }
        for (int i = 0; i < iids.size(); i++) {
            sr.iloc(iids[i]) = (DT)(ar2.iloc(i));
        }
    }

    template <class IT2, DT2>
    void operator=(const Series<IT2, DT2>& sr2)
    {
        if (sr2.size() != size()) {
            throw std::format("size not match: {} != {}", sr.size(), size());
        }

        for (int i = 0; i < iids.size(); i++) {
            IT2& id = sr.pidx->iloc(iids[i]);
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
};
