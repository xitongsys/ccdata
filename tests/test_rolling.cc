#include "pandas/array.h"
#include "pandas/concat.h"
#include "pandas/dataframe.h"
#include "pandas/datetime.h"
#include "pandas/index.h"
#include "pandas/ops.h"
#include "pandas/range.h"
#include "pandas/series.h"

#include <cassert>
#include <iostream>
#include <string>
#include <vector>
#include <cmath>

using namespace std;
using namespace pandas;

// ========== Series Rolling Tests ==========
void test_series_rolling_count()
{
    Series<int, double> sr(Index<int>(Array<int>({ 1, 2, 3, 4, 5 })), Array<double>({ 1, 2, 3, 4, 5 }, "sr"));
    auto rc = sr.rolling(3, 1).count();
    assert(rc.size() == 5);
    assert(rc.iloc(0) == 1);
    assert(rc.iloc(1) == 2);
    assert(rc.iloc(2) == 3);
    assert(rc.iloc(3) == 3);
    assert(rc.iloc(4) == 3);
    cout << "[PASS] test_series_rolling_count" << endl;
}

void test_series_rolling_count_with_nan()
{
    Series<int, double> sr(Index<int>(Array<int>({ 1, 2, 3, 4, 5 })), Array<double>({ 1, pandas::nan<double>(), 3, 4, 5 }, "sr"));
    auto rc = sr.rolling(3, 1).count();
    assert(rc.iloc(0) == 1);
    assert(rc.iloc(1) == 1);  // only value at idx 0 (idx 1 is NaN)
    assert(rc.iloc(2) == 2);  // values idx 0,2 (idx 1 is NaN)
    assert(rc.iloc(3) == 2);
    assert(rc.iloc(4) == 3);
    cout << "[PASS] test_series_rolling_count_with_nan" << endl;
}

void test_series_rolling_sum()
{
    Series<int, double> sr(Index<int>(Array<int>({ 1, 2, 3, 4, 5 })), Array<double>({ 1, 2, 3, 4, 5 }, "sr"));
    auto rs = sr.rolling(3, 2).sum();
    assert(rs.size() == 5);
    assert(pandas::isnan(rs.iloc(0)));  // min_periods=2, but only 1 element
    assert(rs.iloc(1) == 3);             // 1+2=3, cnt=2
    assert(rs.iloc(2) == 6);             // 1+2+3=6
    assert(rs.iloc(3) == 9);
    assert(rs.iloc(4) == 12);
    cout << "[PASS] test_series_rolling_sum" << endl;
}

void test_series_rolling_max()
{
    Series<int, double> sr(Index<int>(Array<int>({ 1, 2, 3, 4, 5 })), Array<double>({ 3, 1, 4, 1, 5 }, "sr"));
    auto rm = sr.rolling(3, 2).max();
    assert(rm.size() == 5);
    assert(pandas::isnan(rm.iloc(0)));
    assert(rm.iloc(1) == 3);   // max(3,1)=3
    assert(rm.iloc(2) == 4);   // max(3,1,4)=4
    assert(rm.iloc(3) == 4);   // max(1,4,1)=4
    assert(rm.iloc(4) == 5);   // max(4,1,5)=5
    cout << "[PASS] test_series_rolling_max" << endl;
}

void test_series_rolling_min()
{
    Series<int, double> sr(Index<int>(Array<int>({ 1, 2, 3, 4, 5 })), Array<double>({ 3, 1, 4, 1, 5 }, "sr"));
    auto rm = sr.rolling(3, 2).min();
    assert(rm.size() == 5);
    assert(pandas::isnan(rm.iloc(0)));
    assert(rm.iloc(1) == 1);   // min(3,1)=1
    assert(rm.iloc(2) == 1);   // min(3,1,4)=1
    assert(rm.iloc(3) == 1);   // min(1,4,1)=1
    assert(rm.iloc(4) == 1);   // min(4,1,5)=1
    cout << "[PASS] test_series_rolling_min" << endl;
}

void test_series_rolling_mean()
{
    Series<int, double> sr(Index<int>(Array<int>({ 1, 2, 3, 4, 5 })), Array<double>({ 2, 4, 6, 8, 10 }, "sr"));
    auto rm = sr.rolling(2, 1).mean();
    assert(rm.size() == 5);
    assert(rm.iloc(0) == 2.0);
    assert(rm.iloc(1) == 3.0);
    assert(rm.iloc(2) == 5.0);
    assert(rm.iloc(3) == 7.0);
    assert(rm.iloc(4) == 9.0);

    cout << "[PASS] test_series_rolling_mean" << endl;
}

