#pragma once

// template<class IT, class DT>
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
    Series<IT, DT2> agg(std::function<DT2(const Series&)> const& func) const
    {
        Series<IT, DT2> res(sr.pidx->new_empty());
        for (int i = 0; i < sr.size(); i++) {
            const IT& id = sr.pidx->iloc(i);
            int b = std::max(0, i - window + 1), e = i;
            Series ds = sr.iloc(b, e + 1).to_series();
            if (ds.count() < min_periods) {
                res.append(id, DT2 {});
            } else {
                res.append(id, func(ds));
            }
        }
        return res;
    }

#define DEFINE_SERIESROLLING_AGG_FUNC(TYPE, FUN)                             \
    Series<IT, TYPE> FUN()                                                   \
    {                                                                        \
        return agg<TYPE>([](const Series& sr) -> TYPE { return sr.FUN(); }); \
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
