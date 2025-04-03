#pragma once

#include <ostream>
#include <string>
#include <vector>

#include "pandas/array.h"
#include "pandas/index.h"
#include "pandas/range.h"
#include "pandas/series.h"

namespace pandas {

template <class IT, class DT, class INT = std::string, class DNT = std::string>
class DataFrame {
public:
    std::shared_ptr<Index<IT, INT>> pidx;
    std::vector<Series<IT, DT, INT, DNT>> values;

#include "dataframe_visitor.tcc"

    DataFrame()
    {
        pidx = std::make_shared<Index<IT, INT>>();
    }

    DataFrame(const DataFrame& df)
    {
        pidx = std::make_shared<Index<IT, INT>>(*df.pidx);
        values = df.values;
    }

    DataFrame(DataFrame&& df)
    {
        pidx = df.pidx;
        values = std::move(df.values);
    }

    DataFrame& operator=(const DataFrame& df)
    {
        pidx = df.pidx;
        values = df.values;
        return *this;
    }

    DataFrame& operator=(DataFrame&& df)
    {
        pidx = df.pidx;
        values = std::move(df.values);
        return *this;
    }

    DataFrame(const Index<IT, INT>& idx)
    {
        pidx = std::make_shared<Index<IT, INT>>(idx);
    }

    DataFrame(const std::vector<IT>& ids, const std::vector<DNT>& columns, DT fillna_value)
    {
        pidx = std::make_shared<Index<IT, INT>>(ids);
        for (int j = 0; j < columns.size(); j++) {
            _append_col(Series<IT, DT, INT, DNT>(columns[j], pidx, fillna_value));
        }
    }

    DataFrame(const Index<IT, INT>& ids, const std::vector<DNT>& columns, DT fillna_value)
    {
        pidx = std::make_shared<Index<IT, INT>>(ids);
        for (int j = 0; j < columns.size(); j++) {
            _append_col(Series<IT, DT, INT, DNT>(columns[j], pidx, fillna_value));
        }
    }

    DataFrame(const std::vector<DNT>& columns, const std::vector<Array<DT, IT>>& rows)
    {
        pidx = std::make_shared<Index<IT, INT>>();
        for (int j = 0; j < columns.size(); j++) {
            values.push_back(Series<IT, DT, INT, DNT>(columns[j]));
        }

        for (int i = 0; i < rows.size(); i++) {
            const Array<DT, IT>& row = rows[i];
            IT id = row.get_name();
            pidx->_append(id, false);

            for (int j = 0; j < columns.size(); j++) {
                values[j].values._append(row.iloc(j));
            }
        }
        pidx->_reindex();

        reassign_index();
    }

    DataFrame(const Index<IT, INT>& idx, const std::vector<Array<DT, DNT>>& cols)
    {
        pidx = std::make_shared<Index<IT, INT>>(idx);
        for (auto& col : cols) {
            Series<IT, DT, INT, DNT> sr(pidx, col);
            values.emplace_back(std::move(sr));
        }
    }

    DataFrame(const std::vector<Series<IT, DT, INT, DNT>>& cols)
    {
        Array<IT, INT> ar_idx;
        std::set<IT> st;
        for (auto& sr : cols) {
            for (int i = 0; i < sr.size(); i++) {
                IT id = sr.pidx->iloc(i);
                if (st.count(id) == 0) {
                    ar_idx._append(id);
                    st.insert(id);
                }
            }
        }
        pidx = std::make_shared<Index<IT, INT>>(std::move(ar_idx));

        for (int i = 0; i < cols.size(); i++) {
            DNT col_name = cols[i].get_name();
            if (has<1>(col_name)) {
                PANDAS_THROW(std::format("duplicated column: {}", pandas::to_string(col_name)));
            }
            values.push_back(cols[i].reindex(*pidx));
            values[i].pidx = pidx;
        }
    }

    DataFrame copy() const
    {
        DataFrame df = *this;
        df.pidx = std::make_shared<Index<IT, INT>>(*pidx);
        df.reassign_index();
        return df;
    }

    void reassign_index()
    {
        for (auto& sr : values) {
            sr.pidx = pidx;
        }
    }

