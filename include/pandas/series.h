#pragma once

#include <algorithm>
#include <ostream>
#include <sstream>
#include <vector>

#include "pandas/array.h"
#include "pandas/index.h"
#include "pandas/pandastype.h"

namespace pandas {

template <class IT, class DT>
class Series {
public:
    std::shared_ptr<Index<IT>> pidx = nullptr;
    Array<DT> values;

    Series()
    {
    }

    Series(std::shared_ptr<Index<IT>> pidx)
    {
        this->pidx = pidx;
        for (int i = 0; i < pidx->size(); i++) {
            values.append(DT {});
        }
    }

    Series(std::shared_ptr<Index<IT>> pidx, const Array<DT>& vals)
    {
        if (pidx->size() != vals.size()) {
            throw std::format("index values size not match: {}!={}", pidx->size(), vals.size());
        }
        this->pidx = pidx;
        values = vals;
    }

    template <class T>
    Series(const T& idx)
    {
        auto ptr = std::make_shared<T>(idx);
        pidx = std::static_pointer_cast<Index<IT>>(ptr);
        for (int i = 0; i < pidx->size(); i++) {
            values.append(DT {});
        }
    }

    template <class T>
    Series(const T& idx, const Array<DT>& vals)
    {
        if (idx.size() != vals.size()) {
            throw std::format("index values size not match: {}!={}", idx.size(), vals.size());
        }
        auto ptr = std::make_shared<T>(idx);
        pidx = std::static_pointer_cast<Index<IT>>(ptr);
        values = vals;
    }

    void append(const IT& id, const DT& val)
    {
        if (pidx->has(id)) {
            throw std::format("append failed: duplicated key");
        }
        pidx->append(id);
        values.append(val);
    }

    template <class IT2, class DT2>
    Series(const Series<IT2, DT2>& sr)
    {
        pidx = sr.pidx->clone();
        values = sr.values;
    }

    Series(const Series& sr)
    {
        pidx = sr.pidx->clone();
        values = sr.values;
    }

    Series(Series&& sr)
    {
        pidx = sr.pidx;
        values = std::move(sr.values);
    }

    template <class IT2, class DT2>
    Series& operator=(const Series<IT2, DT2>& sr)
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

    std::string name() const
    {
        return values.name;
    }

    void set_name(const std::string& name)
    {
        values.name = name;
    }

    template <class IT2>
    Series<IT2, DT> reindex(const Index<IT2>& index) const
    {
        Series<IT2, DT> res;

        for (int i = 0; i < index.size(); i++) {

            IT2 id = index.iloc(i);
            DT val = DT {};

            if (pidx->has((IT)(id))) {
                val = loc((IT)(id));
            }
            res.append(id, val);
        }
        res.set_name(name());
        return res;
    }

    template <class IT2, class DT2>
    Series<IT2, DT2> astype()
    {
        auto idx = pidx->astype<IT2>();
        auto vals = values.astype<DT2>();

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

    DT& loc(const IT& id)
    {
        if (!pidx->has(id)) {
            pidx->append(id);
            values.append(DT {});
        }

        return values.iloc(pidx->loc(id));
    }

    DT& iloc(int iid)
    {
        if (iid >= size()) {
            throw std::format("index overflow {} > {}", iid, size());
        }

        return values.iloc(iid);
    }

    const DT& iloc(int iid) const
    {
        if (iid >= size()) {
            throw std::format("index overflow {} > {}", iid, size());
        }

        return values.iloc(iid);
    }

    SeriesPicker<IT, DT> iloc(int bgn, int end, int step = 1) const
    {
        return SeriesPicker<IT, DT>(*this, range(bgn, end, step));
    }

    SeriesPicker<IT, DT> iloc(int bgn, int end, int step = 1)
    {
        return SeriesPicker<IT, DT>(*this, range(bgn, end, step));
    }

    SeriesPicker<IT, DT> loc(const DT& bgn, const DT& end)
    {
        std::vector<int> iids = pidx->loc(bgn, end);
        return SeriesPicker<IT, DT>(*this, iids);
    }

    SeriesPicker<IT, DT> loc(const Index<IT>& idx)
    {
        std::vector<int> iids;
        for (int i = 0; i < idx.size(); i++) {
            IT& id = idx.iloc(i);
            if (!has(id)) {
                throw std::format("key not found {}", id.to_string());
            }
            iids.push_back(pidx->loc(id));
        }

        return SeriesPicker<IT, DT>(*this, iids);
    }

    SeriesPicker<IT, DT> loc(const Series<IT, Bool>& mask)
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

        return SeriesPicker<IT, DT>(*this, iids);
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
            if (iloc(i).isnan()) {
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
            if (val.isnan()) {
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
}