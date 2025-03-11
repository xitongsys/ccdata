#pragma once

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
        pidx = std::make_shared<Index<IT>>();
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
        if (idx.size() != vals.size()) {
            throw std::format("index values size not match: {}!={}", idx.size(), vals.size());
        }
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

    Series(const Series& sr)
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

    Index<IT>& index()
    {
        return *pidx;
    }

    const Index<IT>& index() const
    {
        return *pidx;
    }

    size_t size() const
    {
        return values.size();
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
        return res;
    }

    std::string to_string() const
    {
        std::stringstream ss;
        for (int i = 0; i < size(); i++) {
            ss << pidx->iloc(i) << " " << values.iloc(i) << std::endl;
        }
        ss << "Name: " << values.name << " Size: " << size() << std::endl;
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

    DT iloc(int iid) const
    {
        if (iid >= size()) {
            throw std::format("index overflow {} > {}", iid, size());
        }

        return values.iloc(iid);
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
};
}