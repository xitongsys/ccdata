#pragma once

#include <algorithm>
#include <format>
#include <map>
#include <ostream>
#include <set>
#include <sstream>
#include <vector>

#include <boost/unordered_map.hpp>

#include "pandas/array.h"
#include "pandas/datetime.h"
#include "pandas/error.h"
#include "pandas/hash.h"
#include "pandas/index.h"
#include "pandas/ops.h"
#include "pandas/range.h"
#include "pandas/util.h"

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

    Series(const DNT& name, const Index<IT, INT>& idx, DT fillna_value)
    {
        this->pidx = std::make_shared<Index<IT, INT>>(idx);
        values._reserve(idx.size());
        for (int i = 0; i < idx.size(); i++) {
            values._append(fillna_value);
        }
        _rename(name);
    }

    Series(const DNT& name, Index<IT, INT>&& idx, DT fillna_value)
    {
        this->pidx = std::make_shared<Index<IT, INT>>(idx);
        values._reserve(idx.size());
        for (int i = 0; i < idx.size(); i++) {
            values._append(fillna_value);
        }
        _rename(name);
    }

    Series(const DNT& name, std::shared_ptr<Index<IT, INT>> pidx, DT fillna_value)
    {
        this->pidx = pidx;
        values._reserve(pidx->size());
        for (int i = 0; i < pidx->size(); i++) {
            values._append(fillna_value);
        }
        _rename(name);
    }

    Series(const Index<IT, INT>& idx, const Array<DT, DNT>& vals)
    {
        if (idx.size() != vals.size()) {
            PANDAS_THROW(std::format("index values size not match: {}!={}", pidx->size(), vals.size()));
        }

        this->pidx = std::make_shared<Index<IT, INT>>(idx);
        values = vals;
    }

    Series(std::shared_ptr<Index<IT, INT>> pidx, const Array<DT, DNT>& vals)
    {
        if (pidx->size() != vals.size()) {
            PANDAS_THROW(std::format("index values size not match: {}!={}", pidx->size(), vals.size()));
        }
        this->pidx = pidx;
        values = vals;
    }

    Series(std::shared_ptr<Index<IT, INT>> pidx, Array<DT, DNT>&& vals)
    {
        if (pidx->size() != vals.size()) {
            PANDAS_THROW(std::format("index values size not match: {}!={}", pidx->size(), vals.size()));
        }
        this->pidx = pidx;
        values = vals;
    }

    Series(Index<IT, INT>&& idx, Array<DT, DNT>&& vals)
    {
        if (idx.size() != vals.size()) {
            PANDAS_THROW(std::format("index values size not match: {}!={}", idx.size(), vals.size()));
        }

        this->pidx = std::make_shared<Index<IT, INT>>(idx);
        values = vals;
    }

    template <class IT2, class DT2, class INT2, class DNT2>
    Series(const Series<IT2, DT2, INT2, DNT2>& sr)
    {
        pidx = std::make_shared<Index<IT, INT>>(*sr.pidx);
        values = sr.values;
    }

    Series(const Series& sr)
    {
        pidx = sr.pidx;
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
        pidx = std::make_shared<Index<IT, INT>>(*pidx);
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
        pidx = sr.pidx;
        values = sr.values;
        return *this;
    }

    Series copy() const
    {
        Series sr = *this;
        sr.pidx = std::make_shared<Index<IT, INT>>(*pidx);
        return sr;
    }

    size_t size() const
    {
        return values.size();
    }

    DNT get_name() const
    {
        return values.get_name();
    }

    void _append(const IT& id, const DT& val, bool flush_index = true)
    {
        pidx->_append(id, flush_index);
        values._append(val);
    }

    template <class IT2>
    Series& _rename(const IT2& name)
    {
        values._rename(name);
        return *this;
    }

    template <class DNT2>
    Series<IT, DT, INT, DNT2> rename(const DNT2& name)
    {
        Array<DT, DNT2> ar_val(values.values, name);
        return Series<IT, DT, INT, DNT2>(*pidx, std::move(ar_val));
    }

    template <class IT2>
    Series<IT2, DT, INT, DNT> reindex(const std::vector<IT2>& index) const
    {
        Array<DT, DNT> ar(index.size(), DT {}, get_name());
        for (int i = 0; i < index.size(); i++) {
            IT2 id = index[i];
            DT val = DT {};

            if (pidx->has(id)) {
                val = loc(id);
            } else {
                val = pandas::nan<DT>();
            }
            ar.values[i] = val;
        }

        return Series<IT2, DT, INT, DNT>(Index<IT2, INT>(Array<IT2, INT>(index)), std::move(ar));
    }

    template <class IT2, class INT2>
    Series<IT2, DT, INT2, DNT> reindex(const Array<IT2, INT2>& index) const
    {
        return reindex(index.values);
    }

    template <class IT2, class INT2>
    Series<IT2, DT, INT2, DNT> reindex(const Index<IT2, INT2>& index) const
    {
        return reindex(index.values);
    }

    template <class IT2, class DT2, class INT2 = INT, class DNT2 = DNT>
    Series<IT2, DT2, INT2, DNT2> astype()
    {
        return Series<IT2, DT2, INT2, DNT2>(pidx->template astype<IT2, INT2>(), values.template astype<DT2, DNT2>());
    }

    std::string to_string(int mx_cnt = 10, bool tail = true) const
    {
        std::vector<std::string> idx_lines = pandas::split(pidx->to_string(mx_cnt, false), "\n");
        std::vector<std::string> val_lines = pandas::split(values.to_string(mx_cnt, false), "\n");
        if (idx_lines.size() != val_lines.size()) {
            PANDAS_THROW(std::format("size not match: {}!={}", idx_lines.size(), val_lines.size()));
        }
        std::vector<std::string> lines;
        for (int i = 0; i < idx_lines.size(); i++) {
            lines.push_back(idx_lines[i] + " " + val_lines[i]);
        }
        if (tail) {
            lines.push_back(std::format("[{} rows]", size()));
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

    /// @brief if id not exists, low perf
    /// @param id
    /// @return
    inline DT& loc_ref(const IT& id)
    {
        if (!pidx->has(id)) {
            pidx->_append(id, true);
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
        return SeriesVisitor<Range<int>>(*this, Range<int>(bgn, end, step));
    }

    SeriesVisitor<typename Index<IT, INT>::IndexRange> loc(const IT& bgn, const IT& end)
    {
        return SeriesVisitor<typename Index<IT, INT>::IndexRange>(*this, typename Index<IT, INT>::IndexRange(*pidx, bgn, end));
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
    SeriesVisitor<RangeVec<int>> loc(const std::vector<IT>& ids)
    {
        std::vector<int> iids;
        for (int i = 0; i < ids.size(); i++) {
            IT id = ids[i];
            int j = pidx->loc_i(id);
            iids.push_back(j);
        }
        return SeriesVisitor<RangeVec<int>>(*this, RangeVec(iids));
    }

    /// @iloc
    /// @tparam IT2
    /// @param iids
    /// @return
    template <class IT2>
    SeriesVisitor<RangeVec<int>> iloc(const std::vector<IT2>& iids)
    {
        return SeriesVisitor<RangeVec<int>>(*this, RangeVec(iids));
    }
    template <class IT2, class INT2>
    SeriesVisitor<RangeVec<int>> iloc(const Array<IT2, INT2>& iids)
    {
        return loc(iids.values);
    }
    template <class IT2, class DT2, class INT2, class DNT2>
    SeriesVisitor<RangeVec<int>> iloc(const Series<IT2, DT2, INT2, DNT2>& iids)
    {
        return loc(iids.values);
    }
    template <class IT2, class INT2>
    SeriesVisitor<RangeVec<int>> iloc(const Index<IT2, INT2>& iids)
    {
        return loc(iids.values);
    }
    SeriesVisitor<RangeVec<int>> iloc(const std::vector<int>& iids)
    {
        return SeriesVisitor<RangeVec<int>>(*this, RangeVec(iids));
    }

    /// @loc by mask
    /// @param mask
    /// @return
    template <class DT2>
    SeriesVisitor<RangeVec<int>> loc_mask(const std::vector<DT2>& mask)
    {
        if (mask.size() != size()) {
            PANDAS_THROW(std::format("size not match: {}!={}", mask.size(), size()));
        }
        std::vector<int> iids;
        iids.reserve(size());

        for (int i = 0; i < size(); i++) {
            if (mask[i]) {
                iids.push_back(i);
            }
        }
        return SeriesVisitor<RangeVec<int>>(*this, RangeVec<int>(iids));
    }
    template <class DT2, class NT2>
    SeriesVisitor<RangeVec<int>> loc_mask(const Array<DT2, NT2>& mask)
    {
        return loc_mask(mask.values);
    }
    template <class IT2, class DT2, class INT2, class DNT2>
    SeriesVisitor<RangeVec<int>> loc_mask(const Series<IT2, DT2, INT2, DNT2>& mask)
    {
        std::vector<int> iids;
        iids.reserve(size());

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
        Array<IT, INT> ar_idx(pidx->get_name());
        Array<DT, DNT> ar_val(get_name());
        ar_idx._reserve(size());
        ar_val._reserve(size());

        if (ascending) {
            for (int i = 0; i < size(); i++) {
                auto& p = pidx->value2iid[i];
                ar_idx._append(p.first);
                ar_val._append(iloc(p.second));
            }
            return Series(std::move(Index<IT, INT>(std::move(ar_idx), false)), std::move(ar_val));

        } else {
            for (int i = size() - 1; i >= 0; i--) {
                auto& p = pidx->value2iid[i];
                ar_idx._append(p.first);
                ar_val._append(iloc(p.second));
            }
            return Series(std::move(Index<IT, INT>(std::move(ar_idx), true)), std::move(ar_val));
        }
    }

    template <class DT2>
    Series sort_values(const std::vector<DT2>& keys, bool ascending = true) const
    {
        if (keys.size() != size()) {
            PANDAS_THROW(std::format("size not match: {}!={}", keys.size(), size()));
        }
        using Pair = std::tuple<IT, DT, DT2>;

        std::vector<Pair> ps;
        ps.reserve(size());

        for (int i = 0; i < size(); i++) {
            IT id = pidx->iloc(i);
            DT val = values.iloc(i);
            ps.push_back(std::tuple<IT, DT, DT2>(std::move(id), std::move(val), keys[i]));
        }

        std::sort(ps.begin(), ps.end(), [&](const Pair& pa, const Pair& pb) -> bool {
            if (ascending) {
                return std::get<2>(pa) < std::get<2>(pb);
            } else {
                return std::get<2>(pb) < std::get<2>(pa);
            }
        });

        Array<IT, INT> ar_idx;
        Array<DT, DNT> ar_val(get_name());
        for (int i = 0; i < ps.size(); i++) {
            const IT& id = std::get<0>(ps[i]);
            const DT& val = std::get<1>(ps[i]);
            ar_idx._append(id);
            ar_val._append(val);
        }
        return Series(std::move(Index<IT, INT>(std::move(ar_idx))), std::move(ar_val));
    }
    template <class DT2, class DNT2>
    Series sort_values(const Array<DT2, DNT2>& ar, bool ascending = true) const
    {
        return sort_values(ar.values, ascending);
    }
    template <class IT2, class DT2, class INT2, class DNT2>
    Series sort_values(const Series<IT2, DT2, INT2, DNT2>& sr, bool ascending = true) const
    {
        auto sr2 = sr.reindex(*pidx).dropna();
        return sort_values(sr2.values, ascending);
    }

    Series sort_values(bool ascending = true) const
    {
        return sort_values(values, ascending);
    }

    template <class DT2, class NT2, class DT3>
    Series where(const Array<DT2, NT2>& mask, DT3 v)
    {
        Series ds = copy();
        ds.loc_mask(!mask) = v;
        return ds;
    }

    template <class DT2, class DT3>
    Series where(const std::vector<DT2>& mask, DT3 v)
    {
        if (mask.size() != size()) {
            PANDAS_THROW(std::format("size not match: {}!={}", mask.size(), size()));
        }
        Array<DT2> mask_ar(mask);
        return where(mask_ar, v);
    }

    template <class IT2, class DT2, class INT2, class DNT2, class DT3>
    Series where(const Series<IT2, DT2, INT2, DNT2>& mask, DT3 v)
    {
        auto mask2 = mask.reindex(*pidx);
        return where(mask2.values, v);
    }

#include "pandas/series_functional.tcc"
#include "pandas/series_group.tcc"
#include "pandas/series_rolling.tcc"
};

}