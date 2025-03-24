#pragma once

#include <ostream>
#include <string>
#include <vector>

#include "pandas/array.h"
#include "pandas/index.h"
#include "pandas/pandastype.h"
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

    DataFrame(const Index<IT, INT>& idx)
    {
        pidx = std::make_shared<Index<IT, INT>>(idx);
    }

    DataFrame(const std::vector<IT>& ids, const std::vector<DNT>& columns)
    {
        pidx = std::make_shared<Index<IT, INT>>(ids);
        for (int j = 0; j < columns.size(); j++) {
            _append_col(Series<IT, DT, INT, DNT>(pidx, columns[j]));
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
            pidx->_append(id);

            for (int j = 0; j < columns.size(); j++) {
                values[j]._append(id, row.iloc(j));
            }
        }

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

    DataFrame(const std::vector<Series<IT, DT, INT, DNT>>& srs)
    {
        pidx = std::make_shared<Index<IT, INT>>();
        for (auto& sr : srs) {
            int n = sr.pidx->size();
            for (int i = 0; i < n; i++) {
                IT id = sr.pidx->iloc(i);
                if (!pidx->has(id)) {
                    pidx->_append(id);
                }
            }
        }

        for (int i = 0; i < srs.size(); i++) {
            values.push_back(srs[i].reindex(*pidx));
            values[i].pidx = pidx;
        }
    }

    DataFrame<IT, DT, INT, DNT> reindex(const Index<IT, INT>& idx)
    {
        DataFrame<IT, DT, INT, DNT> df(idx);
        for (int i = 0; i < values.size(); i++) {
            Series<IT, DT, INT, DNT> sr = values[i].reindex(*df.pidx);
            sr.pidx = df.pidx;
            df.values._append(sr);
        }
        return df;
    }

    void reassign_index()
    {
        for (auto& sr : values) {
            sr.pidx = pidx;
        }
    }

    void _reindex(const Index<IT, INT>& idx)
    {
        pidx = std::make_shared<Index<IT, INT>>(idx);
        for (int i = 0; i < values.size(); i++) {
            values[i] = values[i].reindex(idx);
            values[i].pidx = pidx;
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

    template <int axis = 0>
    size_t size() const
    {
        if constexpr (axis == 0) {
            return pidx->size();
        } else {
            return values.size();
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

    int _append_row(const Array<DT, IT>& ar)
    {
        if (ar.size() != values.size()) {
            throw std::format("row size not match: {}!={}", ar.size(), values.size());
        }

        IT id = ar.get_name();
        pidx->_append(id);
        for (int i = 0; i < values.size(); i++) {
            values[i].values._append(ar.iloc(i));
        }

        return size<0>();
    }

    template <class INT2>
    int _append_row(const Series<DNT, DT, INT2, IT>& sr)
    {
        if (sr.size() != values.size()) {
            throw std::format("row size not match: {}!={}", sr.size(), values.size());
        }

        IT id = sr.get_name();
        pidx->_append(id);

        for (int i = 0; i < values.size(); i++) {
            bool ok = false;
            DNT col = values[i].get_name();
            DT value = sr.loc(col);
            values[i]._append(value);
        }
        return size();
    }

    int _append_col(const Array<DT, DNT>& ar)
    {
        if (ar.size() != size()) {
            throw std::format("append size not match: {}!={}", ar.size(), size());
        }
        if (get_column_index(ar.get_name()) >= 0) {
            throw std::format("duplicated column name: {}", ar.get_name());
        }

        auto sr = Series(pidx, ar, ar.name);
        values.push_back(sr);
        return size(1);
    }

    int _append_col(const Series<IT, DT, INT, DNT>& sr)
    {
        if (get_column_index(sr.get_name()) >= 0) {
            throw std::format("duplicated column name: {}", sr.get_name());
        }

        for (int i = 0; i < sr.size(); i++) {
            IT id = sr.pidx->iloc(i);
            if (!pidx->has(id)) {
                pidx->_append(id);
            }
        }

        for (int i = 0; i < values.size(); i++) {
            values[i] = values[i].reindex(*pidx);
            values[i].pidx = pidx;
        }

        auto sr2 = sr.reindex(*pidx);
        sr2.pidx = pidx;
        values.push_back(sr2);
        return values.size();
    }

    inline DT iloc(int ri, int ci) const
    {
        return values[ci].iloc(ri);
    }

    inline DT& iloc_ref(int ri, int ci)
    {
        return values[ci].iloc_ref(ri);
    }

    template <int axis = 0>
    auto iloc(int i) const
    {
        if constexpr (axis == 0) {
            Series<DNT, DT, std::string, IT> row;
            IT id = pidx->iloc(i);
            for (int i = 0; i < size(1); i++) {
                row._append(values[i].get_name(), values[i].iloc(i));
            }
            row._rename(id);
            return row;

        } else {
            Series<IT, DT, INT, DNT> col = values[i];
            return col;
        }
    }

    template <int axis = 0>
    auto& iloc_ref(int i)
    {
        if constexpr (axis == 0) {
            throw std::format("DataFrame not support iloc_ref for rows");

        } else {
            Series<IT, DT, INT, DNT>& col = values[i];
            return col;
        }
    }

    template <int axis = 0, class T2>
    auto loc(T2 key)
    {
        if constexpr (axis == 0) {
            int i = pidx->loc(key);
            return iloc<0>(i);
        } else {
            int i = get_column_index(key);
            return iloc<1>(i);
        }
    }

    template <int axis = 0>
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

    template <int axis = 0, class KEY>
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
    template <int axis = 0, class IT2>
    auto loc(const std::vector<IT2>& ids)
    {
        if constexpr (axis == 0) {
            std::vector<int> iids_row;
            for (const IT2& id : ids) {
                int j = pidx->loc_i(id);
                iids_row.push_back(j);
            }

            return DataFrameVisitor<RangeVec<int>, Range<int>>(*this, RangeVec<int>(iids_row), Range(0, size<1>()));

        } else {
            std::vector<int> iids_col;
            for (const IT2& col_name : ids) {
                int j = get_column_index(col_name);
                iids_col.push_back(j);
            }

            return DataFrameVisitor<Range<int>, RangeVec<int>>(Range<int>(0, size<0>()), RangeVec<int>(iids_col));
        }
    }
    template <int axis = 0, class IT2, class INT2>
    auto loc(const Array<IT2, INT2>& ids)
    {
        return loc<axis>(ids.values);
    }
    template <int axis = 0, class IT2, class INT2>
    auto loc(const Index<IT2, INT2>& ids)
    {
        return loc<axis>(ids.values);
    }
    template <int axis = 0, class IT2, class DT2, class INT2, class DNT2>
    auto loc(const Series<IT2, DT2, INT2, DNT2>& ids)
    {
        return loc<axis>(ids.values);
    }

    /// @loc by bool mask
    /// @tparam axis
    /// @param mask
    /// @return
    template <int axis = 0>
    auto loc(const std::vector<bool>& mask)
    {
        if constexpr (axis == 0) {
            if (mask.size() != size<0>()) {
                throw std::format("size not match: {}!={}", mask.size(), size<0>());
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
                throw std::format("size not match: {}!={}", mask.size(), size<1>());
            }
            std::vector<int> iids_col;

            for (int j = 0; j < mask.size(); j++) {
                iids_col.push_back(j);
            }
            return DataFrameVisitor<Range<int>, RangeVec<int>>(Range<int>(0, size<0>()), RangeVec<int>(iids_col));
        }
    }

    template <int axis = 0, class INT2>
    auto loc(const Array<bool, INT2>& mask)
    {
        return loc<axis>(mask.values);
    }
    template <int axis = 0, class INT2>
    auto loc(const Index<bool, INT2>& mask)
    {
        return loc<axis>(mask.values);
    }
    template <int axis = 0, class IT2, class INT2, class DNT2>
    auto loc(const Series<IT2, bool, INT2, DNT2>& mask)
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

    std::string to_string(int mx_row = 10, int mx_col = 10) const
    {
        std::vector<std::string> idx_lines = pandas::split(pidx->to_string(mx_row), "\n");
        std::vector<std::vector<std::string>> col_lines;

        if (values.size() > mx_col) {
            for (int j = 0; j < mx_col / 2; j++) {
                col_lines.push_back(pandas::split(values[j].values.to_string(), "\n"));
            }

            std::vector<std::string> empty_lines;
            for (int i = 0; i < col_lines[0].size(); i++) {
                empty_lines.push_back("...");
            }
            col_lines.push_back(empty_lines);

            for (int j = values.size() - mx_col / 2; j < values.size(); j++) {
                col_lines.push_back(pandas::split(values[j].values.to_string(), "\n"));
            }

        } else {
            for (int j = 0; j < values.size(); j++) {
                col_lines.push_back(pandas::split(values[j].values.to_string(), "\n"));
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

        return ss.str();
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

template <
    class IT, class DT, class INT, class DNT,
    class IT2, class DT2, class INT2, class DNT2>
DataFrame<IT, DT, INT, DNT> concat_1(
    Series<IT, DT, INT, DNT>& sr1,
    Series<IT2, DT2, INT2, DNT2>& sr2)
{
    if (sr1.get_name() == sr2.get_name()) {
        throw std::format("columns have same name: {}", sr1.get_name());
    }

    Index<IT, INT> idx;
    for (int i = 0; i < sr1.size(); i++) {
        idx._append(sr1.pidx->iloc(i));
    }
    for (int i = 0; i < sr2.size(); i++) {
        idx._append(sr2.pidx->iloc(i));
    }

    Series<IT, DT, INT, DNT> sr1_new = sr1.reindex(idx);
    Series<IT, DT2, INT, DNT2> sr2_new = sr2.reindex(idx);

    DataFrame<IT, DT, INT, DNT> df;
    df.pidx = std::make_shared<Index<IT, INT>>(idx);

    sr1_new.pidx = df.pidx;
    sr2_new.pidx = df.pidx;

    df.values.emplace_back(std::move(sr1_new));
    df.values.emplace_back(std::move(sr2_new));

    return df;
}

template <
    class IT, class DT, class INT, class DNT,
    class IT2, class DT2, class INT2, class DNT2>
DataFrame<IT, DT, INT, DNT> concat_1(
    const DataFrame<IT, DT, INT, DNT>& df,
    const Series<IT2, DT2, INT2, DNT2>& sr)
{
    for (int i = 0; i < df.values.size(); i++) {
        if (df.values[i].name() == sr.name()) {
            throw std::format("columns have same name: {}", sr.name());
        }
    }

    Index<IT, INT> idx;
    for (int i = 0; i < df.size(); i++) {
        idx._append(df.pidx->iloc(i));
    }
    for (int i = 0; i < sr.size(); i++) {
        idx._append(sr.pidx->iloc(i));
    }

    DataFrame<IT, DT, INT, DNT> df_new = df.reindex(idx);
    Series<IT, DT2, INT, DNT2> sr_new = sr.reindex(idx);
    df_new.values._append(sr_new);

    return df_new;
}

}