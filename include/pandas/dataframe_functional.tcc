#pragma once

// template<class IT, class DT, class INT, class DNT>
// class DataFrame {

void _diff(int periods)
{
    for (auto& sr : values) {
        sr._diff(periods);
    }
}

DataFrame diff(int periods) const
{
    DataFrame df = *this;
    df._diff(periods);
    return df;
}

void _shift(int offset)
{
    for (auto& sr : values) {
        sr._shift(offset);
    }
}

DataFrame shift(int offset) const
{
    DataFrame df = *this;
    df._shift(offset);
    return df;
}

template <class DT2>
void _fillna(const DT2& v)
{
    for (auto& sr : values) {
        sr._fillna(v);
    }
}

template <class DT2>
void fillna(const DT2& v)
{
    DataFrame df = *this;
    df._fillna(v);
    return df;
}

#define DEFINE_DATAFRAME_FUNCS(DT2, FUN)          \
    Series<DNT, DT2> FUN()                        \
    {                                             \
        SingleIndex<DNT> idx(columns());          \
        Array<DT2> vals;                          \
        for (int i = 0; i < values.size(); i++) { \
            vals._append(values[i].FUN());        \
        }                                         \
        vals._rename(#FUN);                       \
        return Series<DNT, DT2>(idx, vals);       \
    }

DEFINE_DATAFRAME_FUNCS(int, count)
DEFINE_DATAFRAME_FUNCS(DT, sum)
DEFINE_DATAFRAME_FUNCS(DT, max)
DEFINE_DATAFRAME_FUNCS(DT, min)
DEFINE_DATAFRAME_FUNCS(double, mean)
DEFINE_DATAFRAME_FUNCS(double, var)
DEFINE_DATAFRAME_FUNCS(double, std)
