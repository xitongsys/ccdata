#pragma once

// template<class IT, class DT>
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

    template <class DT2>
    DataFrame<IT, DT2> agg(std::function<DT2(const Visitor<DT>&)> const& func) const
    {
        DataFrame<IT, DT2> res(df.pidx->new_clone());
        for (int i = 0; i < df.values.size(); i++) {
            Series<IT, DT2> sr1 = df.values[i].rolling(window, min_periods).agg(func);
        }
        return res;
    }

#define DEFINE_SERIESROLLING_AGG_FUNC(TYPE, FUN)                                  \
    Series<IT, TYPE> FUN()                                                        \
    {                                                                             \
        return agg<TYPE>([](const Visitor<DT>& sr) -> TYPE { return sr.FUN(); }); \
    }
    DEFINE_SERIESROLLING_AGG_FUNC(DT, sum)
    DEFINE_SERIESROLLING_AGG_FUNC(DT, max)
    DEFINE_SERIESROLLING_AGG_FUNC(DT, min)
    DEFINE_SERIESROLLING_AGG_FUNC(Int, count)
    DEFINE_SERIESROLLING_AGG_FUNC(Double, mean)
    DEFINE_SERIESROLLING_AGG_FUNC(Double, var)
    DEFINE_SERIESROLLING_AGG_FUNC(Double, std)
    DEFINE_SERIESROLLING_AGG_FUNC(Double, median)
};

SeriesRolling rolling(int window, int min_periods)
{
    return SeriesRolling(*this, window, min_periods);
}