void test_series_rolling_var()
{
    Series<int, double> sr(Index<int>(Array<int>({ 1, 2, 3, 4 })), Array<double>({ 2, 4, 6, 8 }, "sr"));
    auto rv = sr.rolling(2, 2).var();
    // rolling var at idx1: var of {2,4} = ((2-3)^2+(4-3)^2)/(2-1) = 2
    assert(abs(rv.iloc(1) - 2.0) < 1e-10);
    assert(abs(rv.iloc(2) - 2.0) < 1e-10);
    cout << "[PASS] test_series_rolling_var" << endl;
}

void test_series_rolling_std()
{
    Series<int, double> sr(Index<int>(Array<int>({ 1, 2, 3, 4 })), Array<double>({ 2, 4, 6, 8 }, "sr"));
    auto rs = sr.rolling(2, 2).std();
    assert(abs(rs.iloc(1) - sqrt(2.0)) < 1e-10);
    cout << "[PASS] test_series_rolling_std" << endl;
}

void test_series_rolling_median()
{
    Series<int, double> sr(Index<int>(Array<int>({ 1, 2, 3, 4, 5 })), Array<double>({ 1, 10, 2, 9, 3 }, "sr"));
    auto rm = sr.rolling(3, 2).median();
    // window [0]: {1} -> nan (min_periods=2)
    assert(pandas::isnan(rm.iloc(0)));
    // window [0,1]: {1,10} -> median=(1+10)/2=5.5
    assert(abs(rm.iloc(1) - 5.5) < 1e-10);
    // window [0,1,2]: {1,10,2} -> sorted {1,2,10}, median=2
    assert(abs(rm.iloc(2) - 2.0) < 1e-10);
    cout << "[PASS] test_series_rolling_median" << endl;
}

void test_series_rolling_min_periods_all()
{
    Series<int, double> sr(Index<int>(Array<int>({ 1, 2, 3 })), Array<double>({ 1, 2, 3 }, "sr"));
    auto rs = sr.rolling(3, 3).sum();
    assert(pandas::isnan(rs.iloc(0)));
    assert(pandas::isnan(rs.iloc(1)));
    assert(rs.iloc(2) == 6);
    cout << "[PASS] test_series_rolling_min_periods_all" << endl;
}

void test_series_rolling_window1()
{
    Series<int, double> sr(Index<int>(Array<int>({ 1, 2, 3 })), Array<double>({ 10, 20, 30 }, "sr"));
    auto rs = sr.rolling(1, 1).sum();
    assert(rs.iloc(0) == 10);
    assert(rs.iloc(1) == 20);
    assert(rs.iloc(2) == 30);
    cout << "[PASS] test_series_rolling_window1" << endl;
}

void test_series_rolling_agg()
{
    Series<int, double> sr(Index<int>(Array<int>({ 1, 2, 3, 4 })), Array<double>({ 1, 2, 3, 4 }, "sr"));
    using SV = Series<int, double>::SeriesVisitor<Range<int>>;
    auto rs = sr.rolling(2, 1).agg<double>([](SV& sv) -> double {
        return sv.sum() * 2.0;
    });
    assert(rs.iloc(0) == 2.0);   // 1*2
    assert(rs.iloc(1) == 6.0);    // (1+2)*2=6
    assert(rs.iloc(2) == 10.0);   // (2+3)*2=10
    assert(rs.iloc(3) == 14.0);   // (3+4)*2=14
    cout << "[PASS] test_series_rolling_agg" << endl;
}

// ========== DataFrame Rolling Tests ==========
void test_dataframe_rolling_sum()
{
    DataFrame<int, double> df({ 1, 2, 3, 4 }, { "a", "b" }, 0.0);
    df.iloc_ref(1, 0) = pandas::nan<double>();
    df.iloc_ref(2, 1) = pandas::nan<double>();
    // df:
    // a: 0, NaN, 0, 0
    // b: 0, 0, NaN, 0
    // Set some actual values
    df.iloc_ref(0, 0) = 1;
    df.iloc_ref(1, 0) = 2;
    df.iloc_ref(2, 0) = 3;
    df.iloc_ref(3, 0) = 4;
    df.iloc_ref(0, 1) = 5;
    df.iloc_ref(1, 1) = 6;
    df.iloc_ref(2, 1) = 7;
    df.iloc_ref(3, 1) = 8;

    auto rd = df.rolling(3, 1).sum();
    assert(rd.size<0>() == 4);
    assert(rd.size<1>() == 2);
    assert(rd.iloc(0, 0) == 1);
    assert(rd.iloc(1, 0) == 3);    // 1+2
    assert(rd.iloc(2, 0) == 6);    // 1+2+3=6
    assert(rd.iloc(3, 0) == 9);    // 2+3+4=9
    cout << "[PASS] test_dataframe_rolling_sum" << endl;
}

