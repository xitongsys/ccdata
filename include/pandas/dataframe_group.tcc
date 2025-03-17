#pragma once

// template<class IT, class DT>
// class DataFrame {

template <class KT>
class DataFrameGroup {
public:
    std::map<KT, DataFrame<IT, DT>> dfs;

    DataFrameGroup() { }

    DataFrameGroup(const DataFrameGroup& sg)
    {
    }

    DataFrameGroup(DataFrameGroup&& sg)
    {
    }

    void append(const KT& key, const DT& value)
    {
        if (srs.count(key) == 0) {
            srs[key] = Array<DT>();
        }
        srs[key].append(value);
    }

    template <class DT2>
    DataFrame<KT, DT2> agg(std::function<DT2(const Visitor<DT>&)> const& func)
    {
    }

#define DEFINE_SERIESGROUP_AGG_FUNC(TYPE, FUN)                                    \
    Series<KT, TYPE> FUN()                                                        \
    {                                                                             \
        return agg<TYPE>([](const Visitor<DT>& sr) -> TYPE { return sr.FUN(); }); \
    }
    DEFINE_SERIESGROUP_AGG_FUNC(DT, sum)
    DEFINE_SERIESGROUP_AGG_FUNC(DT, max)
    DEFINE_SERIESGROUP_AGG_FUNC(DT, min)
    DEFINE_SERIESGROUP_AGG_FUNC(int, count)
    DEFINE_SERIESGROUP_AGG_FUNC(double, mean)
    DEFINE_SERIESGROUP_AGG_FUNC(double, var)
    DEFINE_SERIESGROUP_AGG_FUNC(double, std)
};

template <class KT>
DataFrameGroup<KT> groupby(const Array<KT>& ar) const
{
    DataFrameGroup<KT> sg;

    if (size() != sr.size()) {
        throw std::format("size not match: {}!={}", sr.size(), size());
    }

    for (int i = 0; i < ar.size(); i++) {
        const KT& key = ar.iloc(i);
        const DT& val = iloc(i);
        sg.append(key, val);
    }
    return sg;
}

template <class KT>
DataFrameGroup<KT> groupby(const Series<IT, KT>& sr) const
{
    DataFrameGroup<KT> sg;

    if (size() != sr.size()) {
        throw std::format("size not match: {}!={}", sr.size(), size());
    }
    for (int i = 0; i < sr.size(); i++) {
        const IT& id = pidx->iloc(i);
        const KT& key = sr.iloc(i);
        sg.append(key, val);
    }
    return sg;
}
