#pragma once

// template<class IT, class DT, class NT>
// class Series {

template <class IT2, class DT2, class INT2, class DNT2>
class SeriesPicker;

template <>
class SeriesPicker<IT, DT, INT, DNT> : public Visitor<DT> {
public:
    Series<IT, DT, INT, DNT>& sr;
    std::shared_ptr<Visitor<int>> pvis;

    SeriesPicker(Series<IT, DT, INT, DNT>& sr_, const std::shared_ptr<Visitor<int>>& pvis_)
        : sr(sr_)
        , pvis(pvis_)
    {
    }

    bool has_left() const
    {
        return pvis->has_left();
    }

    

    Series<IT, DT, INT, DNT> to_series() const
    {
        pvis->reset();
        auto pidx = sr.pidx->new_empty();
        Series<IT, DT, INT, DNT> res(pidx);

        for (int i = pvis.next().value_or(-1); i >= 0; i = pvis.next().value_or(-1)) {
            const IT& id = sr.pidx->iloc(i);
            const DT& val = sr.values.iloc(i);
            res.append(id, val);
        }
        return res;
    }

    template <class T2>
    void operator=(const T2& v)
    {
        pvis->reset();
        for (int i = pvis.next().value_or(-1); i >= 0; i = pvis.next().value_or(-1)) {
            sr.iloc(i) = v;
        }
    }

    template <class DT2, class NT2>
    void operator=(const Array<DT2, NT2>& ar)
    {
        pvis.reset();
        if (ar.size() != pvis.size()) {
            throw std::format("size not match: {} != {}", ar.size(), size());
        }

        pvis.reset();
        for (int i = pvis.next().value_or(-1), j = 0; i >= 0; i = pvis.next().value_or(-1), j++) {
            sr.iloc(i) = ar.iloc(j);
        }
    }

    template <class IT2, class DT2, class INT2, class DNT2>
    void operator=(const Series<IT2, DT2, INT2, DNT2>& sr2)
    {
        pvis.reset();
        if (sr2.size() != size()) {
            throw std::format("size not match: {} != {}", sr2.size(), size());
        }

        pvis.reset();
        for (int i = pvis.next().value_or(-1); i >= 0; i = pvis.next().value_or(-1)) {
            IT& id = sr.pidx->iloc(i);
            if (!sr2.has(id)) {
                throw std::format("key not found: {}", id);
            }
            sr.iloc(i) = (DT)(sr2.loc(id));
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