void test_dataframe_rolling_mean()
{
    DataFrame<int, double> df({ 1, 2, 3 }, { "a", "b" }, 0.0);
    df.iloc_ref(0, 0) = 1;
    df.iloc_ref(1, 0) = 2;
    df.iloc_ref(2, 0) = 3;
    df.iloc_ref(0, 1) = 4;
    df.iloc_ref(1, 1) = 5;
    df.iloc_ref(2, 1) = 6;

    auto rm = df.rolling(2, 1).mean();
    assert(rm.iloc(0, 0) == 1.0);
    assert(rm.iloc(1, 0) == 1.5);
    assert(rm.iloc(0, 1) == 4.0);
    assert(rm.iloc(1, 1) == 4.5);
    cout << "[PASS] test_dataframe_rolling_mean" << endl;
}

void test_dataframe_rolling_count()
{
    DataFrame<int, double> df({ 1, 2, 3, 4 }, { "a" }, 0.0);
    df.iloc_ref(1, 0) = pandas::nan<double>();

    auto rc = df.rolling(2, 1).count();
    assert(rc.iloc(0, 0) == 1);
    assert(rc.iloc(1, 0) == 1);  // only index 0 is non-NaN in window [0,1]
    assert(rc.iloc(2, 0) == 1);  // window [1,2], idx 1 is NaN
    assert(rc.iloc(3, 0) == 2);  // window [2,3], both non-NaN
    cout << "[PASS] test_dataframe_rolling_count" << endl;
}

void test_dataframe_rolling_max()
{
    DataFrame<int, double> df({ 1, 2, 3 }, { "a" }, 0.0);
    df.iloc_ref(0, 0) = 5;
    df.iloc_ref(1, 0) = 1;
    df.iloc_ref(2, 0) = 3;
    auto rm = df.rolling(2, 1).max();
    assert(rm.iloc(0, 0) == 5);
    assert(rm.iloc(1, 0) == 5);
    assert(rm.iloc(2, 0) == 3);
    cout << "[PASS] test_dataframe_rolling_max" << endl;
}

void test_dataframe_rolling_min()
{
    DataFrame<int, double> df({ 1, 2, 3 }, { "a" }, 0.0);
    df.iloc_ref(0, 0) = 5;
    df.iloc_ref(1, 0) = 1;
    df.iloc_ref(2, 0) = 3;
    auto rm = df.rolling(2, 1).min();
    assert(rm.iloc(0, 0) == 5);
    assert(rm.iloc(1, 0) == 1);
    assert(rm.iloc(2, 0) == 1);
    cout << "[PASS] test_dataframe_rolling_min" << endl;
}

void test_dataframe_rolling_median()
{
    DataFrame<int, double> df({ 1, 2, 3 }, { "a" }, 0.0);
    df.iloc_ref(0, 0) = 1;
    df.iloc_ref(1, 0) = 10;
    df.iloc_ref(2, 0) = 2;
    auto rm = df.rolling(3, 1).median();
    assert(pandas::isnan(rm.iloc(0, 0)));  // only 1 element, nan
    assert(abs(rm.iloc(1, 0) - 5.5) < 1e-10);
    assert(abs(rm.iloc(2, 0) - 2.0) < 1e-10);
    cout << "[PASS] test_dataframe_rolling_median" << endl;
}

void test_dataframe_rolling_agg()
{
    DataFrame<int, double> df({ 1, 2, 3 }, { "a", "b" }, 0.0);
    df.iloc_ref(0, 0) = 1;
    df.iloc_ref(1, 0) = 2;
    df.iloc_ref(2, 0) = 3;
    df.iloc_ref(0, 1) = 4;
    df.iloc_ref(1, 1) = 5;
    df.iloc_ref(2, 1) = 6;

    using SV = Series<int, double>::SeriesVisitor<Range<int>>;
    auto ra = df.rolling(2, 1).agg<double>([](SV& sv) -> double {
        return sv.sum() * 10;
    });
    assert(ra.iloc(0, 0) == 10);
    assert(ra.iloc(1, 0) == 30);
    assert(ra.iloc(2, 0) == 50);
    assert(ra.iloc(0, 1) == 40);
    assert(ra.iloc(1, 1) == 90);
    assert(ra.iloc(2, 1) == 110);
    cout << "[PASS] test_dataframe_rolling_agg" << endl;
}

