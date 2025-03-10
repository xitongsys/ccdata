#pragma once

#include <vector>

#include "pandas/array.h"
#include "pandas/index.h"
#include "pandas/pandastype.h"

namespace pandas {

template <class IT = Int, class DT>
class Series {
public:
    std::shared_ptr<Index<IT>> pidx = nullptr;
    Array<DT> values;

    Series()
    {
    }

    Series(const Array<DT>& vals)
    {
        pidx = std::make_shared<Index<Int>>();
        values = vals;

        for (int i = 0; i < vals.size(); i++) {
            pidx->append(i);
        }
    }

    Series(const Index<IT>& idx, const Array<DT>& vals)
    {
        pidx = std::make_shared<Index<IT>>(idx);
        values = vals;
    }

    void append(const IT& id, const DT& val)
    {
        pidx->append(id);
        values.append(val);
    }

    template <class IT2, class DT2>
    Series(const Series<IT2, DT2>& sr)
    {
        pidx = std::make_shared<Index<IT>>(*sr.pidx);
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
        pidx = std::make_shared<Index<IT>>(*sr.pidx);
        values = sr.values;
        return *this;
    }

    Series& operator=(Series&& sr)
    {
        pidx = sr.pidx;
        values = std::move(sr.values);
        return *this;
    }

    Index& index()
    {
        return *pidx;
    }

    size_t size() const
    {
        return values.size();
    }

    template <class IT2>
    Series<IT2, DT> reindex(const Index<IT2>& index)
    {
        Series<IT2, DT> res;

        for (int i = 0; i < index.size(); i++) {
            IT2& id = index.iloc(i);
            DT val = DT {};
            if (pidx->has((IT)(id))) {
                val = loc((IT)(id));
            }
            res.append(id, val);
        }
        return res;
    }

    DT& loc(const IT& id)
    {
        if (!pidx->has(id)) {
            pidx->append(id);
            values.append(T {});
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
};
}