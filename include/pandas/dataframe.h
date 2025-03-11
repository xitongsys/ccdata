#pragma once

#include <ostream>
#include <vector>

#include "pandas/array.h"
#include "pandas/pandastype.h"
#include "pandas/series.h"

namespace pandas {

template <class IT, class DT>
class DataFrame {
public:
    std::shared_ptr<Index<IT>> pidx;
    std::vector<Series<IT, DT>> values;

    DataFrame()
    {
    }

    Array<Str> columns() const
    {
        Array<Str> res;
        for (int i = 0; i < values.size(); i++) {
            res.append(values[i].name());
        }
        return res;
    }

    size_t size(int axis = 0) const
    {
        if (axis == 0) {
            return pidx->size();
        } else {
            return values.size();
        }
    }

    Index<IT> index() const
    {
        return *pidx;
    }

    Index<IT>& index()
    {
        return *pidx;
    }

    std::string to_string() const
    {
        std::stringstream ss;
        ss << "columns:[";
        for (int i = 0; i < size(1); i++) {
            ss << values[i].name() << ",";
        }
        ss << "]\n";
        for (int i = 0; i < size(); i++) {
            for (int j = 0; j < size(1); j++) {
                ss << values[j].iloc(i) << ",";
            }
            ss << "\n";
        }
        return ss.str();
    }

#define DEFINE_DATAFRAME_FUNCS(DT2, FUN)          \
    Series<Str, DT2> FUN()                        \
    {                                             \
        Index<Str> idx(columns());                \
        Array<DT2> vals;                          \
        for (int i = 0; i < values.size(); i++) { \
            vals.append(values[i].FUN());         \
        }                                         \
        return Series(idx, vals);                 \
    }

    DEFINE_DATAFRAME_FUNCS(Int, count)
    DEFINE_DATAFRAME_FUNCS(DT, sum)
    DEFINE_DATAFRAME_FUNCS(DT, max)
    DEFINE_DATAFRAME_FUNCS(DT, min)
    DEFINE_DATAFRAME_FUNCS(Double, mean)
    DEFINE_DATAFRAME_FUNCS(Double, var)
    DEFINE_DATAFRAME_FUNCS(Double, std)
    DEFINE_DATAFRAME_FUNCS(Double, median)
};

template <class IT, class DT>
DataFrame<IT, DT> concat(const Series<IT, DT>& sr1, const Series<IT, DT>& sr2)
{
    Index<IT> idx = concat(sr1.index(), sr2.index());

    Series<IT, DT> sr1_new = sr1.reindex(idx);
    Series<IT, DT> sr2_new = sr2.reindex(idx);

    DataFrame<IT, DT> df;
    df.pidx = std::make_shared<Index<IT>>(idx);
    sr1_new.pidx = df.pidx;
    sr2_new.pidx = df.pidx;

    df.values.emplace_back(std::move(sr1_new));
    df.values.emplace_back(std::move(sr2_new));

    return df;
}

}