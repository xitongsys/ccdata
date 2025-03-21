#pragma once

// template<class IT, class DT, class INT, class DNT>
// class DataFrame {

class DataFrameRolling {
public:
    DataFrame& df;
    int window, min_periods;
    DataFrameRolling(DataFrame& df_, int window_, int min_periods_)
        : df(df_)
        , window(window_)
        , min_periods(min_periods_)
    {
    }

    using SV = typename Series<IT, DT, INT, DNT>::template SeriesVisitor<Range<int>>;

    template <class DT2>
    DataFrame<IT, DT2, INT, DNT> agg(const std::function<DT2(SV&)>& func)
    {
        DataFrame<IT, DT2, INT, DNT> res(*df.pidx);
        for (int i = 0; i < df.values.size(); i++) {
            Series<IT, DT2, INT, DNT> sr1 = df.values[i].rolling(window, min_periods).agg(func);
            sr1.pidx = res.pidx;
            res.values.push_back(sr1);
        }
        return res;
    }

#define DEFINE_DATAFRAME_ROLLING_FUNC(TYPE, FUN)                                              \
    DataFrame<IT, TYPE, INT, DNT> FUN()                                                       \
    {                                                                                         \
        DataFrame<IT, TYPE, INT, DNT> res(*df.pidx);                                          \
        for (int i = 0; i < df.values.size(); i++) {                                          \
            Series<IT, TYPE, INT, DNT> sr1 = df.values[i].rolling(window, min_periods).FUN(); \
            sr1.pidx = res.pidx;                                                              \
            res.values.push_back(sr1);                                                        \
        }                                                                                     \
        return res;                                                                           \
    }

    DEFINE_DATAFRAME_ROLLING_FUNC(DT, sum)
    DEFINE_DATAFRAME_ROLLING_FUNC(DT, max)
    DEFINE_DATAFRAME_ROLLING_FUNC(DT, min)
    DEFINE_DATAFRAME_ROLLING_FUNC(int, count)
    DEFINE_DATAFRAME_ROLLING_FUNC(double, mean)
    DEFINE_DATAFRAME_ROLLING_FUNC(double, var)
    DEFINE_DATAFRAME_ROLLING_FUNC(double, std)
    DEFINE_DATAFRAME_ROLLING_FUNC(double, median)
};

DataFrameRolling rolling(int window, int min_periods)
{
    return DataFrameRolling(*this, window, min_periods);
}
