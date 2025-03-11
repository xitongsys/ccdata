#pragma once

// template<class IT, class DT>
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

    void append(const KT& key, const IT& id, const DT& value)
    {
        if (srs.count(key) == 0) {
            srs[key] = Series();
        }
        srs[key].append(id, value);
    }

    template <class DT2>
    Series<KT, DT2> agg(std::function<DT2(const Series&)> const& func)
    {
        Series<KT, DT2> res;
        for (auto it = srs.begin(); it != srs.end(); it++) {
            KT key = it->first;
            Series& sr = it->second;
            DT2 value = func(sr);
            res.append(key, value);
        }

        return res;
    }

#define DEFINE_SERIESGROUP_AGG_FUNC(TYPE, FUN)                               \
    Series<KT, TYPE> FUN()                                                   \
    {                                                                        \
        return agg<TYPE>([](const Series& sr) -> TYPE { return sr.FUN(); }); \
    }
    DEFINE_SERIESGROUP_AGG_FUNC(DT, sum)
    DEFINE_SERIESGROUP_AGG_FUNC(DT, max)
    DEFINE_SERIESGROUP_AGG_FUNC(DT, min)
    DEFINE_SERIESGROUP_AGG_FUNC(Int, size)
    DEFINE_SERIESGROUP_AGG_FUNC(Int, count)
    DEFINE_SERIESGROUP_AGG_FUNC(Double, mean)
    DEFINE_SERIESGROUP_AGG_FUNC(Double, var)
    DEFINE_SERIESGROUP_AGG_FUNC(Double, std)
    DEFINE_SERIESGROUP_AGG_FUNC(Double, median)
};

template <class KT>
SeriesGroup<KT> groupby(const Array<KT>& sr) const
{
    SeriesGroup<KT> sg;

    if (size() != sr.size()) {
        throw std::format("size not match: {}!={}", sr.size(), size());
    }
    for (int i = 0; i < sr.size(); i++) {
        const IT& id = pidx->iloc(i);
        const KT& key = sr.iloc(i);
        const DT& val = iloc(i);

        sg.append(key, id, val);
    }
    return sg;
}

template <class KT>
SeriesGroup<KT> groupby(const Series<IT, KT>& sr) const
{
    SeriesGroup<KT> sg;

    if (size() != sr.size()) {
        throw std::format("size not match: {}!={}", sr.size(), size());
    }
    for (int i = 0; i < sr.size(); i++) {
        const IT& id = sr.pidx->iloc(i);
        const KT& key = sr.iloc(i);
        const DT& val = loc(id);

        sg.append(key, id, val);
    }
    return sg;
}
