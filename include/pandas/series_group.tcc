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
            KT key = it->first;
            SeriesVisitor<RangeVec<int>>& sv = it->second;
            DT2 val = func(sv);
            ar_idx._append(key);
            ar_val._append(val);
        }
        return Series<KT, DT2, INT, DNT>(std::move(Index<KT, INT>(std::move(ar_idx))), std::move(ar_val));
    }

    Series<KT, DT, INT, DNT> sum()
    {
        Array<KT, INT> ar_idx;
        Array<DT, DNT> ar_val(sr.get_name());
        for (auto it = items.begin(); it != items.end(); it++) {
            KT key = it->first;
            SeriesVisitor<RangeVec<int>>& sv = it->second;
            sv.reset();
            DT s = 0;
            while (sv.has_left()) {
                s += sv.next();
            }

            ar_idx._append(key);
            ar_val._append(s);
        }
        return Series<KT, DT, INT, DNT>(std::move(Index<KT, INT>(std::move(ar_idx))), std::move(ar_val));
    }

#define DEFINE_SERIESGROUP_AGG_FUNC(TYPE, FUN)                                                           \
    Series<KT, TYPE, INT, DNT> FUN()                                                                     \
    {                                                                                                    \
        return agg<TYPE>([](SeriesVisitor<RangeVec<int>>& sv) -> TYPE { return sv.to_series().FUN(); }); \
    }
    //DEFINE_SERIESGROUP_AGG_FUNC(DT, sum)
    DEFINE_SERIESGROUP_AGG_FUNC(DT, max)
    DEFINE_SERIESGROUP_AGG_FUNC(DT, min)
    DEFINE_SERIESGROUP_AGG_FUNC(int, count)
    DEFINE_SERIESGROUP_AGG_FUNC(double, mean)
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
                IT2 it = ds.pidx->iloc(i);
                DT2 val = ds.iloc(i);
                ar_idx._append(std::tuple(key, it));
                ar_val._append(val);
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

    std::map<KT, std::vector<int>> iids_group;
    for (int i = 0; i < size(); i++) {
        const KT& key = vs[i];
        iids_group[key].push_back(i);
    }

    SeriesGroup<KT> sg(*this);
    for (auto it = iids_group.begin(); it != iids_group.end(); it++) {
        auto sv = SeriesVisitor<RangeVec<int>>(*this, std::move(RangeVec<int>(std::move(it->second))));
        sg.items.emplace(it->first, std::move(sv));
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

    std::map<KT, std::vector<int>> iids_group;
    for (int i = 0; i < size(); i++) {
        IT id = pidx->iloc(i);
        const KT& key = sr.loc(id);
        iids_group[key].push_back(i);
    }

    SeriesGroup<KT> sg(*this);
    for (auto it = iids_group.begin(); it != iids_group.end(); it++) {
        auto sv = SeriesVisitor<RangeVec<int>>(*this, std::move(RangeVec<int>(std::move(it->second))));
        sg.items.emplace(it->first, std::move(sv));
    }

    return sg;
}
