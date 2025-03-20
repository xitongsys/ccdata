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
};
