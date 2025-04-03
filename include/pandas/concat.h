#pragma once

#include "pandas/dataframe.h"
#include "pandas/index.h"
#include "pandas/series.h"

namespace pandas {

/// @concat Array
/// @return
template <int axis, class IT1, class NT1>
auto concat(const Array<IT1, NT1>& idx1)
{
    if constexpr (axis == 0) {
        return idx1;

    } else if constexpr (axis == 1) {
        Array<std::tuple<IT1>, NT1> idx;
        idx._rename(idx1.get_name());
        for (int i = 0; i < idx1.size(); i++) {
            idx._append(std::tuple<IT1>(idx1.iloc(i)));
        }
        return idx;
    }
}
template <int axis,
    class IT1, class NT1,
    class IT2, class NT2,
    class... Ts>
auto concat(
    const Array<IT1, NT1>& idx1,
    const Array<IT2, NT2>& idx2,
    const Ts&... idxs)
{
    if constexpr (axis == 0) {
        Array<IT1, NT1> idx = idx1;
        Array<IT2, NT2> idx_tail = concat<0>(idx2, idxs...);

        for (int i = 0; i < idx_tail.size(); i++) {
            const IT2& id = idx_tail.iloc(i);
            idx._append(id);
        }
        return idx;

    } else if constexpr (axis == 1) {
        auto idx_tail = concat<1>(idx2, idxs...);
        if (idx1.size() != idx_tail.size()) {
            PANDAS_THROW(std::format("size not match: {}!={}", idx1.size(), idx_tail.size()));
        }
        using NEW_IT = std::remove_reference<decltype(add_first_element(idx1.iloc(0), idx_tail.iloc(0)))>::type;
        Array<NEW_IT, NT1> idx(idx1.get_name());

        for (int i = 0; i < idx1.size(); i++) {
            IT1 id1 = idx1.iloc(i);
            auto id_tail = idx_tail.iloc(i);
            auto new_id = add_first_element(id1, id_tail);
            idx._append(new_id);
        }
        return idx;

    } else {
        PANDAS_THROW(std::format("axis not supported: {}", axis));
        return -1;
    }
}
template <int axis, class IT, class NT>
auto concat(const std::vector<Array<IT, NT>>& idss)
{
    if constexpr (axis == 0) {
        Array<IT, NT> idx;
        for (auto& ids : idss) {
            for (int i = 0; i < ids.size(); i++) {
                const IT& id = ids.iloc(i);
                if (!idx.has(id)) {
                    idx._append(id);
                }
            }
        }
        return idx;

    } else {
        PANDAS_THROW(std::format("axis not supported: {}", axis));
        return -1;
    }
}

/// @concat Index
/// @return
template <int axis, class IT1, class NT1>
auto concat(const Index<IT1, NT1>& idx1)
{
    if constexpr (axis == 0) {
        return idx1;

    } else if constexpr (axis == 1) {
        Array<std::tuple<IT1>> ar_idx(idx1.get_name());
        for (int i = 0; i < idx1.size(); i++) {
            ar_idx._append(std::tuple<IT1>(idx1.iloc(i)));
        }
        return Index<std::tuple<IT1>, NT1>(std::move(ar_idx));
    }
}
template <int axis,
    class IT1, class NT1,
    class IT2, class NT2,
    class... Ts>
auto concat(
    const Index<IT1, NT1>& idx1,
    const Index<IT2, NT2>& idx2,
    const Ts&... idxs)
{
    if constexpr (axis == 0) {
        Index<IT2, NT2> idx_tail = concat<0>(idx2, idxs...);
        Array<IT1, NT1> ar_idx = idx1.values;

        for (int i = 0; i < idx_tail.size(); i++) {
            const IT2& id = idx_tail.iloc(i);
            ar_idx._append(id);
        }
        return Index<IT1, NT1>(std::move(ar_idx));

    } else if constexpr (axis == 1) {
        auto idx_tail = concat<1>(idx2, idxs...);
        if (idx1.size() != idx_tail.size()) {
            PANDAS_THROW(std::format("size not match: {}!={}", idx1.size(), idx_tail.size()));
        }
        using NEW_IT = std::remove_reference<decltype(add_first_element(idx1.iloc(0), idx_tail.iloc(0)))>::type;
        Array<NEW_IT, NT1> ar_idx(idx1.get_name());

        for (int i = 0; i < idx1.size(); i++) {
            IT1 id1 = idx1.iloc(i);
            auto id_tail = idx_tail.iloc(i);
            auto new_id = add_first_element(id1, id_tail);
            ar_idx._append(new_id);
        }
        return Index<NEW_IT, NT1>(std::move(ar_idx));

    } else {
        PANDAS_THROW(std::format("axis not supported: {}", axis));
        return -1;
    }
}
template <int axis, class IT, class NT>
auto concat(const std::vector<Index<IT, NT>>& idss)
{
    if constexpr (axis == 0) {
        Array<IT, NT> ar_idx;
        for (auto& ids : idss) {
            for (int i = 0; i < ids.size(); i++) {
                const IT& id = ids.iloc(i);
                ar_idx._append(id);
            }
        }
        return Index<IT, NT>(std::move(ar_idx));

    } else {
        PANDAS_THROW(std::format("axis not supported: {}", axis));
        return -1;
    }
}

/// @concat Series
/// @return
template <int axis, class IT1, class DT1, class INT1, class DNT1>
auto concat(const Series<IT1, DT1, INT1, DNT1>& sr1)
{
    if constexpr (axis == 0) {
        return sr1;

    } else if constexpr (axis == 1) {
        return DataFrame<IT1, DT1, INT1, DNT1>({ sr1 });

    } else {
        PANDAS_THROW(std::format("axis not supported: {}", axis));
    }
}
template <int axis,
    class IT1, class DT1, class INT1, class DNT1,
    class IT2, class DT2, class INT2, class DNT2,
    class... Ts>
auto concat(
    const Series<IT1, DT1, INT1, DNT1>& sr1,
    const Series<IT2, DT2, INT2, DNT2>& sr2,
    const Ts&... srs)
{
    if constexpr (axis == 0) {
        Series<IT1, DT1, INT1, DNT1> sr = sr1.copy();
        Series<IT2, DT2, INT2, DNT2> sr_tail = concat<0>(sr2, srs...);

        Array<IT1, INT1> ar_idx = sr.pidx->values;
        Array<DT1, DNT1> ar_val = sr.values;

        for (int i = 0; i < sr_tail.size(); i++) {
            const IT2& id = sr_tail.pidx->iloc(i);
            ar_idx._append(id);
            ar_val._append(sr_tail.iloc(i));
        }

        return Series<IT1, DT1, INT1, DNT1>(
            std::move(Index<IT1, INT1>(std::move(ar_idx))),
            std::move(ar_val));

    } else if constexpr (axis == 1) {
        DataFrame<IT2, DT2, INT2, DNT2> df_tail = concat<1>(sr2, srs...);
        Index<IT1, INT1> idx = concat<0>(*sr1.pidx, *df_tail.pidx).drop_duplicates("first");
        idx._rename(sr1.get_name());
        DataFrame<IT1, DT1, INT1, DNT1> df({ sr1.reindex(idx) });

        for (int j = 0; j < df_tail.size<1>(); j++) {
            df._append_col(df_tail.iloc<1>(j));
        }

        return df;

    } else {
        PANDAS_THROW(std::format("axis not supported: {}", axis));
        return -1;
    }
}
template <int axis, class IT, class DT, class INT, class DNT>
auto concat(const std::vector<Series<IT, DT, INT, DNT>>& srs)
{
    if constexpr (axis == 0) {
        Array<IT, INT> ar_idx;
        Array<DT, DNT> ar_val;

        for (auto& sr : srs) {
            int n = sr.size();
            for (int i = 0; i < n; i++) {
                IT id = sr.pidx->iloc(i);
                DT val = sr.iloc(i);
                ar_idx._append(id);
                ar_val._append(val);
            }
        }
        return Series<IT, DT, INT, DNT>(
            std::move(Index<IT, INT>(std::move(ar_idx))),
            std::move(ar_val));

    } else if constexpr (axis == 1) {
        return DataFrame<IT, DT, INT, DNT>(srs);

    } else {
        PANDAS_THROW(std::format("axis not supported: {}", axis));
        return -1;
    }
}

/// @concat DataFrame
/// @return
template <int axis, class IT1, class DT1, class INT1, class DNT1>
auto concat(const DataFrame<IT1, DT1, INT1, DNT1>& df1)
{
    if constexpr (axis == 0) {
        return df1;

    } else if constexpr (axis == 1) {
        return df1;

    } else {
        PANDAS_THROW(std::format("axis not supported: {}", axis));
    }
}
template <int axis,
    class IT1, class DT1, class INT1, class DNT1,
    class IT2, class DT2, class INT2, class DNT2,
    class... Ts>
auto concat(
    const DataFrame<IT1, DT1, INT1, DNT1>& df1,
    const DataFrame<IT2, DT2, INT2, DNT2>& df2,
    const Ts&... dfs)
{
    if constexpr (axis == 0) {
        DataFrame<IT2, DT2, INT2, DNT2> df_tail = concat<0>(df2, dfs...);
        auto cols = concat<0>(Index<DNT1>(df1.columns()), Index<DNT2>(df_tail.columns())).drop_duplicates("first");
        DataFrame<IT1, DT1, INT1, DNT1> df = df1.reindex<1>(cols);
        df_tail = df_tail.reindex<1>(cols);

        std::vector<Series<IT1, DT1, INT1, DNT1>> srs;
        for (int j = 0; j < cols.size(); j++) {
            srs.push_back(concat<0>(df.iloc_ref<1>(j), df_tail.iloc_ref<1>(j)));
        }

        return DataFrame(srs);

    } else if constexpr (axis == 1) {
        DataFrame<IT2, DT2, INT2, DNT2> df_tail = concat<1>(df2, dfs...);
        Index<IT1, INT1> idx = concat<0>(*df1.pidx, *df_tail.pidx).drop_duplicates("first");

        DataFrame<IT1, DT1, INT1, DNT1> df = df1.reindex<0>(idx);
        for (int j = 0; j < df_tail.size<1>(); j++) {
            df._append_col(df_tail.iloc<1>(j));
        }

        return df;

    } else {
        PANDAS_THROW(std::format("axis not supported: {}", axis));
        return -1;
    }
}
template <int axis, class IT, class DT, class INT, class DNT>
auto concat(const std::vector<DataFrame<IT, DT, INT, DNT>>& dfs)
{
    if constexpr (axis == 0) {
        std::vector<Array<IT, INT>> ar_idxs;
        std::vector<Array<DT, DNT>> ar_vals;
        std::map<DNT, int> col2id;

        for (const auto& df : dfs) {
            for (int j = 0; j < df.size<1>(); j++) {
                auto& sr = df.iloc_ref<1>(j);
                DNT col = sr.get_name();
                if (col2id.count(col) == 0) {
                    col2id[col] = ar_idxs.size();
                    ar_idxs.emplace_back(Array<IT, INT>());
                    ar_vals.emplace_back(Array<DT, DNT>(col));
                }
                int j2 = col2id[col];

                for (int i = 0; i < sr.size(); i++) {
                    ar_idxs[j2]._append(sr.pidx->iloc(i));
                    ar_vals[j2]._append(sr.iloc(i));
                }
            }
        }

        std::vector<Series<IT, DT, INT, DNT>> srs;
        for (int j = 0; j < ar_idxs.size(); j++) {
            srs.emplace_back(Series<IT, DT, INT, DNT>(
                std::move(Index<IT, INT>(std::move(ar_idxs[j]))),
                std::move(ar_vals[j])));
        }

        return DataFrame<IT, DT, INT, DNT>(srs);

    } else if constexpr (axis == 1) {
        std::vector<Series<IT, DT, INT, DNT>> srs;
        for (auto& df : dfs) {
            int c = df.size<1>();
            for (int j = 0; j < c; j++) {
                srs.push_back(df.iloc<1>(j));
            }
        }
        return DataFrame<IT, DT, INT, DNT>(srs);

    } else {
        PANDAS_THROW(std::format("axis not supported: {}", axis));
        return -1;
    }
}

}
