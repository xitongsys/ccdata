#pragma once

#include <algorithm>
#include <ostream>
#include <sstream>
#include <vector>

#include "emptyindex.h"
#include "pandas/array.h"
#include "pandas/index.h"
#include "pandas/pandastype.h"
#include "pandas/range.h"
#include "pandas/singleindex.h"
#include "pandas/visitor.h"

namespace pandas {
// IT: index type, DT: data type, INT: index name type, DNT: data name type
template <class IT, class DT, class INT = std::string, class DNT = std::string>
class Series : public Visitor<DT> {
public:
    std::shared_ptr<Index<IT, INT>> pidx = nullptr;
    Array<DT, DNT> values;

    Series()
    {
        pidx = std::make_shared<SingleIndex<IT, INT>>();
    }

    Series(const DNT& name)
    {
        pidx = std::make_shared<SingleIndex<IT, INT>>();
        _rename(name);
    }

    Series(const Index<IT, INT>& idx)
    {
        this->pidx = std::make_shared<SingleIndex<IT, INT>>(idx);
        for (int i = 0; i < idx.size(); i++) {
            values._append(pandas::nan<DT>());
        }
    }

    Series(std::shared_ptr<Index<IT, INT>> pidx)
    {
        this->pidx = pidx;
        for (int i = 0; i < pidx->size(); i++) {
            values._append(pandas::nan<DT>());
        }
    }

    Series(std::shared_ptr<Index<IT, INT>> pidx, const Array<DT, DNT>& vals)
    {
        if (pidx->size() != vals.size()) {
            throw std::format("index values size not match: {}!={}", pidx->size(), vals.size());
        }
        this->pidx = pidx;
        values = vals;
    }

    Series(std::shared_ptr<Index<IT, INT>> pidx, const Array<DT, DNT>& vals, const DNT& name)
        : Series(pidx, vals)
    {
        _rename(name);
    }

    template <class T>
    Series(const T& idx, const DNT& name)
    {
        auto ptr = std::make_shared<T>(idx);
        pidx = std::static_pointer_cast<Index<IT, INT>>(ptr);
        for (int i = 0; i < pidx->size(); i++) {
            values.append(DT {});
        }
        _rename(name);
    }

    template <class T>
    Series(const T& idx, const Array<DT, DNT>& vals, const DNT& name)
    {
        if (idx.size() != vals.size()) {
            throw std::format("index values size not match: {}!={}", idx.size(), vals.size());
        }
        auto ptr = std::make_shared<T>(idx);
        pidx = std::static_pointer_cast<Index<IT, INT>>(ptr);
        values = vals;
        _rename(name);
    }

    void _append(const IT& id, const DT& val)
    {
        if (pidx->has(id)) {
            throw std::format("append failed: duplicated key");
        }
        pidx->_append(id);
        values._append(val);
    }

    template <class IT2, class DT2, class INT2, class DNT2>
    Series(const Series<IT2, DT2, INT2, DNT2>& sr)
    {
        pidx = sr.pidx->clone();
        values = sr.values;
    }

    Series(const Series& sr)
    {
        pidx = sr.pidx->new_clone();
        values = sr.values;
    }

    Series(Series&& sr)
    {
        pidx = sr.pidx;
        values = std::move(sr.values);
    }

    template <class IT2, class DT2, class INT2, class DNT2>
    Series& operator=(const Series<IT2, DT2, INT2, DNT2>& sr)
    {
        pidx = sr.pidx->clone();
        values = sr.values;
        return *this;
    }

    Series& operator=(Series&& sr)
    {
        pidx = sr.pidx;
        values = std::move(sr.values);
        return *this;
    }

    Series& operator=(const Series& sr)
    {
        pidx = sr.pidx->clone();
        values = sr.values;
        return *this;
    }

    size_t size() const
    {
        return values.size();
    }

    DNT get_name() const
    {
        return values.get_name();
    }

    void _rename(const std::string& name)
    {
        values._rename(name);
    }

    template <class IT2, class INT2>
    Series<IT2, DT, INT2, DNT> reindex(const Index<IT2, INT2>& index)
    {
        Series<IT2, DT, INT2, DNT> res(index.new_index());

        for (int i = 0; i < index.size(); i++) {
            IT2 id = index.iloc(i);
            DT val = pandas::nan<DT>();
            if (pidx->has((IT)(id))) {
                val = loc((IT)(id));
            }
            res._append(id, val);
        }
        res._rename(get_name());
        return res;
    }

    template <class IT2, class DT2, class INT2 = INT, class DNT2 = DNT>
    Series<IT2, DT2, INT2, DNT2> astype()
    {
        auto idx = pidx->astype<IT2, INT2>();
        auto vals = values.astype<DT2, DNT2>();

        return Series<IT2, DT2>(idx, vals);
    }

    std::string to_string() const
    {
        std::stringstream ss;
        ss << pidx->to_string() << "\n";
        ss << values.to_string() << "\n";
        return ss.str();
    }

    friend std::ostream& operator<<(std::ostream& os, const Series& sr)
    {
        os << sr.to_string();
        return os;
    }

#include "pandas/series_op.tcc"
#include "pandas/series_picker.tcc"

    const DT& loc(const IT& id) const
    {
        return values.iloc(pidx->loc(id));
    }

    DT& loc_ref(const IT& id)
    {
        if (!pidx->has(id)) {
            pidx->_append(id);
            values._append(DT {});
        }

        return values.iloc_ref(pidx->loc(id));
    }

    DT& iloc_ref(int i)
    {
        return values.iloc_ref(i);
    }