    template <int axis, class T2>
    auto reindex(const std::vector<T2>& ids) const
    {
        if constexpr (axis == 0) {
            std::vector<Series<T2, DT, INT, DNT>> srs;
            for (int j = 0; j < size<1>(); j++) {
                srs.push_back(iloc<1>(j).reindex(ids));
            }
            return DataFrame<T2, DT, INT, DNT>(srs);

        } else if constexpr (axis == 1) {
            std::vector<Series<IT, DT, INT, T2>> srs;
            for (const T2& col_name : ids) {
                int j = get_column_index(col_name);
                if (j < 0) {
                    Series<IT, DT, INT, T2> sr(col_name, *pidx, pandas::nan<DT>());
                    srs.push_back(sr);
                } else {
                    srs.push_back(iloc<1>(j).astype<IT, DT, INT, T2>());
                }
            }
            return DataFrame<IT, DT, INT, T2>(srs);

        } else {
            PANDAS_THROW(std::format("axis not supported: {}", axis));
        }
    }

    template <int axis, class IT2, class INT2>
    auto reindex(const Array<IT2, INT2>& ids) const
    {
        if constexpr (axis == 0) {
            auto df = reindex<0>(ids.values);
            df.pidx->_rename(ids.get_name());
            return df;

        } else if constexpr (axis == 1) {
            return reindex<1>(ids.values);

        } else {
            PANDAS_THROW(std::format("axis not supported: {}", axis));
        }
    }

    template <int axis, class IT2, class INT2>
    auto reindex(const Index<IT2, INT2>& ids) const
    {
        if constexpr (axis == 0) {
            auto df = reindex<0>(ids.values);
            df.pidx->_rename(ids.get_name());
            return df;

        } else if constexpr (axis == 1) {
            return reindex<1>(ids.values);

        } else {
            PANDAS_THROW(std::format("axis not supported: {}", axis));
        }
    }

    std::vector<DNT> columns() const
    {
        std::vector<DNT> res;
        for (int i = 0; i < values.size(); i++) {
            res.push_back(values[i].get_name());
        }
        return res;
    }

    template <int axis>
    size_t size() const
    {
        if constexpr (axis == 0) {
            return pidx->size();
        } else {
            return values.size();
        }
    }

    template <int axis, class T2>
    bool has(const T2& v)
    {
        if constexpr (axis == 0) {
            return pidx->has(v);

        } else if constexpr (axis == 1) {
            return get_column_index(v) >= 0;

        } else {
            PANDAS_THROW(std::format("axis not supported: {}", axis));
            return false;
        }
    }

    int get_column_index(const DNT& name) const
    {
        for (int i = 0; i < values.size(); i++) {
            if (values[i].get_name() == name) {
                return i;
            }
        }
        return -1;
    }

    template <class IT2, class DT2>
    int _append_row(const IT2& id, const std::vector<DT2>& row, bool reindex = true)
    {
        if (row.size() != size<1>()) {
            PANDAS_THROW(std::format("size not match: {}!={}", row.size(), size<1>()));
        }
        pidx->_append(id, reindex);
        for (int i = 0; i < row.size(); i++) {
            values[i].values._append(row[i]);
        }
        return size<0>();
    }
    template <class IT2, class DT2>
    int _append_row(const Array<DT2, IT2>& ar, bool reindex = true)
    {
        return _append_row(ar.get_name(), ar.values, reindex);
    }

    template <class INT2>
    int _append_row(const Series<DNT, DT, INT2, IT>& sr, bool reindex = true)
    {
        auto sr2 = sr.reindex(columns());
        return _append_row(sr.values, reindex);
    }

    int _append_col(const DNT& col_name, const DT& val)
    {
        if (has<1>(col_name)) {
            PANDAS_THROW(std::format("duplicated column: {}", pandas::to_string(col_name)));
        }
        std::vector<DT> vs;
        for (int i = 0; i < size<0>(); i++) {
            vs.push_back(val);
        }
        _append_col(col_name, vs);
        return size<1>();
    }

