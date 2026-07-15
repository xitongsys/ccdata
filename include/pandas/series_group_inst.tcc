#pragma once

// template<class IT, class DT, class INT, class NDT>
// class Series {

template <class KT>
class SeriesGroupInst {
public:
    Series& sr;
    boost::unordered_flat_map<KT, Series> items;

    SeriesGroupInst(Series& sr_)
        : sr(sr_)
    {
    }

    SeriesGroupInst(const SeriesGroupInst& sg)
        : sr(sg.sr), items(sg.items)
    {
    }

    SeriesGroupInst(SeriesGroupInst&& sg)
        : sr(sg.sr), items(std::move(sg.items))
    {
    }

    /////////////// agg ////////////////////////

    template <class DT2>
    Series<KT, DT2, INT, DNT> agg(std::function<DT2(Series&)> const& func)
    {
        Array<KT, INT> ar_idx;
        Array<DT2, DNT> ar_val(sr.get_name());
        for (auto it = items.begin(); it != items.end(); it++) {
            ar_idx._append(it->first);
            ar_val._append(func(it->second));
        }
        return Series<KT, DT2, INT, DNT>(std::move(Index<KT, INT>(std::move(ar_idx))), std::move(ar_val));
    }

#define DEFINE_SERIESGROUPINST_AGG_FUNC(TYPE, FUN)                                           \
    Series<KT, TYPE, INT, DNT> FUN()                                                         \
    {                                                                                        \
        return agg<TYPE>([](Series& sv) -> TYPE { return sv.FUN(); });                       \
    }
    DEFINE_SERIESGROUPINST_AGG_FUNC(DT, sum)
    DEFINE_SERIESGROUPINST_AGG_FUNC(DT, max)
    DEFINE_SERIESGROUPINST_AGG_FUNC(DT, min)
    DEFINE_SERIESGROUPINST_AGG_FUNC(double, count)
    DEFINE_SERIESGROUPINST_AGG_FUNC(double, mean)

#define DEFINE_SERIESGROUPINST_AGG_FUNC(TYPE, FUN)                                                   \
    Series<KT, TYPE, INT, DNT> FUN(double ddof = 1)                                                  \
    {                                                                                                \
        return agg<TYPE>([ddof](Series& sv) -> TYPE { return sv.FUN(ddof); });                       \
    }
    DEFINE_SERIESGROUPINST_AGG_FUNC(double, var)
    DEFINE_SERIESGROUPINST_AGG_FUNC(double, std)

    //////////// apply ////////////////////////////
    template <class IT2, class DT2, class INT2, class DNT2>
    Series<std::tuple<KT, IT2>, DT2, INT2, DNT2> apply(std::function<Series<IT2, DT2, INT2, DNT2>(Series&)> const& func)
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
SeriesGroupInst<KT> groupbyinst(const std::vector<KT>& vs, bool flush_index = true)
{
    if (size() != vs.size()) {
        PANDAS_THROW(std::format("size not match: {}!={}", vs.size(), size()));
    }

    boost::unordered_flat_map<KT,std::pair<Array<IT,INT>, Array<DT,DNT>>> mp;

    SeriesGroupInst<KT> sg(*this);
    for (int i = 0; i < size(); i++) {
        const KT& key = vs[i];
        auto [it, inserted] = mp.try_emplace(key, std::pair<Array<IT,INT>,Array<DT,DNT>>(Array<IT,INT>(pidx->get_name()), Array<DT,DNT>(get_name())));
        it->second.first._append(this->pidx->iloc(i));
        it->second.second._append(this->values.iloc(i));
    }

    for(auto it=mp.begin(); it!=mp.end(); it++) {
        Series sr(
            std::move(Index<IT, INT>(std::move(it->second.first), flush_index)),
            std::move(it->second.second)
        );
        sg.items.emplace(it->first, std::move(sr));
    }

    return sg;
}

template <class KT, class DNT2>
SeriesGroupInst<KT> groupbyinst(const Array<KT, DNT2>& sr)
{
    return groupbyinst(sr.values);
}

template <class IT2, class KT, class INT2, class DNT2>
SeriesGroupInst<KT> groupbyinst(const Series<IT2, KT, INT2, DNT2>& sr)
{
    if (size() != sr.size()) {
        PANDAS_THROW(std::format("size not match: {}!={}", sr.size(), size()));
    }

    auto keys = sr.reindex(*pidx);
    return groupbyinst(keys.values);
}
