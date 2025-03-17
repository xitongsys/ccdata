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
        pidx = std::make_shared<SingleIndex<IT>>();
    }

    DataFrame(const Index<IT>& idx)
    {
        pidx = std::make_shared<SingleIndex<IT>>(idx);
    }

    DataFrame(const std::vector<Series<IT, DT>>& srs)
    {
        for (int i = 0; i < srs.size(); i++) {
            *this = concat_1(*this, srs[i]);
        }
    }

    DataFrame<IT, DT> reindex(const Index<IT>& idx)
    {
        DataFrame df = DataFrame(idx);
        for (int i = 0; i < values.size(); i++) {
            Series<IT, DT> sr = values[i].reindex(idx);
            df.values.append(sr);
        }
        return df;
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

    int get_column_name_index(const std::string& name) const
    {
        for (int i = 0; i < values.size(); i++) {
            if (values[i].name() == name) {
                return i;
            }
        }
        return -1;
    }

    int append_row(const IT& id, const Array<DT>& ar)
    {
        if (ar.size() != values.size()) {
            throw std::format("row size not match: {}!={}", ar.size(), values.size());
        }

        if (pidx->has(id)) {
            throw std::format("duplicated id: {}", to_string(id));
        }

        pidx->append(id);
        for (int i = 0; i < values.size(); i++) {
            values[i].values.append(ar.iloc(i));
        }

        return size();
    }

    int append_row(const IT& id, const Series<std::string, DT>& sr)
    {
        if (sr.size() != values.size()) {
            throw std::format("row size not match: {}!={}", sr.size(), values.size());
        }

        if (pidx->has(id)) {
            throw std::format("duplicated id: {}", to_string(id));
        }

        pidx->append(id);
        for (int i = 0; i < values.size(); i++) {
            bool ok = false;
            for (int j = 0; j < sr.size(); j++) {
                if (sr.pidx->iloc(j) == values[i].name) {
                    values[i].values.append(sr.iloc(j));
                    ok = true;
                }
            }
            if (!ok) {
                throw std::format("{} not found", values[i].name());
            }
        }
        return size();
    }

    int append_col(const Array<DT>& ar)
    {
        if (ar.size() != size()) {
            throw std::format("append size not match: {}!={}", ar.size(), size());
        }
        if (get_column_name_index(ar.name) >= 0) {
            throw std::format("duplicated column name: {}", ar.name);
        }

        auto sr = Series(pidx, ar, ar.name);
        values.push_back(sr);
        return values.size();
    }

    int append_col(const Series<IT, DT>& sr)
    {
        if (get_column_name_index(sr.name()) >= 0) {
            throw std::format("duplicated column name: {}", sr.name);
        }
        for (int i = 0; i < sr.size(); i++) {
            pidx->append(sr.pidx->iloc(i));
        }

        for (int i = 0; i < values.size(); i++) {
            values[i] = values[i].reindex(*pidx);
            values[i].pidx = pidx;
        }

        auto sr2 = sr.reindex(*pidx);
        values.push_back(sr2);
        return values.size();
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
    if (sr1.name() == sr2.name()) {
        throw std::format("columns have same name: {}", sr1.name());
    }

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

template <class IT, class DT>
DataFrame<IT, DT> concat_1(const DataFrame<IT, DT>& df, const Series<IT, DT>& sr)
{
    for (int i = 0; i < df.values.size(); i++) {
        if (df.values[i].name() == sr.name()) {
            throw std::format("columns have same name: {}", sr.name());
        }
    }

    SingleIndex<IT> idx;
    for (int i = 0; i < df.size(); i++) {
        idx.append(df.pidx->iloc(i));
    }
    for (int i = 0; i < sr.size(); i++) {
        idx.append(sr.pidx->iloc(i));
    }

    DataFrame<IT, DT> df_new = df.reindex(idx);
    Series<IT, DT> sr_new = sr.reindex(idx);
    df_new.values.append(sr_new);

    return df_new;
}

}