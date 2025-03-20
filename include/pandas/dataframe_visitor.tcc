#pragma once

// template<class IT, class DT, class INT, class DNT>
// class DataFrame {

template <class IITROW, class IITCOL>
class DataFrameVisitor {
public:
    DataFrame<IT, DT, INT, DNT>& df;
    IITROW it_row;
    IITCOL it_col;
    std::vector<int> iids_row, iids_col;

    DataFrameVisitor(DataFrame<IT, DT, INT, DNT>& df_, const IITROW& it_row_, const IITCOL& it_col_)
        : df(df_)
        , it_row(it_row_)
        , it_col(it_col_)
    {
    }

    DataFrame to_frame()
    {
        SingleIndex<IT, INT> idx;
        it_row.reset();
        while (it_row.has_left()) {
            int i = it_row.next();
            idx._append(df.pidx->iloc(i));
        }

        std::vector<Series<IT, DT, INT, DNT>> srs;
        it_col.reset();
        while (it_col.has_left()) {
            int j = it_col.next();
            srs.push_back(df.iloc<1>(j).loc(idx).to_series());
        }

        return DataFrame(srs);
    }
};