    template <class DT2, class DNT2>
    int _append_col(const DNT2& col_name, const std::vector<DT2>& col)
    {
        if (col.size() != size<0>()) {
            PANDAS_THROW(std::format("size not match: {}!={}", col.size(), size<0>()));
        }
        if (has<1>(col_name)) {
            PANDAS_THROW(std::format("duplicated column: {}", pandas::to_string(col_name)));
        }
        values.emplace_back(Series<IT, DT, INT, DNT>(pidx, Array<DT2, DNT2>(col, col_name)));
        return size<1>();
    }

    template <class DT2, class DNT2>
    int _append_col(const Array<DT2, DNT2>& ar)
    {
        return _append_col(ar.get_name(), ar.values);
    }

    template <class IT2, class DT2, class INT2, class DNT2>
    int _append_col(const Series<IT2, DT2, INT2, DNT2>& sr)
    {
        auto sr2 = sr.reindex(*pidx);
        return _append_col(sr2.values);
    }

    inline DT iloc(int ri, int ci) const
    {
        return values[ci].iloc(ri);
    }

    inline DT& iloc_ref(int ri, int ci)
    {
        return values[ci].iloc_ref(ri);
    }

    template <int axis>
    auto iloc(int i) const
    {
        if constexpr (axis == 0) {
            Series<DNT, DT, std::string, IT> row;
            IT id = pidx->iloc(i);
            for (int j = 0; j < size<1>(); j++) {
                row._append(values[j].get_name(), values[j].iloc(i));
            }
            row._rename(id);
            return row;

        } else {
            Series<IT, DT, INT, DNT> col = values[i];
            return col;
        }
    }

    template <int axis>
    auto& iloc_ref(int i)
    {
        if constexpr (axis == 0) {
            PANDAS_THROW(std::format("DataFrame not support iloc_ref for rows"));

        } else {
            Series<IT, DT, INT, DNT>& col = values[i];
            return col;
        }
    }

    template <int axis>
    auto& loc_ref(const DNT& col_name)
    {
        if constexpr (axis == 0) {
            PANDAS_THROW(std::format("DataFrame not support loc_ref for rows"));

        } else {
            int j = get_column_index(col_name);
            return iloc_ref<1>(j);
        }
    }

    template <int axis, class T2>
    auto loc(T2 key) const
    {
        if constexpr (axis == 0) {
            int i = pidx->loc(key);
            return iloc<0>(i);
        } else {
            int i = get_column_index(key);
            return iloc<1>(i);
        }
    }

    template <int axis>
    DataFrameVisitor<Range<int>, Range<int>> iloc(int bgn, int end, int step)
    {
        if constexpr (axis == 0) {
            Range<int> it_row(bgn, end, step);
            Range<int> it_col(0, size<1>(), 1);
            return DataFrameVisitor<Range<int>, Range<int>>(*this, it_row, it_col);

        } else {
            Range<int> it_row(0, size<0>(), step);
            Range<int> it_col(bgn, end, 1);
            return DataFrameVisitor<Range<int>, Range<int>>(*this, it_row, it_col);
        }
    }

    template <int axis, class KEY>
    auto loc(const KEY& bgn, const KEY& end)
    {
        using SIR = typename Index<IT, INT>::template IndexRange;

        if constexpr (axis == 0) {
            Range<int> it_col(0, size<1>(), 1);
            return DataFrameVisitor<SIR, Range<int>>(*this, SIR(*pidx, bgn, end), it_col);

        } else {
            std::vector<DNT> cols;
            for (int i = 0; i < size<1>(); i++) {
                DNT col = values[i].get_name();
                if (col >= bgn && col <= end) {
                    cols.push_back(col);
                }
            }

            Range<int> it_row(0, size<0>(), 1);
            RangeVec<DNT> it_rv;
            return DataFrameVisitor<Range<int>, RangeVec<DNT>>(*this, it_row, it_rv);
        }
    }