    DT iloc(int i) const
    {
        return values.iloc(i);
    }

    SeriesPicker<IT, DT, INT, DNT> iloc(int bgn, int end, int step = 1) const
    {
        auto pvis = std::make_shared<Range<int>>(bgn, end, step);
        return SeriesPicker<IT, DT, INT, DNT>(*this, pvis);
    }

    SeriesPicker<IT, DT, INT, DNT> iloc(int bgn, int end, int step = 1)
    {
        auto pvis = std::make_shared<Range<int>>(bgn, end, step);
        return SeriesPicker<IT, DT, INT, DNT>(*this, pvis);
    }

    SeriesPicker<IT, DT, INT, DNT> loc(const DT& bgn, const DT& end)
    {
        auto pvis = pidx->loc(bgn, end);
        return SeriesPicker<IT, DT, INT, DNT>(*this, pvis);
    }

    template <class INT2>
    SeriesPicker<IT, DT, INT, DNT> loc(const Index<IT, INT2>& idx)
    {
        std::vector<int> iids;
        for (int i = 0; i < idx.size(); i++) {
            IT& id = idx.iloc(i);
            if (!has(id)) {
                throw std::format("key not found {}", id.to_string());
            }
            iids.push_back(pidx->loc(id));
        }

        return SeriesPicker<IT, DT, INT, DNT>(*this, iids);
    }

    template <class INT2, class DNT2>
    SeriesPicker<IT, DT, INT, DNT> loc(const Series<IT, bool, INT2, DNT2>& mask)
    {
        if (mask.size() != size()) {
            throw std::format("size not match: {}!={}", mask.size(), size());
        }
        std::vector<int> iids;

        for (int i = 0; i < mask.size(); i++) {
            Bool flag = mask.iloc(i);
            IT& id = mask.pidx->iloc(i);
            if (flag == true) {
                iids.push_back(pidx->loc(id));
            }
        }

        return SeriesPicker<IT, DT, INT, DNT>(*this, iids);
    }

    Series sort_index(bool ascending = true) const
    {
        using Pair = std::tuple<IT, DT>;

        std::vector<Pair> ps;
        for (int i = 0; i < size(); i++) {
            IT id = pidx->iloc(i);
            DT val = values.iloc(i);
            ps.push_back(std::make_tuple<IT, DT>(std::move(id), std::move(val)));
        }

        std::sort(ps.begin(), ps.end(), [&](const Pair& pa, const Pair& pb) -> bool {
            if (ascending) {
                return std::get<0>(pa) < std::get<0>(pb);
            } else {
                return std::get<0>(pb) < std::get<0>(pa);
            }
        });

        Series res;
        for (int i = 0; i < ps.size(); i++) {
            const IT& id = std::get<0>(ps[i]);
            const DT& val = std::get<1>(ps[i]);
            res.append(id, val);
        }
        return res;
    }

    Series sort_values(bool ascending = true) const
    {
        using Pair = std::tuple<IT, DT>;

        std::vector<Pair> ps;
        for (int i = 0; i < size(); i++) {
            IT id = pidx->iloc(i);
            DT val = values.iloc(i);
            ps.push_back(std::make_tuple<IT, DT>(std::move(id), std::move(val)));
        }

        std::sort(ps.begin(), ps.end(), [&](const Pair& pa, const Pair& pb) -> bool {
            if (ascending) {
                return std::get<1>(pa) < std::get<1>(pb);
            } else {
                return std::get<1>(pb) < std::get<1>(pa);
            }
        });

        Series res;
        for (int i = 0; i < ps.size(); i++) {
            const IT& id = std::get<0>(ps[i]);
            const DT& val = std::get<1>(ps[i]);
            res.append(id, val);
        }
        return res;
    }

    Series shift(int offset) const
    {
        Series res = *this;
        for (int i = 0; i < size(); i++) {
            res.iloc(i) = DT {};
        }

        if (abs(offset) >= size()) {
            // nothing to do
        } else if (offset > 0) {
            for (int i = offset; i < size(); i++) {
                res.iloc(i) = iloc(i - offset);
            }

        } else if (offset < 0) {
            for (int i = 0; i < size() + offset; i++) {
                res.iloc(i) = iloc(i - offset);
            }
        }
        return res;
    }

    Series dropna() const
    {
        Series res;
        for (int i = 0; i < size(); i++) {
            if (isnan(iloc(i))) {
                continue;
            }
            const IT& id = pidx->iloc(i);
            const DT& val = values.iloc(i);
            res.append(id, val);
        }
        return res;
    }

    template <class DT2>
    Series fillna(const DT2& v) const
    {
        Series res;
        for (int i = 0; i < size(); i++) {
            const IT& id = pidx->iloc(i);
            const DT& val = values.iloc(i);
            if (isnan(val)) {
                res.append(id, v);
            } else {
                res.append(id, val);
            }
        }
        return res;
    }

#include "pandas/series_functional.tcc"
#include "pandas/series_group.tcc"
#include "pandas/series_rolling.tcc"
};

template <
    class IT1, class DT1, class INT1, class DNT1,
    class IT2, class DT2, class INT2, class DNT2>
Series<IT1, DT1, INT1, DNT1> concat_0(
    const Series<IT1, DT1, INT1, DNT1>& sr1,
    const Series<IT2, DT2, INT2, DNT2>& sr2)
{
    Series<IT1, DT1, INT1, DNT1> sr = sr1;
    for (int i = 0; i < sr2.size(); i++) {
        const IT2& id = sr2.pidx->iloc(i);
        const DT2& val = sr2.iloc(i);
        sr1.append(id, val);
    }
    return sr1;
}

}