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

    Series<KT, Int> count()
    {
        return agg<Int>([](const Series& sr) -> Int { return Int(sr.count()); });
    }

    Series<KT, Int> size()
    {
        return agg<Int>([](const Series& sr) -> Int { return Int(sr.size()); });
    }

    Series<KT, DT> sum()
    {
        return agg<DT>([](const Series& sr) -> DT { return sr.sum(); });
    }
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
