#pragma once

// template<class IT, class DT, class INT, class DNT>
// class DataFrame {

template <class DT2>
DataFrame<IT, DT2, INT, DNT> map(std::function<DT2(const DT&)> const& func) const
{
    std::vector<Series<IT, DT2, INT, DNT>> srs;
    for (int j = 0; j < size<1>(); j++) {
        srs.push_back(iloc<1>(j).map(func));
    }
    return DataFrame<IT, DT2, INT, DNT>(srs);
}

/// @dropna
/// @param how
DataFrame dropna(const std::string& how = "any")
{
    Series<IT, int, INT, DNT> ds_cnt = count<1>();

    if (how == "all") {
        Series<IT, bool, INT, DNT> mask = (ds_cnt > 0);
        return loc(mask).to_frame();

    } else if (how == "any") {
        Series<IT, bool, INT, DNT> mask = (ds_cnt == size<1>());
        return loc(mask).to_frame();
    }
}
void _dropna(const std::string& how = "any")
{
    *this = dropna(how);
}

/// @diff
/// @param periods
void _diff(int periods)
{
    for (auto& sr : values) {
        sr._diff(periods);
    }
}
DataFrame diff(int periods) const
{
    DataFrame df = this->copy();
    df._diff(periods);
    return df;
}

/// @shift
/// @param offset
void _shift(int offset)
{
    for (auto& sr : values) {
        sr._shift(offset);
    }
}
DataFrame shift(int offset) const
{
    DataFrame df = this->copy();
    df._shift(offset);
    return df;
}

/// @fillna
/// @tparam DT2
/// @param v
template <class DT2>
void _fillna(const DT2& v)
{
    for (auto& sr : values) {
        sr._fillna(v);
    }
}
template <class DT2>
DataFrame fillna(const DT2& v) const
{
    DataFrame df = this->copy();
    df._fillna(v);
    return df;
}

/// @ffill
/// @param limit
void _ffill(int limit)
{
    for (auto& sr : values) {
        sr._ffill(limit);
    }
}
DataFrame ffill(int limit) const
{
    DataFrame df = this->copy();
    df._ffill(limit);
    return df;
}

/// @bfill
/// @param limit
void _bfill(int limit)
{
    for (auto& sr : values) {
        sr._bfill(limit);
    }
}
DataFrame bfill(int limit) const
{
    DataFrame df = this->copy();
    df._bfill(limit);
    return df;
}

/// @cumsum
void _cumsum()
{
    for (auto& sr : values) {
        sr._cumsum();
    }
}
DataFrame cumsum() const
{
    DataFrame df = this->copy();
    df._cumsum();
    return df;
}

/// @pow
void _pow(double n)
{
    for (auto& sr : values) {
        sr._pow(n);
    }
}
DataFrame pow(double n) const
{
    DataFrame df = this->copy();
    df._pow(n);
    return df;
}

/// @replace
template <class DT2, class DT3>
void _replace(const DT2& v_old, const DT3& v_new)
{
    for (auto& sr : values) {
        sr._replace(v_old, v_new);
    }
}
template <class DT2, class DT3>
DataFrame replace(const DT2& v_old, const DT3& v_new)
{
    DataFrame df = this->copy();
    df._replace(v_old, v_new);
    return df;
}

/// @T
/// @return
DataFrame<DNT, DT, std::string, IT> T() const
{
    std::vector<IT> cols;
    for (int i = 0; i < size<0>(); i++) {
        IT id = pidx->iloc(i);
        cols.push_back(id);
    }

    std::vector<Array<DT, DNT>> rows;
    for (int j = 0; j < size<1>(); j++) {
        Array<DT, DNT> row(iloc<1>(j).get_name());
        for (int i = 0; i < size<0>(); i++) {
            DT val = iloc(i, j);
            row._append(val);
        }
        rows.emplace_back(std::move(row));
    }

    return DataFrame<DNT, DT, std::string, IT>(cols, rows);
}

#define DEFINE_DATAFRAME_FUNCS(DT2, FUN)              \
    template <int axis>                               \
    auto FUN()                                        \
    {                                                 \
        if constexpr (axis == 0) {                    \
            Index<DNT> idx(columns());                \
            Array<DT2> vals;                          \
            for (int i = 0; i < values.size(); i++) { \
                vals._append(values[i].FUN());        \
            }                                         \
            vals._rename(#FUN);                       \
            return Series<DNT, DT2>(idx, vals);       \
        } else {                                      \
            return T().FUN<0>();                      \
        }                                             \
    }

DEFINE_DATAFRAME_FUNCS(int, count)
DEFINE_DATAFRAME_FUNCS(DT, sum)
DEFINE_DATAFRAME_FUNCS(DT, max)
DEFINE_DATAFRAME_FUNCS(DT, min)
DEFINE_DATAFRAME_FUNCS(double, mean)
DEFINE_DATAFRAME_FUNCS(double, var)
DEFINE_DATAFRAME_FUNCS(double, std)
DEFINE_DATAFRAME_FUNCS(double, median)

template <int axis>
auto corr()
{
    if constexpr (axis == 1) {
        DataFrame<DNT, double, std::string, DNT> df_corr(columns(), columns());
        for (int i = 0; i < size<1>(); i++) {
            for (int j = i; j < size<1>(); j++) {
                double c = iloc_ref<1>(i).values.corr(iloc_ref<1>(j).values);
                df_corr.iloc_ref(i, j) = c;
                df_corr.iloc_ref(j, i) = c;
            }
        }
        return df_corr;

    } else {
        return T().corr<1>();
    }
}
