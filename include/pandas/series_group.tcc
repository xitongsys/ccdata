#pragma once

// template<class IT, class DT, class INT, class NDT>
// class Series {

template <class KT>
class SeriesGroup {
public:
    std::map<KT, SeriesVisitor<RangeVec<int>>> items;

    SeriesGroup() { }

    SeriesGroup(const SeriesGroup& sg)
    {
        items = sg.items;
    }

    SeriesGroup(SeriesGroup&& sg)
    {
        items = std::move(sg.items);
    }

    template <class DT2>
    Series<KT, DT2, INT, DNT> agg(std::function<DT2(SeriesVisitor<RangeVec<int>>&)> const& func)
    {
        Series<KT, DT2, INT, DNT> res;
        for (auto it = items.begin(); it != items.end(); it++) {
            KT key = it->first;
            SeriesVisitor<RangeVec<int>>& sv = it->second;
            DT2 val = func(sv);
            res._append(key, val);
        }
        return res;
    }

#define DEFINE_SERIESGROUP_AGG_FUNC(TYPE, FUN)                                                                                          \
    Series<KT, TYPE, INT, DNT> FUN()                                                                                                    \
    {                                                                                                                                   \
        return agg<TYPE>([](SeriesVisitor<RangeVec<int>>& sv) -> TYPE { return pandas::FUN<TYPE, SeriesVisitor<RangeVec<int>>>(sv); }); \
    }
    DEFINE_SERIESGROUP_AGG_FUNC(DT, sum)
    DEFINE_SERIESGROUP_AGG_FUNC(DT, max)
    DEFINE_SERIESGROUP_AGG_FUNC(DT, min)
    DEFINE_SERIESGROUP_AGG_FUNC(int, count)
    DEFINE_SERIESGROUP_AGG_FUNC(double, mean)
    DEFINE_SERIESGROUP_AGG_FUNC(double, var)
    DEFINE_SERIESGROUP_AGG_FUNC(double, std)
};

template <class KT, class DNT2>
SeriesGroup<KT> groupby(const Array<KT, DNT2>& sr)
{
    if (size() != sr.size()) {
        throw std::format("size not match: {}!={}", sr.size(), size());
    }

    std::map<KT, std::vector<int>> iids_group;
    for (int i = 0; i < size(); i++) {
        const KT& key = sr.iloc(i);
        iids_group[key].push_back(i);
    }

    SeriesGroup<KT> sg;
    for (auto it = iids_group.begin(); it != iids_group.end(); it++) {
        auto sv = SeriesVisitor<RangeVec<int>>(*this, std::move(RangeVec<int>(std::move(it->second))));
        sg.items.emplace(it->first, std::move(sv));
    }

    return sg;
}

template <class IT2, class KT, class INT2, class DNT2>
SeriesGroup<KT> groupby(const Series<IT2, KT, INT2, DNT2>& sr)
{
    if (size() != sr.size()) {
        throw std::format("size not match: {}!={}", sr.size(), size());
    }

    std::map<KT, std::vector<int>> iids_group;
    for (int i = 0; i < size(); i++) {
        IT id = pidx->iloc(i);
        const KT& key = sr.loc(id);
        iids_group[key].push_back(i);
    }

    SeriesGroup<KT> sg;
    for (auto it = iids_group.begin(); it != iids_group.end(); it++) {
        auto sv = SeriesVisitor<RangeVec<int>>(*this, std::move(RangeVec<int>(std::move(it->second))));
        sg.items.emplace(it->first, std::move(sv));
    }

    return sg;
}
