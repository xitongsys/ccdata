#pragma once

// template<class IT, class DT>
// class DataFrame {

template <class KT>
class DataFrameGroup {
public:
    using SV = typename Series<IT, DT, INT, DNT>::template SeriesVisitor<RangeVec<int>>;
    using DV = DataFrameVisitor<RangeVec<int>, Range<int>>;

    DataFrame& df;
    Array<KT> keys;

    DataFrameGroup(DataFrame& df_, const Array<KT>& keys_)
        : df(df_)
        , keys(keys_)
    {
    }

    DataFrameGroup(const DataFrameGroup& sg)
        : df(sg.df)
        , keys(sg.keys)
    {
    }

    DataFrameGroup(DataFrameGroup&& sg)
        : keys(std::move(sg.keys))
        , df(sg.df)
    {
    }

    //////////////////// agg ////////////////////////
    template <class DT2>
    DataFrame<KT, DT2, INT, DNT> agg(std::function<DT2(SV&)> const& func)
    {
        std::vector<Series<KT, DT2, INT, DNT>> srs;
        for (int j = 0; j < df.template size<1>(); j++) {
            srs.push_back(df.iloc_ref<1>(j).groupby(keys).agg(func));
        }

        return DataFrame<KT, DT2, INT, DNT>(srs);
    }

#define DEFINE_DATAFRAMEGROUP_AGG_FUNC(TYPE, FUN)                              \
    DataFrame<KT, TYPE, INT, DNT> FUN()                                        \
    {                                                                          \
        return agg<TYPE>([](SV& sr) -> TYPE { return sr.to_series().FUN(); }); \
    }
    DEFINE_DATAFRAMEGROUP_AGG_FUNC(DT, sum)
    DEFINE_DATAFRAMEGROUP_AGG_FUNC(DT, max)
    DEFINE_DATAFRAMEGROUP_AGG_FUNC(DT, min)
    DEFINE_DATAFRAMEGROUP_AGG_FUNC(double, count)
    DEFINE_DATAFRAMEGROUP_AGG_FUNC(double, mean)
    DEFINE_DATAFRAMEGROUP_AGG_FUNC(double, var)
    DEFINE_DATAFRAMEGROUP_AGG_FUNC(double, std)

    //////////////////// apply /////////////////////////////////
    template <class IT2, class DT2, class INT2, class DNT2>
    DataFrame<std::tuple<KT, IT2>, DT2, INT, DNT> apply(std::function<Series<IT2, DT2, INT2, DNT2>(SV&)> const& func)
    {
        std::vector<Series<std::tuple<KT, IT2>, DT2, INT, DNT>> srs;
        for (int j = 0; j < df.template size<1>(); j++) {
            srs.push_back(df.iloc_ref<1>(j).groupby(keys).apply(func));
        }

        return DataFrame<KT, DT2, INT, DNT>(srs);
    }
};

template <class KT>
DataFrameGroup<KT> groupby(const std::vector<KT>& vs)
{
    if (size<0>() != vs.size()) {
        PANDAS_THROW(std::format("size not match: {}!={}", vs.size(), size<0>()));
    }

    return DataFrameGroup<KT>(*this, Array<KT>(vs));
}

template <class KT, class NT2>
DataFrameGroup<KT> groupby(const Array<KT, NT2>& sr)
{
    return groupby(sr.values);
}

template <class IT2, class KT, class INT2, class DNT2>
DataFrameGroup<KT> groupby(const Series<IT2, KT, INT2, DNT2>& sr)
{
    if (sr.size() != size<0>()) {
        PANDAS_THROW(std::format("size not match: {}!={}", sr.size(), size<0>()));
    }

    for (int i = 0; i < size<0>; i++) {
        IT id = pidx->iloc(i);
        if (!sr.pidx->has(id)) {
            PANDAS_THROW(std::format("id not found: {}", pandas::to_string(id)));
        }
    }

    auto sr2 = sr.reindex(*pidx);

    return groupby(sr2.values);
}