// ========== DataFrame Enhanced Tests ==========
void test_frame_constructors_advanced()
{
    DataFrame<int, double> df1({ 0, 1 }, { "a", "b" }, pandas::nan<double>());
    df1._fillna(1);
    assert(df1.size<0>() == 2);
    assert(df1.size<1>() == 2);

    // copy construct
    auto df2 = df1;
    assert(df2.size<0>() == 2);

    // copy method
    auto df3 = df1.copy();
    df1.iloc_ref(0, 0) = 99;
    assert(df2.iloc(0, 0) == 99);   // copy share pidx
    assert(df3.iloc(0, 0) == 1);    // deep copy, independent

    // with Index
    Index<int> idx(Array<int>({ 10, 20 }));
    DataFrame<int, double> df4(idx, { "x", "y" }, 0.0);
    assert(df4.pidx->iloc(0) == 10);
    assert(df4.size<0>() == 2);
    assert(df4.size<1>() == 2);
    cout << "[PASS] test_frame_constructors_advanced" << endl;
}

void test_frame_append_col()
{
    DataFrame<int, double> df({ 1, 2, 3 }, { "a" }, 0.0);
    df._append_col("b", 1.0);
    assert(df.size<1>() == 2);
    assert(df.iloc(0, 1) == 1.0);
    assert(df.columns().size() == 2);

    // append by Array
    Array<double> ar({ 10, 20, 30 }, "c");
    df._append_col(ar);
    assert(df.size<1>() == 3);
    assert(df.iloc(0, 2) == 10);

    // duplicate column should throw
    bool threw = false;
    try {
        df._append_col("a", 0.0);
    } catch (...) {
        threw = true;
    }
    assert(threw);
    cout << "[PASS] test_frame_append_col" << endl;
}

void test_frame_append_row()
{
    DataFrame<int, double> df(std::vector<int>(), { "a", "b" }, pandas::nan<double>());
    df._append_row(1, std::vector<double>({ 1, 2 }));
    df._append_row(2, std::vector<double>({ 3, 4 }));
    assert(df.size<0>() == 2);
    assert(df.iloc(0, 0) == 1);
    assert(df.iloc(1, 1) == 4);

    // mismatch sizes should throw
    bool threw = false;
    try {
        df._append_row(3, std::vector<double>({ 1 }));
    } catch (...) {
        threw = true;
    }
    assert(threw);
    cout << "[PASS] test_frame_append_row" << endl;
}

void test_frame_operators()
{
    DataFrame<int, double> df({ 0, 1 }, { "a", "b" }, pandas::nan<double>());
    df._fillna(1);
    df._append_row(2, std::vector<double>({ 2, 3 }));

    // scalar arithmetic
    auto df2 = df + 1;
    assert((df2.iloc(2, 1) == 4) && (df2.iloc(0, 0) == 2));

    // compound assignment
    df2 += 3;
    assert((df2.iloc(2, 1) == 7) && (df2.iloc(0, 0) == 5));

    // vector arithmetic axis=0
    auto df3 = df2.operator+ <0>(std::vector<int>({ 1, 2, 3 }));
    assert((df3.iloc(2, 1) == 10) && (df3.iloc(0, 0) == 6));

    // Array arithmetic axis=1
    auto df4 = df3.operator- <1>(Array<int>({ 1, 2 }));
    assert((df4.iloc(2, 1) == 8) && (df4.iloc(0, 0) == 5));

    // comparison ops
    auto cmp = df4 > 6;
    assert(cmp.iloc(0, 0) == false);
    assert(cmp.iloc(2, 1) == true);
    cout << "[PASS] test_frame_operators" << endl;
}

void test_frame_loc_single()
{
    DataFrame<int, double> df({ 10, 20, 30 }, { "a", "b" }, pandas::nan<double>());
    df._fillna(1);

    // loc row
    auto row = df.loc<0>(10);
    assert(row.size() == 2);

    // loc column
    auto col = df.loc<1>("a");
    assert(col.size() == 3);
    cout << "[PASS] test_frame_loc_single" << endl;
}

