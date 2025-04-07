#pragma once

// template<class IT, class DT, class INT, class NDT>
// class Series {

template <class KT>
class SeriesGroup {
public:
    Series& sr;
    std::map<KT, SeriesVisitor<RangeVec<int>>> items;

    SeriesGroup(Series& sr_)
        : sr(sr_)
    {
    }

    SeriesGroup(const SeriesGroup& sg)
        : sr(sg.sr)
    {
        items = sg.items;
    }

    SeriesGroup(SeriesGroup&& sg)
        : sr(sg.sr)
    {
        items = std::move(sg.items);
    }

    /////////////// agg ////////////////////////

    template <class DT2>
    Series<KT, DT2, INT, DNT> agg(std::function<DT2(SeriesVisitor<RangeVec<int>>&)> const& func)
    {
        Array<KT, INT> ar_idx;
        Array<DT2, DNT> ar_val(sr.get_name());
        for (auto it = items.begin(); it != items.end(); it++) {
            ar_idx._append(it->first);
            ar_val._append(func(it->second));
        }
        return Series<KT, DT2, INT, DNT>(std::move(Index<KT, INT>(std::move(ar_idx))), std::move(ar_val));
    }

#define DEFINE_SERIESGROUP_AGG_FUNC(TYPE, FUN)                                               \
    Series<KT, TYPE, INT, DNT> FUN()                                                         \
    {                                                                                        \
        return agg<TYPE>([](SeriesVisitor<RangeVec<int>>& sv) -> TYPE { return sv.FUN(); }); \
    }
    DEFINE_SERIESGROUP_AGG_FUNC(DT, sum)
    DEFINE_SERIESGROUP_AGG_FUNC(DT, max)
    DEFINE_SERIESGROUP_AGG_FUNC(DT, min)
    DEFINE_SERIESGROUP_AGG_FUNC(int, count)
    DEFINE_SERIESGROUP_AGG_FUNC(double, mean)

#define DEFINE_SERIESGROUP_AGG_FUNC(TYPE, FUN)                                                       \
    Series<KT, TYPE, INT, DNT> FUN(double ddof = 1)                                                  \
    {                                                                                                \
        return agg<TYPE>([ddof](SeriesVisitor<RangeVec<int>>& sv) -> TYPE { return sv.FUN(ddof); }); \
    }
    DEFINE_SERIESGROUP_AGG_FUNC(double, var)
    DEFINE_SERIESGROUP_AGG_FUNC(double, std)

    //////////// apply ////////////////////////////
    template <class IT2, class DT2, class INT2, class DNT2>
    Series<std::tuple<KT, IT2>, DT2, INT2, DNT2> apply(std::function<Series<IT2, DT2, INT2, DNT2>(SeriesVisitor<RangeVec<int>>&)> const& func)
    {
        Array<std::tuple<KT, IT2>, INT2> ar_idx;
        Array<DT2, DNT2> ar_val(sr.get_name());

        for (auto it = items.begin(); it != items.end(); it++) {
            KT key = it->first;
            auto ds = func(it->second);
            for (int i = 0; i < ds.size(); i++) {
                ar_idx._append(std::tuple(key, ds.pidx->iloc(i)));
                ar_val._append(ds.iloc(i));
            }
        }
        return Series<std::tuple<KT, IT2>, DT2, INT2, DNT2>(
            std::move(Index<std::tuple<KT, IT2>, INT2>(std::move(ar_idx))),
            std::move(ar_val));
    }
};

template <class KT>
SeriesGroup<KT> groupby(const std::vector<KT>& vs)
{
    if (size() != vs.size()) {
        PANDAS_THROW(std::format("size not match: {}!={}", vs.size(), size()));
    }

    SeriesGroup<KT> sg(*this);
    for (int i = 0; i < size(); i++) {
        const KT& key = vs[i];
        if (sg.items.count(key) == 0) {
            sg.items.emplace(key, SeriesVisitor<RangeVec<int>>(*this, RangeVec<int>()));
        }
        (sg.items.find(key)->second).it._append(i);
    }

    return sg;
}

template <class KT, class DNT2>
SeriesGroup<KT> groupby(const Array<KT, DNT2>& sr)
{
    return groupby(sr.values);
}

template <class IT2, class KT, class INT2, class DNT2>
SeriesGroup<KT> groupby(const Series<IT2, KT, INT2, DNT2>& sr)
{
    if (size() != sr.size()) {
        PANDAS_THROW(std::format("size not match: {}!={}", sr.size(), size()));
    }

    auto keys = sr.reindex(*pidx);
    return groupby(keys.values);
}
