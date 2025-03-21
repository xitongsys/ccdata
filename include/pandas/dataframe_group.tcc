#pragma once

// template<class IT, class DT>
// class DataFrame {

template <class KT>
class DataFrameGroup {
public:
    using SV = Series<IT, DT, INT, DNT>::SeriesVisitor<RangeVec<int>>;
    using DV = DataFrameVisitor<RangeVec<int>, Range<int>>;

    std::map<KT, DV> items;

    DataFrameGroup() { }

    DataFrameGroup(const DataFrameGroup& sg)
        : items(sg.items)
    {
    }

    DataFrameGroup(DataFrameGroup&& sg)
        : items(std::move(sg.items))
    {
    }

    template <class DT2>
    DataFrame<KT, DT2, INT, DNT> agg(std::function<DT2(SV&)> const& func)
    {
        std::vector<DNT> col_names;
        std::vector<Array<DT2, KT>> rows;
        for (auto it = items.begin(); it != items.end(); it++) {
            KT key = it->first;
            DV& dv = it->second;

            if (col_names.size() == 0) {
                col_names = dv.columns();
            }

            Array<DT2, KT> row(key);
            std::vector<SV> svs = dv.to_series_visitors();
            for (auto& sv : svs) {
                DT2 v = func(sv);
                row._append(v);
            }
            rows.emplace_back(row);
        }

        return DataFrame<KT, DT2, INT, DNT>(col_names, rows);
    }

#define DEFINE_SERIESGROUP_AGG_FUNC(TYPE, FUN)                     \
    DataFrame<KT, TYPE, INT, DNT> FUN()                            \
    {                                                              \
        return agg<TYPE>([](SV& sr) -> TYPE { return sr.FUN(); }); \
    }
    DEFINE_SERIESGROUP_AGG_FUNC(DT, sum)
    DEFINE_SERIESGROUP_AGG_FUNC(DT, max)
    DEFINE_SERIESGROUP_AGG_FUNC(DT, min)
    DEFINE_SERIESGROUP_AGG_FUNC(int, count)
    DEFINE_SERIESGROUP_AGG_FUNC(double, mean)
    DEFINE_SERIESGROUP_AGG_FUNC(double, var)
    DEFINE_SERIESGROUP_AGG_FUNC(double, std)
};

template <class KT, class NT2>
DataFrameGroup<KT> groupby(const Array<KT, NT2>& sr)
{
    if (size() != sr.size()) {
        throw std::format("size not match: {}!={}", sr.size(), size());
    }

    std::map<KT, std::vector<int>> iids_group;
    for (int i = 0; i < size(); i++) {
        const KT& key = sr.iloc(i);
        iids_group[key].push_back(i);
    }

    DataFrameGroup<KT> dg;
    for (auto it = iids_group.begin(); it != iids_group.end(); it++) {
        auto dv = DataFrameVisitor<RangeVec<int>, Range<int>>(*this, RangeVec<int>(std::move(it->second)), Range<int>(0, size(1)));
        dg.items.emplace(it->first, std::move(dv));
    }

    return dg;
}

template <class KT, class INT2, class DNT2>
DataFrameGroup<KT> groupby(const Series<IT, KT, INT2, DNT2>& sr)
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

    DataFrameGroup<KT> dg;
    for (auto it = iids_group.begin(); it != iids_group.end(); it++) {
        auto dv = DataFrameVisitor<RangeVec<int>, Range<int>>(*this, RangeVec<int>(std::move(it->second)), Range<int>(0, size(1)));
        dg.items.emplace(it->first, std::move(dv));
    }

    return dg;
}