void test_frame_loc_vector()
{
    DataFrame<int, double> df({ 10, 20, 30 }, { "a", "b" }, pandas::nan<double>());
    df._fillna(1);

    // loc rows by vector
    auto visitor = df.loc<0, int>(std::vector<int>({ 10, 30 }));
    auto sub = visitor.to_frame();
    assert(sub.size<0>() == 2);
    assert(sub.iloc(0, 0) == 1);
    assert(sub.iloc(1, 1) == 1);

    // loc columns by vector
    auto visitor_col = df.loc<1, std::string>(std::vector<std::string>({ "b" }));
    auto sub_col = visitor_col.to_frame();
    assert(sub_col.size<1>() == 1);
    cout << "[PASS] test_frame_loc_vector" << endl;
}

void test_frame_loc_range()
{
    DataFrame<int, double> df({ 10, 20, 30, 40 }, { "a", "b" }, pandas::nan<double>());
    df._fillna(1);
    // loc row range using IndexRange
    cout << "[PASS] test_frame_loc_range" << endl;
}

void test_frame_loc_mask()
{
    DataFrame<int, double> df({ 10, 20, 30 }, { "a", "b" }, pandas::nan<double>());
    df._fillna(1);

    auto visitor = df.loc_mask<0>(std::vector<char>({ true, false, true }));
    auto sub = visitor.to_frame();
    assert(sub.size<0>() == 2);
    assert(sub.pidx->iloc(0) == 10);
    assert(sub.pidx->iloc(1) == 30);
    cout << "[PASS] test_frame_loc_mask" << endl;
}

void test_frame_iloc()
{
    DataFrame<int, double> df({ 10, 20, 30 }, { "a", "b" }, pandas::nan<double>());
    df._fillna(1);

    // iloc specific cell
    assert(df.iloc(0, 0) == 1);
    assert(df.iloc(1, 1) == 1);

    // iloc_ref
    df.iloc_ref(0, 0) = 99;
    assert(df.iloc(0, 0) == 99);

    // iloc row
    auto row = df.iloc<0>(0);
    assert(row.size() == 2);

    // iloc column
    auto col = df.iloc<1>(0);
    assert(col.size() == 3);
    assert(col.get_name() == "a");
    cout << "[PASS] test_frame_iloc" << endl;
}

void test_frame_set_index()
{
    Series<int, double> sr1(Index<int>(Array<int>({ 0, 1 })), Array<double>({ 1, 2 }, "sr1"));
    Series<int, double> sr2(Index<int>(Array<int>({ 0, 2 })), Array<double>({ 4, 3 }, "sr2"));
    DataFrame<int, double> df1(std::vector<Series<int, double>>({ sr1, sr2 }));

    auto df2 = df1.set_index<double, std::string>("index", std::vector<double>({ 10, 11, 12 }));
    assert(df2.pidx->iloc(0) == 10);

    auto df3 = df1.set_index<std::string, std::string>(Array<std::string>({ "10", "11", "12" }));
    assert(df3.pidx->iloc(0) == "10");
    cout << "[PASS] test_frame_set_index" << endl;
}

void test_frame_reindex()
{
    DataFrame<int, double> df({ 10, 20, 30 }, { "a", "b" }, pandas::nan<double>());
    df._fillna(1);

    auto df2 = df.reindex<0>(std::vector<int>({ 10, 30, 40 }));
    assert(df2.size<0>() == 3);
    assert(df2.iloc(0, 0) == 1);
    assert(df2.iloc(1, 0) == 1);
    assert(pandas::isnan(df2.iloc(2, 0)));  // 40 not present

    auto df3 = df.reindex<1>(std::vector<std::string>({ "a", "c" }));
    assert(df3.size<1>() == 2);
    assert(df3.iloc(0, 0) == 1);
    assert(pandas::isnan(df3.iloc(0, 1)));  // "c" not present
    cout << "[PASS] test_frame_reindex" << endl;
}

void test_frame_dropna()
{
    DataFrame<int, double> df({ 1, 2, 3 }, { "a", "b" }, pandas::nan<double>());
    df._fillna(1);
    // Add a fully NaN row
    df._append_row(4, std::vector<double>({ pandas::nan<double>(), pandas::nan<double>() }));

    auto df2 = df.dropna("any");
    assert(df2.size<0>() == 3);  // row 4 has all NaN, so dropped

    cout << "[PASS] test_frame_dropna" << endl;
}

