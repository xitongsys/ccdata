#pragma once

#include <ostream>
#include <vector>

#include "pandas/array.h"
#include "pandas/index.h"
#include "pandas/pandastype.h"
#include "pandas/series.h"
#include "pandas/singleindex.h"

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
        for (int i = 0; i < size(1); i++) {
            ss << pandas::to_string(values[i]) << "\n";
        }
        return ss.str();
    }

#define DEFINE_DATAFRAME_FUNCS(DT2, FUN)          \
    Series<std::string, DT2> FUN()                \
    {                                             \
        SingleIndex<std::string> idx(columns());  \
        Array<DT2> vals;                          \
        for (int i = 0; i < values.size(); i++) { \
            vals.append(values[i].FUN());         \
        }                                         \
        return Series(idx, vals);                 \
    }

    DEFINE_DATAFRAME_FUNCS(int, count)
    DEFINE_DATAFRAME_FUNCS(DT, sum)
    DEFINE_DATAFRAME_FUNCS(DT, max)
    DEFINE_DATAFRAME_FUNCS(DT, min)
    DEFINE_DATAFRAME_FUNCS(double, mean)
    DEFINE_DATAFRAME_FUNCS(double, var)
    DEFINE_DATAFRAME_FUNCS(double, std)


    friend std::ostream& operator<<(std::ostream& os, const DataFrame& df)
    {
        os << df.to_string();
        return os;
    }
};

template <class IT, class DT>
DataFrame<IT, DT> concat_1(const Series<IT, DT>& sr1, const Series<IT, DT>& sr2)
{
    SingleIndex<IT> idx;
    for (int i = 0; i < sr1.size(); i++) {
        idx.append(sr1.pidx->iloc(i));
    }
    for (int i = 0; i < sr2.size(); i++) {
        idx.append(sr2.pidx->iloc(i));
    }

    Series<IT, DT> sr1_new = sr1.reindex(idx);
    Series<IT, DT> sr2_new = sr2.reindex(idx);

    DataFrame<IT, DT> df;
    df.pidx = std::make_shared<SingleIndex<IT>>(idx);
    sr1_new.pidx = df.pidx;
    sr2_new.pidx = df.pidx;

    df.values.emplace_back(std::move(sr1_new));
    df.values.emplace_back(std::move(sr2_new));

    return df;
}

}