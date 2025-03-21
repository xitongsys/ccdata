#pragma once

#include <algorithm>
#include <map>
#include <ostream>
#include <set>
#include <sstream>
#include <vector>

#include "pandas/array.h"
#include "pandas/index.h"
#include "pandas/ops.h"
#include "pandas/pandastype.h"
#include "pandas/range.h"

namespace pandas {
// IT: index type, DT: data type, INT: index name type, DNT: data name type
template <class IT, class DT, class INT = std::string, class DNT = std::string>
class Series {
public:
    std::shared_ptr<Index<IT, INT>> pidx = nullptr;
    Array<DT, DNT> values;

    Series()
    {
        pidx = std::make_shared<Index<IT, INT>>();
    }

    Series(const DNT& name)
    {
        pidx = std::make_shared<Index<IT, INT>>();
        _rename(name);
    }

    Series(const Index<IT, INT>& idx)
    {
        this->pidx = std::make_shared<Index<IT, INT>>(idx);
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

    Series(const Index<IT, INT>& idx, const Array<DT, DNT>& vals)
    {
        if (idx.size() != vals.size()) {
            throw std::format("index values size not match: {}!={}", pidx->size(), vals.size());
        }

        this->pidx = std::make_shared<Index<IT, INT>>(idx);
        values = vals;
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
            values._append(pandas::nan<DT>());
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
        pidx = std::make_shared<Index<IT, INT>>(*sr.pidx);
        values = sr.values;
    }

    Series(const Series& sr)
    {
        pidx = std::make_shared<Index<IT, INT>>(*sr.pidx);
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
        pidx = std::make_shared<Index<IT, INT>>(*sr, pidx);
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
        pidx = std::make_shared<Index<IT, INT>>(*sr.pidx);
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

    template <class IT2>
    void _rename(const IT2& name)
    {
        values._rename(name);
    }

    Series rename(const std::string& name)
    {
        Series res = *this;
        res._rename(name);
        return res;
    }

    template <class IT2, class INT2>
    Series<IT2, DT, INT2, DNT> reindex(const Index<IT2, INT2>& index) const
    {
        Series<IT2, DT, INT2, DNT> res;

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

    std::string to_string(int mx_cnt = 10) const
    {
        std::vector<std::string> idx_lines = pandas::split(pidx->to_string(mx_cnt), "\n");
        std::vector<std::string> val_lines = pandas::split(values.to_string(mx_cnt), "\n");
        if (idx_lines.size() != val_lines.size()) {
            throw std::format("size not match: {}!={}", idx_lines.size(), val_lines.size());
        }
        std::vector<std::string> lines;
        for (int i = 0; i < idx_lines.size(); i++) {
            lines.push_back(idx_lines[i] + " " + val_lines[i]);
        }

        return pandas::line_width_adjust(pandas::join(lines, "\n"));
    }

    friend std::ostream& operator<<(std::ostream& os, const Series& sr)
    {
        os << sr.to_string();
        return os;
    }

#include "pandas/series_op.tcc"
#include "pandas/series_visitor.tcc"

    inline DT loc(const IT& id) const
    {
        return values.iloc(pidx->loc_i(id));
    }

    inline DT& loc_ref(const IT& id)
    {
        if (!pidx->has(id)) {
            pidx->_append(id);
            values._append(pandas::nan<DT>());
        }

        return values.iloc_ref(pidx->loc_i(id));
    }

    inline DT& iloc_ref(int i)
    {
        return values.iloc_ref(i);
    }

    inline DT iloc(int i) const
    {
        return values.iloc(i);
    }

    SeriesVisitor<Range<int>> iloc(int bgn, int end, int step = 1)
    {
        auto a = SeriesVisitor<typename Index<IT, INT>::IndexRange>(*this, Index<IT, INT>::IndexRange(*pidx, bgn, end));
        return SeriesVisitor<Range<int>>(*this, Range<int>(bgn, end, step));
    }

    SeriesVisitor<typename Index<IT, INT>::IndexRange> loc(const IT& bgn, const IT& end)
    {
        return SeriesVisitor<Index<IT, INT>::IndexRange>(*this, Index<IT, INT>::IndexRange(*pidx, bgn, end));
    }

    /// @loc by ids
    /// @tparam IT2
    /// @param ids
    /// @return
    template <class IT2>
    SeriesVisitor<RangeVec<int>> loc(const std::vector<IT2>& ids)
    {
        std::vector<int> iids;
        for (int i = 0; i < ids.size(); i++) {
            IT2 id = ids[i];
            int j = pidx->loc_i(id);
            iids.push_back(j);
        }
        return SeriesVisitor<RangeVec<int>>(*this, RangeVec(iids));
    }
    template <class IT2, class INT2>
    SeriesVisitor<RangeVec<int>> loc(const Array<IT2, INT2>& ids)
    {
        return loc(ids.values);
    }
    template <class IT2, class DT2, class INT2, class DNT2>
    SeriesVisitor<RangeVec<int>> loc(const Series<IT2, DT2, INT2, DNT2>& ids)
    {
        return loc(ids.values);
    }
    template <class IT2, class INT2>
    SeriesVisitor<RangeVec<int>> loc(const Index<IT2, INT2>& ids)
    {
        return loc(ids.values);
    }

    /// @loc by mask
    /// @param mask
    /// @return
    template <>
    SeriesVisitor<RangeVec<int>> loc(const std::vector<bool>& mask)
    {
        if (mask.size() != size()) {
            throw std::format("size not match: {}!={}", mask.size(), size());
        }
        std::vector<int> iids;
        for (int i = 0; i < size(); i++) {
            if (mask[i]) {
                iids.push_back(i);
            }
        }
        return SeriesVisitor<RangeVec<int>>(*this, RangeVec<int>(iids));
    }
    template <class NT2>
    SeriesVisitor<RangeVec<int>> loc(const Array<bool, NT2>& mask)
    {
        return loc(mask.values);
    }
    template <class IT2, class INT2, class DNT2>
    SeriesVisitor<RangeVec<int>> loc(const Series<IT2, bool, INT2, DNT2>& mask)
    {
        std::vector<int> iids;
        for (int i = 0; i < size(); i++) {
            IT id = pidx->iloc(i);
            if (mask.loc(id)) {
                iids.push_back(i);
            }
        }
        return SeriesVisitor<RangeVec<int>>(*this, RangeVec<int>(iids));
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
            res._append(id, val);
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
            res._append(id, val);
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
            res._append(id, val);
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
        sr1._append(id, val);
    }
    return sr1;
}

}