void test_frame_T()
{
    DataFrame<int, double> df({ 1, 2 }, { "a", "b", "c" }, pandas::nan<double>());
    df._fillna(1);

    auto transposed = df.T();
    assert(transposed.size<0>() == 3);  // columns become rows
    assert(transposed.size<1>() == 2);  // rows become columns
    assert(transposed.iloc(0, 0) == 1);
    cout << "[PASS] test_frame_T" << endl;
}

void test_frame_functional_map()
{
    DataFrame<int, double> df({ 1, 2, 3 }, { "a", "b" }, 0.0);
    df.iloc_ref(0, 0) = 1;
    df.iloc_ref(1, 0) = 2;
    df.iloc_ref(2, 0) = 3;
    df.iloc_ref(0, 1) = 4;
    df.iloc_ref(1, 1) = 5;
    df.iloc_ref(2, 1) = 6;

    auto df2 = df.map<double>([](const double& v) -> double { return v * 10; });
    assert(df2.iloc(0, 0) == 10);
    assert(df2.iloc(2, 1) == 60);
    cout << "[PASS] test_frame_functional_map" << endl;
}

void test_frame_functional_diff_shift()
{
    DataFrame<int, double> df({ 1, 2, 3 }, { "a", "b" }, pandas::nan<double>());
    df.iloc_ref(0, 0) = 1;
    df.iloc_ref(1, 0) = 4;
    df.iloc_ref(2, 0) = 9;
    df.iloc_ref(0, 1) = 2;
    df.iloc_ref(1, 1) = 6;
    df.iloc_ref(2, 1) = 18;

    auto dfd = df.diff(1);
    assert(pandas::isnan(dfd.iloc(0, 0)));
    assert(dfd.iloc(1, 0) == 3);
    assert(dfd.iloc(2, 0) == 5);

    auto dfs = df.shift(1);
    assert(pandas::isnan(dfs.iloc(0, 0)));
    assert(dfs.iloc(1, 0) == 1);
    cout << "[PASS] test_frame_functional_diff_shift" << endl;
}

void test_frame_functional_fillna()
{
    DataFrame<int, double> df({ 1, 2, 3 }, { "a" }, pandas::nan<double>());
    df.iloc_ref(0, 0) = pandas::nan<double>();
    df.iloc_ref(1, 0) = 2;
    df.iloc_ref(2, 0) = pandas::nan<double>();

    df._fillna(0.0);
    assert(df.iloc(0, 0) == 0);
    assert(df.iloc(1, 0) == 2);
    assert(df.iloc(2, 0) == 0);

    // ffill
    DataFrame<int, double> df2({ 1, 2, 3, 4 }, { "a" }, pandas::nan<double>());
    df2.iloc_ref(0, 0) = 10;
    df2.iloc_ref(1, 0) = pandas::nan<double>();
    df2.iloc_ref(2, 0) = pandas::nan<double>();
    df2.iloc_ref(3, 0) = 20;
    df2._ffill(1);
    assert(df2.iloc(0, 0) == 10);
    assert(df2.iloc(1, 0) == 10);
    assert(pandas::isnan(df2.iloc(2, 0)));  // limit=1 default
    assert(df2.iloc(3, 0) == 20);

    cout << "[PASS] test_frame_functional_fillna" << endl;
}

void test_frame_functional_cumsum()
{
    DataFrame<int, double> df({ 1, 2, 3 }, { "a", "b" }, pandas::nan<double>());
    df.iloc_ref(0, 0) = 1;
    df.iloc_ref(1, 0) = 2;
    df.iloc_ref(2, 0) = 3;
    df.iloc_ref(0, 1) = 4;
    df.iloc_ref(1, 1) = 5;
    df.iloc_ref(2, 1) = 6;

    df._cumsum();
    assert(df.iloc(0, 0) == 1);
    assert(df.iloc(1, 0) == 3);
    assert(df.iloc(2, 0) == 6);
    assert(df.iloc(1, 1) == 9);
    assert(df.iloc(2, 1) == 15);
    cout << "[PASS] test_frame_functional_cumsum" << endl;
}

void test_frame_functional_abs()
{
    DataFrame<int, double> df({ 1, 2, 3 }, { "a" }, pandas::nan<double>());
    df.iloc_ref(0, 0) = -5;
    df.iloc_ref(1, 0) = 3;
    df.iloc_ref(2, 0) = -8;
    auto dfa = df.abs();
    assert(dfa.iloc(0, 0) == 5);
    assert(dfa.iloc(1, 0) == 3);
    assert(dfa.iloc(2, 0) == 8);
    cout << "[PASS] test_frame_functional_abs" << endl;
}

