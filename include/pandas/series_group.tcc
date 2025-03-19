#pragma once

// template<class IT, class DT, class INT, class NDT>
// class Series {

template <class KT>
class SeriesGroup {
public:
    std::map<KT, Array<DT, DNT>> srs;

    SeriesGroup() { }

    SeriesGroup(const SeriesGroup& sg)
    {
        srs = sg.srs;
    }

    SeriesGroup(SeriesGroup&& sg)
    {
        srs = std::move(sg.srs);
    }

    void append(const KT& key, const DT& value)
    {
        if (srs.count(key) == 0) {
            srs[key] = Array<DT, DNT>();
        }
        srs[key].append(value);
    }

    template <class DT2>
    Series<KT, DT2, INT, DNT> agg(std::function<DT2(Visitor<DT>&)> const& func)
    {
        SingleIndex<KT, INT> id;
        Series<KT, DT2, INT, DNT> res(id);

        for (auto it = srs.begin(); it != srs.end(); it++) {
            KT key = it->first;
            //Array<DT, DNT>& sr = it->second;
            //DT2 value = func(sr);
            //res._append(key, value);
        }

        return res;
    }

#define DEFINE_SERIESGROUP_AGG_FUNC(TYPE, FUN)                                    \
    Series<KT, TYPE> FUN()                                                        \
    {                                                                             \
        return agg<TYPE>([](Visitor<DT>& sr) -> TYPE { return sr.FUN(); }); \
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
        const KT& key = sr.iloc(i);
        const DT& val = iloc(i);
        sg.append(key, val);
    }
    return sg;
}

template <class KT, class INT2, class DNT2>
SeriesGroup<KT> groupby(const Series<IT, KT, INT2, DNT2>& sr) 
{
    SeriesGroup<KT> sg;

    if (size() != sr.size()) {
        throw std::format("size not match: {}!={}", sr.size(), size());
    }
    for (int i = 0; i < sr.size(); i++) {
        const IT& id = pidx->iloc(i);
        const DT& val = loc(id);
        const KT& key = sr.iloc(i);
        sg.append(key, val);
    }
    return sg;
}
