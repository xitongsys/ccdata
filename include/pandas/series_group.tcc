#pragma once

// template<class IT, class DT, class INT, class NDT>
// class Series {

template <class KT>
class SeriesGroup {
public:
    std::map<KT, Series> srs;

    SeriesGroup() { }

    SeriesGroup(const SeriesGroup& sg)
    {
        srs = sg.srs;
    }

    SeriesGroup(SeriesGroup&& sg)
    {
        srs = std::move(sg.srs);
    }

    void _append(const KT& key, const IT& id, const DT& value)
    {
        if (srs.count(key) == 0) {
            srs[key] = Series();
        }
        srs[key]._append(id, value);
    }

    template <class DT2>
    Series<KT, DT2, INT, DNT> agg(std::function<DT2(SeriesVisitor<Range<int>>&)> const& func)
    {
        Series<KT, DT2, INT, DNT> res;
        for (auto it = srs.begin(); it != srs.end(); it++) {
            KT key = it->first;
            SeriesVisitor<Range<int>> sv(it->second, Range<int>(0, it->second.size()));
            DT2 val = func(sv);
            res._append(key, val);
        }
        return res;
    }

#define DEFINE_SERIESGROUP_AGG_FUNC(TYPE, FUN)                                                                                    \
    Series<KT, TYPE, INT, DNT> FUN()                                                                                              \
    {                                                                                                                             \
        return agg<TYPE>([](SeriesVisitor<Range<int>>& sv) -> TYPE { return pandas::FUN<TYPE, SeriesVisitor<Range<int>>>(sv); }); \
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
    SeriesGroup<KT> sg;

    if (size() != sr.size()) {
        throw std::format("size not match: {}!={}", sr.size(), size());
    }
    for (int i = 0; i < sr.size(); i++) {
        IT id = pidx->iloc(i);
        DT val = iloc(i);
        KT key = sr.iloc(i);
        sg._append(key, id, val);
    }
    return sg;
}

template <class IT2, class KT, class INT2, class DNT2>
SeriesGroup<KT> groupby(const Series<IT2, KT, INT2, DNT2>& sr)
{
    SeriesGroup<KT> sg;

    if (size() != sr.size()) {
        throw std::format("size not match: {}!={}", sr.size(), size());
    }
    for (int i = 0; i < size(); i++) {
        const IT& id = pidx->iloc(i);
        const DT& val = iloc(i);
        const KT& key = sr.loc(id);
        sg._append(key, id, val);
    }
    return sg;
}