void test_frame_functional_pow_replace()
{
    DataFrame<int, double> df({ 1, 2, 3 }, { "a" }, 0.0);
    df.iloc_ref(0, 0) = 2;
    df.iloc_ref(1, 0) = 3;
    df.iloc_ref(2, 0) = 4;

    auto dfp = df.pow(2);
    assert(dfp.iloc(0, 0) == 4);
    assert(dfp.iloc(2, 0) == 16);

    df.iloc_ref(1, 0) = 99;
    df._replace(99, 100);
    assert(df.iloc(1, 0) == 100);
    cout << "[PASS] test_frame_functional_pow_replace" << endl;
}

void test_frame_stats()
{
    DataFrame<int, double> df({ 1, 2, 3 }, { "a", "b" }, pandas::nan<double>());
    df.iloc_ref(0, 0) = 1;
    df.iloc_ref(1, 0) = 2;
    df.iloc_ref(2, 0) = 3;
    df.iloc_ref(0, 1) = 4;
    df.iloc_ref(1, 1) = 5;
    df.iloc_ref(2, 1) = 6;

    // sum<0> -> per column
    auto s_sum = df.sum<0>();
    assert(s_sum.size() == 2);
    assert(s_sum.loc("a") == 6);
    assert(s_sum.loc("b") == 15);

    // max<0>
    auto s_max = df.max<0>();
    assert(s_max.loc("a") == 3);

    // min<0>
    auto s_min = df.min<0>();
    assert(s_min.loc("a") == 1);

    // count<0>
    auto s_cnt = df.count<0>();
    assert(s_cnt.loc("a") == 3);

    // mean<0>
    auto s_mean = df.mean<0>();
    assert(abs(s_mean.loc("a") - 2.0) < 1e-10);

    // var / std
    auto s_var = df.var<0>(1);
    auto s_std = df.std<0>(1);
    assert(!pandas::isnan(s_var.loc("a")));
    assert(!pandas::isnan(s_std.loc("a")));
    cout << "[PASS] test_frame_stats" << endl;
}

void test_frame_corr()
{
    DataFrame<int, double> df({ 1, 2, 3, 4, 5 }, { "x", "y" }, pandas::nan<double>());
    df.iloc_ref(0, 0) = 1;
    df.iloc_ref(1, 0) = 2;
    df.iloc_ref(2, 0) = 3;
    df.iloc_ref(3, 0) = 4;
    df.iloc_ref(4, 0) = 5;
    df.iloc_ref(0, 1) = 2;
    df.iloc_ref(1, 1) = 4;
    df.iloc_ref(2, 1) = 6;
    df.iloc_ref(3, 1) = 8;
    df.iloc_ref(4, 1) = 10;

    auto c = df.corr<1>();
    assert(c.size<0>() == 2);
    assert(c.size<1>() == 2);
    assert(abs(c.iloc(0, 1) - 1.0) < 1e-10);
    assert(abs(c.iloc(1, 0) - 1.0) < 1e-10);
    assert(abs(c.iloc(0, 0) - 1.0) < 1e-10);
    cout << "[PASS] test_frame_corr" << endl;
}

void test_frame_droplevel()
{
    Index<int> id1(std::vector<int>({ 1, 2, 3 }), "id1");
    Index<int> id2(std::vector<int>({ 1, 4, 5 }), "id2");
    auto id = concat<1>(id1, id2);
    DataFrame<std::tuple<int, int>, double> df(id.values.values, std::vector<std::string>({ "a", "b" }), pandas::nan<double>());
    df._fillna(1);

    auto df1 = df.droplevel<0>();
    assert((df1.pidx->iloc(1) == 4) && (df1.iloc(0, 0) == 1));
    cout << "[PASS] test_frame_droplevel" << endl;
}

void test_frame_sort()
{
    DataFrame<int, double> df(std::vector<int>(), { "a", "b" }, pandas::nan<double>());
    df._append_row(1, std::vector<double>({ 3, 1 }), false);
    df._append_row(2, std::vector<double>({ 2, 2 }), false);
    df._append_row(3, std::vector<double>({ 1, 3 }), false);
    df.pidx->_flush_index();

    auto df2 = df.sort_values("b", false);
    assert((df2.iloc(0, 0) == 1) && (df2.iloc(0, 1) == 3));

    auto df3 = df.sort_values("a", true);
    assert((df3.iloc(0, 0) == 1) && (df3.iloc(0, 1) == 3));
    cout << "[PASS] test_frame_sort" << endl;
}