    /// @loc by ids
    /// @tparam IT2
    /// @tparam axis
    /// @param ids
    /// @return
    template <int axis, class IT2>
    auto loc(const std::vector<IT2>& ids)
    {
        if constexpr (axis == 0) {
            std::vector<int> iids_row;
            for (const IT2& id : ids) {
                int j = pidx->loc_i(id);
                iids_row.push_back(j);
            }

            return DataFrameVisitor<RangeVec<int>, Range<int>>(*this, RangeVec<int>(iids_row), Range<int>(0, size<1>()));

        } else {
            std::vector<int> iids_col;
            for (const IT2& col_name : ids) {
                int j = get_column_index(col_name);
                iids_col.push_back(j);
            }

            return DataFrameVisitor<Range<int>, RangeVec<int>>(Range<int>(0, size<0>()), RangeVec<int>(iids_col));
        }
    }
    template <int axis, class IT2, class INT2>
    auto loc(const Array<IT2, INT2>& ids)
    {
        return loc<axis>(ids.values);
    }
    template <int axis, class IT2, class INT2>
    auto loc(const Index<IT2, INT2>& ids)
    {
        return loc<axis>(ids.values);
    }
    template <int axis, class IT2, class DT2, class INT2, class DNT2>
    auto loc(const Series<IT2, DT2, INT2, DNT2>& ids)
    {
        return loc<axis>(ids.values);
    }

    /// @loc by bool mask
    /// @tparam axis
    /// @param mask
    /// @return
    template <int axis, class DT2>
    auto loc_mask(const std::vector<DT2>& mask)
    {
        if constexpr (axis == 0) {
            if (mask.size() != size<0>()) {
                PANDAS_THROW(std::format("size not match: {}!={}", mask.size(), size<0>()));
            }
            std::vector<int> iids_row;

            for (int i = 0; i < mask.size(); i++) {
                if (mask[i]) {
                    iids_row.push_back(i);
                }
            }
            return DataFrameVisitor<RangeVec<int>, Range<int>>(*this, RangeVec<int>(iids_row), Range(0, size<1>()));

        } else {
            if (mask.size() != size<1>()) {
                PANDAS_THROW(std::format("size not match: {}!={}", mask.size(), size<1>()));
            }
            std::vector<int> iids_col;

            for (int j = 0; j < mask.size(); j++) {
                iids_col.push_back(j);
            }
            return DataFrameVisitor<Range<int>, RangeVec<int>>(Range<int>(0, size<0>()), RangeVec<int>(iids_col));
        }
    }

    template <int axis, class DT2, class INT2>
    auto loc_mask(const Array<DT2, INT2>& mask)
    {
        return loc_mask<axis>(mask.values);
    }
    template <int axis, class DT2, class INT2>
    auto loc_mask(const Index<DT2, INT2>& mask)
    {
        return loc_mask<axis>(mask.values);
    }
    template <int axis, class IT2, class DT2, class INT2, class DNT2>
    auto loc_mask(const Series<IT2, DT2, INT2, DNT2>& mask)
    {
        if constexpr (axis == 0) {
            std::vector<int> iids_row;
            for (int i = 0; i < size<0>(); i++) {
                IT id = pidx->iloc(i);
                if (mask.loc(id)) {
                    iids_row.push_back(i);
                }
            }
            return DataFrameVisitor<RangeVec<int>, Range<int>>(*this, RangeVec<int>(iids_row), Range<int>(0, size<1>()));

        } else {
            std::vector<int> iids_col;
            for (int j = 0; j < size<1>(); j++) {
                DNT col_name = iloc<1>(j).get_name();
                if (mask.loc(col_name)) {
                    iids_col.push_back(j);
                }
            }
            return DataFrameVisitor<Range<int>, RangeVec<int>>(*this, Range<int>(0, size<0>()), RangeVec<int>(iids_col));
        }
    }

