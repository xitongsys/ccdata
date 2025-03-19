#pragma once

// template<class IT, class DT, class INT, class DNT>
// class Series {

class SeriesRolling {
public:
    Series& sr;
    int window, min_periods;
    SeriesRolling(Series& sr_, int window_, int min_periods_)
        : sr(sr_)
        , window(window_)
        , min_periods(min_periods_)
    {
    }

    template <class DT2>
    Series<IT, DT2, INT, DNT> agg(std::function<DT2(SeriesVisitor<Range<int>>&)> const& func)
    {
        Series<IT, DT2, INT, DNT> res = sr;
        for (int i = 0; i < sr.size(); i++) {
            int b = std::max(0, i - window + 1), e = i;
            SeriesVisitor<Range<int>> sv(sr, Range<int>(b, e));
            res.iloc_ref(i) = func(sv);
        }
        return res;
    }

#define DEFINE_SERIESROLLING_AGG_FUNC(TYPE, FUN)                                                                                  \
    Series<IT, TYPE, INT, DNT> FUN()                                                                                              \
    {                                                                                                                             \
        return agg<TYPE>([](SeriesVisitor<Range<int>>& sv) -> TYPE { return pandas::FUN<TYPE, SeriesVisitor<Range<int>>>(sv); }); \
    }
    DEFINE_SERIESROLLING_AGG_FUNC(DT, sum)
    DEFINE_SERIESROLLING_AGG_FUNC(DT, max)
    DEFINE_SERIESROLLING_AGG_FUNC(DT, min)
    DEFINE_SERIESROLLING_AGG_FUNC(double, mean)
    DEFINE_SERIESROLLING_AGG_FUNC(double, var)
    DEFINE_SERIESROLLING_AGG_FUNC(double, std)
};

SeriesRolling rolling(int window, int min_periods)
{
    return SeriesRolling(*this, window, min_periods);
}