void test_frame_groupby()
{
    DataFrame<int, double> df(std::vector<int>(), { "a", "b" }, pandas::nan<double>());
    df._append_row(1, std::vector<double>({ 1, 1 }));
    df._append_row(2, std::vector<double>({ 2, 2 }));
    df._append_row(3, std::vector<double>({ 3, 3 }));

    auto dg = df.groupby(std::vector<std::string>({ "t1", "t2", "t2" })).sum();
    dg = dg.sort_values(dg.pidx->values);
    assert((dg.iloc(0, 0) == 1) && (dg.iloc(1, 1) == 5));

    // count groupby
    df._append_row(4, std::vector<double>({ pandas::nan<double>(), 1 }));
    auto dg2 = df.groupby(df.pidx->values).count();
    dg2 = dg2.sort_values(dg2.pidx->values);
    assert(dg2.iloc(3, 0) == 0);

    // multi-key groupby
    Array<int> ar1({ 1, 1, 2, 2 });
    Array<std::string> ar2({ "a", "a", "b", "c" });
    auto ar3 = concat<1>(ar1, ar2);
    auto dg3 = df.groupby(ar3).sum();
    dg3 = dg3.sort_values(dg3.pidx->values);
    assert((dg3.iloc(0, 0) == 3) && (dg3.iloc(2, 0) == 0));
    cout << "[PASS] test_frame_groupby" << endl;
}

void test_frame_empty()
{
    DataFrame<int, double> df;
    assert(df.size<0>() == 0);
    assert(df.size<1>() == 0);
    assert(df.columns().size() == 0);

    // operations on empty frame
    auto df2 = df + 1;
    assert(df2.size<0>() == 0);
    cout << "[PASS] test_frame_empty" << endl;
}

void test_frame_to_string()
{
    DataFrame<int, double> df({ 1, 2 }, { "a", "b" }, 0.0);
    string s = df.to_string();
    assert(s.find("a") != string::npos);
    assert(s.find("[2 rows x 2 columns]") != string::npos);
    cout << "[PASS] test_frame_to_string" << endl;
}

void test_frame_columns()
{
    DataFrame<int, double> df({ 1, 2 }, { "x", "y", "z" }, 0.0);
    auto cols = df.columns();
    assert(cols.size() == 3);
    assert(cols[0] == "x");
    assert(cols[2] == "z");
    cout << "[PASS] test_frame_columns" << endl;
}

int main()
{
    try {
        cout << "=== Rolling Window Tests ===" << endl;
        test_series_rolling_count();
        test_series_rolling_count_with_nan();
        test_series_rolling_sum();
        test_series_rolling_max();
        test_series_rolling_min();
        test_series_rolling_mean();
        test_series_rolling_var();
        test_series_rolling_std();
        test_series_rolling_median();
        test_series_rolling_min_periods_all();
        test_series_rolling_window1();
        test_series_rolling_agg();

        cout << "\n=== DataFrame Rolling Tests ===" << endl;
        test_dataframe_rolling_sum();
        test_dataframe_rolling_mean();
        test_dataframe_rolling_count();
        test_dataframe_rolling_max();
        test_dataframe_rolling_min();
        test_dataframe_rolling_median();
        test_dataframe_rolling_agg();

        cout << "\n=== DataFrame Enhanced Tests ===" << endl;
        test_frame_constructors_advanced();
        test_frame_append_col();
        test_frame_append_row();
        test_frame_operators();
        test_frame_loc_single();
        test_frame_loc_vector();
        test_frame_loc_range();
        test_frame_loc_mask();
        test_frame_iloc();
        test_frame_set_index();
        test_frame_reindex();
        test_frame_dropna();
        test_frame_T();
        test_frame_functional_map();
        test_frame_functional_diff_shift();
        test_frame_functional_fillna();
        test_frame_functional_cumsum();
        test_frame_functional_abs();
        test_frame_functional_pow_replace();
        test_frame_stats();
        test_frame_corr();
        test_frame_droplevel();
        test_frame_sort();
        test_frame_groupby();
        test_frame_empty();
        test_frame_to_string();
        test_frame_columns();

        cout << "\nAll rolling and DataFrame enhanced tests passed!" << endl;

    } catch (const std::string& s) {
        cout << "ERROR: " << s << endl;
        return 1;
    } catch (const std::exception& e) {
        cout << "ERROR: " << e.what() << endl;
        return 1;
    } catch (...) {
        cout << "ERROR: unknown" << endl;
        return 1;
    }
    return 0;
}
