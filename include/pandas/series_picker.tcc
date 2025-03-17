#pragma once

// template<class IT, class DT, class NT>
// class Series {

template <class IT2, class DT2, class INT2, class DNT2>
class SeriesPicker;

class SeriesPicker : public Visitor<DT> {
public:
    Series<IT, DT, INT, DNT>& sr;
    std::vector<int> iids;

    SeriesPicker(Series<IT, DT, INT, DNT>& sr_, const std::vector<int>& iids_)
        : sr(sr_)
        , iids(iids_)
    {
    }

    Series<IT, DT, INT, DNT> to_series() const
    {
        auto pidx = sr.pidx->new_empty();
        Series<IT, DT> res(pidx);
        for (int i : iids) {
            const IT& id = sr.pidx->iloc(i);
            const DT& val = sr.values.iloc(i);
            res.append(id, val);
        }
        return res;
    }

    Series<IT, DT, INT, DNT> to_emptyindex_series() const
    {
        std::shared_ptr<Index<IT>> pidx = std::make_shared<EmptyIndex<IT>>();
        Series<IT, DT> res(pidx);
        for (int i : iids) {
            const DT& val = sr.iloc(i);
            res.append(IT {}, val);
        }
        return res;
    }

    size_t size() const
    {
        return iids.size();
    }

    DT iloc(int i) const
    {
        return sr.iloc(iids[i]);
    }

    DT& iloc(int i)
    {
        return sr.iloc(iids[i]);
    }

    template <class T2>
    void operator=(const T2& v)
    {
        for (int i : iids) {
            sr.iloc(i) = (DT)(v);
        }
    }

    template <class DT2, class NT2>
    void operator=(const Array<DT2, NT2>& ar)
    {
        if (ar.size() != size()) {
            throw std::format("size not match: {} != {}", ar.size(), size());
        }
        for (int i = 0; i < iids.size(); i++) {
            sr.iloc(iids[i]) = (DT)(ar.iloc(i));
        }
    }

    template <class IT2, class DT2, class INT2, class DNT2>
    void operator=(const Series<IT2, DT2, NT2, DNT2>& sr2)
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

    template <class IT2, class DT2, class INT2, class DNT2>
    void operator=(const SeriesPicker<IT2, DT2, INT2, DNT2>& sp2)
    {
        Series<IT2, DT2, INT2, DNT2> ds = sp2.to_series();
        (*this) = ds;
    }

#include "pandas/series_picker_op.tcc"
};