    /// @brief set index
    /// @tparam IT2
    /// @tparam INT2
    /// @param name
    /// @param vs
    /// @return
    template <class IT2, class INT2>
    DataFrame<IT2, DT, INT2, DNT> set_index(const INT2& name, const std::vector<IT2>& vs)
    {
        if (vs.size() != size<0>()) {
            PANDAS_THROW(std::format("size not match: {}!={}", vs.size(), size<0>()));
        }
        Index<IT2, INT2> idx(vs, name);
        DataFrame<IT2, DT, INT2, DNT> df(idx);
        for (int j = 0; j < size<1>(); j++) {
            df._append_col(iloc<1>(j).values);
        }
        return df;
    }
    template <class IT2, class INT2>
    DataFrame<IT2, DT, INT2, DNT> set_index(const Array<IT2, INT2>& ar)
    {
        return set_index(ar.get_name(), ar.values);
    }
    template <class IT2, class INT2>
    DataFrame<IT2, DT, INT2, DNT> set_index(const Index<IT2, INT2>& idx)
    {
        return set_index(idx.get_name(), idx.values);
    }
    template <class IT2, class DT2, class INT2, class DNT2>
    DataFrame<IT2, DT, INT2, DNT> set_index(const Series<IT2, DT2, INT2, DNT2>& sr)
    {
        if (sr.size() != size<0>()) {
            PANDAS_THROW(std::format("size not match: {}!={}", sr.size(), size<0>()));
        }
        auto sr2 = sr.reindex(*pidx);
        return set_index(sr.get_name(), sr.values);
    }

    std::string to_string(int mx_row = 10, int mx_col = 10, bool tail = true) const
    {
        std::vector<std::string> idx_lines = pandas::split(pidx->to_string(mx_row, false), "\n");
        std::vector<std::vector<std::string>> col_lines;

        if (values.size() > mx_col) {
            for (int j = 0; j < mx_col / 2; j++) {
                col_lines.push_back(pandas::split(values[j].values.to_string(mx_row, false), "\n"));
            }

            std::vector<std::string> empty_lines;
            for (int i = 0; i < col_lines[0].size(); i++) {
                empty_lines.push_back("...");
            }
            col_lines.push_back(empty_lines);

            for (int j = values.size() - mx_col / 2; j < values.size(); j++) {
                col_lines.push_back(pandas::split(values[j].values.to_string(mx_row, false), "\n"));
            }

        } else {
            for (int j = 0; j < values.size(); j++) {
                col_lines.push_back(pandas::split(values[j].values.to_string(mx_row, false), "\n"));
            }
        }

        std::stringstream ss;
        for (int i = 0; i < idx_lines.size(); i++) {
            ss << idx_lines[i] << " ";
            for (int j = 0; j < col_lines.size(); j++) {
                ss << col_lines[j][i] << " ";
            }
            if (i + 1 < idx_lines.size()) {
                ss << "\n";
            }
        }

        std::string s = ss.str();
        if (tail) {
            s += std::format("\n[{} rows x {} columns]", size<0>(), size<1>());
        }

        return s;
    }

    template <class DT2>
    DataFrame sort_values(const std::vector<DT2>& vs, bool ascending = true) const
    {
        std::vector<Series<IT, DT, INT, DNT>> srs;
        for (const Series<IT, DT, INT, DNT>& sr : values) {
            srs.push_back(sr.sort_values(vs, ascending));
        }
        return concat<1>(srs);
    }
    template <class DT2, class DNT2>
    DataFrame sort_values(const Array<DT2, DNT2>& ar, bool ascending = true) const
    {
        return sort_values(ar.values, ascending);
    }
    template <class IT2, class DT2, class INT2, class DNT2>
    DataFrame sort_values(const Series<IT2, DT2, INT2, DNT2>& sr_key, bool ascending = true) const
    {
        std::vector<Series<IT, DT, INT, DNT>> srs;
        for (const Series<IT, DT, INT, DNT>& sr : values) {
            srs.push_back(sr.sort_values(sr_key, ascending));
        }
        return concat<1>(srs);
    }
    DataFrame sort_values(const DNT& col, bool ascending = true) const
    {
        return sort_values<IT, DT, INT, DNT>(loc<1>(col), ascending);
    }

    friend std::ostream& operator<<(std::ostream& os, const DataFrame& df)
    {
        os << df.to_string();
        return os;
    }

#include "pandas/dataframe_functional.tcc"
#include "pandas/dataframe_group.tcc"
#include "pandas/dataframe_op.tcc"
#include "pandas/dataframe_rolling.tcc"
};

}