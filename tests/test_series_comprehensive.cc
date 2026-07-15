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
#include <vector>
#include <string>
#include <cmath>
#include <memory>

using namespace std;
using namespace pandas;

// ========== Series Constructor Tests ==========
void test_series_default_construct()
{
    Series<int, double> sr;
    assert(sr.size() == 0);
    assert(sr.get_name() == "");
    assert(sr.pidx != nullptr);
    cout << "[PASS] test_series_default_construct" << endl;
}

void test_series_name_construct()
{
    Series<int, double> sr("my_series");
    assert(sr.get_name() == "my_series");
    assert(sr.size() == 0);
    cout << "[PASS] test_series_name_construct" << endl;
}

void test_series_name_idx_fillna_construct()
{
    Index<int> idx(Array<int>({ 1, 2, 3 }));
    Series<int, double> sr("vals", idx, 0.0);
    assert(sr.size() == 3);
    assert(sr.get_name() == "vals");
    assert(sr.iloc(0) == 0.0);
    assert(sr.pidx->iloc(0) == 1);
    cout << "[PASS] test_series_name_idx_fillna_construct" << endl;
}

void test_series_shared_ptr_construct()
{
    auto pidx = std::make_shared<Index<int>>();
    pidx->_append(0);
    pidx->_append(1);
    Series<int, double> sr("shared", pidx, 0.0);
    assert(sr.size() == 2);

    pidx->_append(2);
    assert(sr.size() == 3);  // shared pointer, so sr sees it
    cout << "[PASS] test_series_shared_ptr_construct" << endl;
}

void test_series_from_idx_array()
{
    Index<int> idx(Array<int>({ 1, 2, 3 }));
    Array<double> vals({ 10.0, 20.0, 30.0 }, "sr");
    Series<int, double> sr(idx, vals);
    assert(sr.size() == 3);
    assert(sr.iloc(0) == 10.0);
    assert(sr.get_name() == "sr");

    // mismatched sizes throw
    bool threw = false;
    try {
        Array<double> vals2({ 1.0, 2.0 });
        Series<int, double> sr2(idx, vals2);
    } catch (...) {
        threw = true;
    }
    assert(threw);
    cout << "[PASS] test_series_from_idx_array" << endl;
}

void test_series_copy_construct()
{
    Series<int, double> sr1(Index<int>(Array<int>({ 1, 2 })), Array<double>({ 10, 20 }, "sr1"));
    Series<int, double> sr2(sr1);
    assert(sr2.size() == 2);
    assert(sr2.iloc(0) == 10);
    assert(sr2.get_name() == "sr1");

    // By design, Series copy shares pidx (shallow)
    // So modifying sr1 pidx affects sr2 pidx
    sr1.pidx->iloc_ref(0) = 99;
    assert(sr2.pidx->iloc(0) == 99);  // shared!
    cout << "[PASS] test_series_copy_construct" << endl;
}

void test_series_copy_method()
{
    Series<int, double> sr1(Index<int>(Array<int>({ 1, 2 })), Array<double>({ 10, 20 }, "sr1"));
    auto sr2 = sr1.copy();
    sr1.pidx->iloc_ref(0) = 99;
    assert(sr2.pidx->iloc(0) == 1);  // deep copy! independent
    cout << "[PASS] test_series_copy_method" << endl;
}

void test_series_move_construct()
{
    Series<int, double> sr1(Index<int>(Array<int>({ 1, 2 })), Array<double>({ 10, 20 }, "sr1"));
    Series<int, double> sr2(std::move(sr1));
    assert(sr2.size() == 2);
    assert(sr2.iloc(0) == 10);
    cout << "[PASS] test_series_move_construct" << endl;
}

// ========== Series Operator Tests ==========
void test_series_compound_add()
{
    Series<int, double> sr(Index<int>(Array<int>({ 0, 1, 2 })), Array<double>({ 1, 2, 3 }, "sr"));
    sr += 1;
    assert(sr.sum() == 9);

    sr += 2;
    assert(sr.sum() == 15);

    sr *= 2;
    assert(sr.sum() == 30);

    sr /= 2;
    assert(sr.sum() == 15);
    cout << "[PASS] test_series_compound_add" << endl;
}

void test_series_arith_series()
{
    Series<int, double> sr1(Index<int>(Array<int>({ 0, 1, 2 })), Array<double>({ 1, 2, 3 }, "sr1"));
    auto sr2 = sr1.astype<int, int>();
    sr2 = sr2 % 2;
    auto sr3 = sr1 + sr2;
    assert(sr3.sum() == 16);

    auto sr4 = sr1 - sr2;
    assert(sr4.iloc(0) == 0);

    auto sr5 = sr1 * sr2;
    assert(sr5.iloc(1) == 0);
    cout << "[PASS] test_series_arith_series" << endl;
}

void test_series_vector_ops()
{
    Series<int, double> sr(Index<int>(Array<int>({ 1, 2 })), Array<double>({ 3, 4 }, "sr"));
    sr += 1;
    assert((sr.iloc(0) == 4) && (sr.iloc(1) == 5));

    auto sr2 = sr + vector<int>({ 1, 2 });
    assert((sr2.iloc(0) == 5) && (sr2.iloc(1) == 7));
    cout << "[PASS] test_series_vector_ops" << endl;
}

void test_series_comparison()
{
    Series<int, double> sr(Index<int>(Array<int>({ 0, 1, 2 })), Array<double>({ 1, 2, 3 }, "sr"));
    auto cmp = sr > 2;
    assert(cmp.size() == 3);
    assert(cmp.iloc(0) == 0 && cmp.iloc(2) == 1);

    auto cmp2 = sr < 2;
    assert(cmp2.iloc(0) == 1 && cmp2.iloc(1) == 0);

    auto cmp3 = sr == 2;
    assert(cmp3.iloc(1) == 1 && cmp3.iloc(0) == 0);

    auto cmp4 = sr != 2;
    assert(cmp4.iloc(1) == 0 && cmp4.iloc(0) == 1);
    cout << "[PASS] test_series_comparison" << endl;
}

void test_series_bitwise_ops()
{
    Series<int, int> sr(Index<int>(Array<int>({ 0, 1, 2 })), Array<int>({ 1, 2, 3 }, "sr"));
    auto r_and = sr & 3;
    assert(r_and.iloc(0) == 1);
    assert(r_and.iloc(2) == 3);

    auto r_or = sr | 8;
    assert(r_or.iloc(0) == 9);
    assert(r_or.iloc(1) == 10);
    cout << "[PASS] test_series_bitwise_ops" << endl;
}

void test_series_logical_ops()
{
    Series<int, char> sr(Index<int>(Array<int>({ 0, 1, 2 })), Array<char>({ true, false, true }, "sr"));
    auto not_sr = !sr;
    assert(not_sr.iloc(0) == false);
    assert(not_sr.iloc(1) == true);

    auto and_sr = sr && !sr;
    assert(and_sr.iloc(0) == false);
    assert(and_sr.iloc(1) == false);
    cout << "[PASS] test_series_logical_ops" << endl;
}

// ========== Series Loc/Iloc Tests ==========
void test_series_loc_ref()
{
    Series<int, double> sr(Index<int>(Array<int>({ 1, 2, 3 })), Array<double>({ 2, 3, 4 }, "sr"));
    sr.loc_ref(1) = 4;
    assert(sr.loc(1) == 4);
    assert(sr.loc(2) == 3);
    cout << "[PASS] test_series_loc_ref" << endl;
}

void test_series_loc_range()
{
    Series<int, double> sr(Index<int>(Array<int>({ 1, 2, 3 })), Array<double>({ 2, 3, 4 }, "sr"));
    sr.loc(2, 3) = 5;
    assert(sr.loc(2) == 5);
    assert(sr.loc(3) == 5);
    cout << "[PASS] test_series_loc_range" << endl;
}

void test_series_iloc_range()
{
    Series<int, double> sr(Index<int>(Array<int>({ 1, 2, 3, 4, 5 })), Array<double>({ 1, 2, 3, 4, 5 }, "sr"));
    sr.iloc(1, 3) = 99;
    assert(sr.iloc(1) == 99);
    assert(sr.iloc(2) == 99);
    assert(sr.iloc(0) == 1);
    assert(sr.iloc(3) == 4);
    cout << "[PASS] test_series_iloc_range" << endl;
}

void test_series_iloc_step()
{
    Series<int, double> sr(Index<int>(Array<int>({ 1, 2, 3, 4, 5 })), Array<double>({ 1, 2, 3, 4, 5 }, "sr"));
    sr.iloc(0, 5, 2) = 100;
    assert(sr.iloc(0) == 100);
    assert(sr.iloc(1) == 2);
    assert(sr.iloc(2) == 100);
    assert(sr.iloc(4) == 100);
    cout << "[PASS] test_series_iloc_step" << endl;
}

void test_series_loc_vector()
{
    Series<int, double> sr(Index<int>(Array<int>({ 1, 2, 3 })), Array<double>({ 2, 3, 4 }, "sr"));
    sr.loc(std::vector<int>({ 1, 2 })) = 7;
    assert(sr.iloc(0) == 7);
    assert(sr.iloc(1) == 7);
    assert(sr.iloc(2) == 4);
    cout << "[PASS] test_series_loc_vector" << endl;
}

void test_series_iloc_vector()
{
    Series<int, double> sr(Index<int>(Array<int>({ 1, 2, 3 })), Array<double>({ 2, 3, 4 }, "sr"));
    sr.iloc({ 1, 2 }) = 8;
    assert(sr.iloc(1) == 8);
    assert(sr.iloc(2) == 8);
    cout << "[PASS] test_series_iloc_vector" << endl;
}

void test_series_loc_mask()
{
    Series<int, double> sr(Index<int>(Array<int>({ 1, 2, 3 })), Array<double>({ 2, 3, 4 }, "sr"));
    auto mask = sr > 2;
    sr.loc_mask(mask) = 9;
    assert(sr.iloc(0) == 2);
    assert(sr.iloc(1) == 9);
    assert(sr.iloc(2) == 9);
    cout << "[PASS] test_series_loc_mask" << endl;
}

void test_series_loc_mask_vector()
{
    Series<int, double> sr(Index<int>(Array<int>({ 1, 2, 3, 4, 5 })), Array<double>({ 1, 2, 3, 4, 5 }, "sr"));
    vector<char> mask = { true, false, true, false, true };
    auto sv = sr.loc_mask(mask);
    auto sub = sv.to_series();
    assert(sub.size() == 3);
    assert(sub.iloc(0) == 1);
    assert(sub.iloc(1) == 3);
    assert(sub.iloc(2) == 5);
    cout << "[PASS] test_series_loc_mask_vector" << endl;
}

void test_series_set_by_loc_with_auto_append()
{
    Series<int, double> sr(Index<int>(Array<int>({ 1, 2, 3 })), Array<double>({ 2, 3, 4 }, "sr"));
    sr.loc_ref(10) = 99;  // auto-append
    assert(sr.size() == 4);
    assert(sr.loc(10) == 99);
    assert(pandas::isnan(sr.loc(3)));  // old elements should be NaN (but they weren't modified)
    // Actually loc_ref with new id appends NaN then assigns
    cout << "[PASS] test_series_set_by_loc_with_auto_append" << endl;
}

// ========== Series Iloc/Loc Instantiated Tests ==========
void test_series_iloc_inst()
{
    Series<int, double> sr(Index<int>(Array<int>({ 10, 20, 30, 40, 50 })), Array<double>({ 1, 2, 3, 4, 5 }, "sr"));
    auto sub = sr.iloc_inst(std::vector<int>({ 0, 2, 4 }));
    assert(sub.size() == 3);
    assert(sub.iloc(0) == 1);
    assert(sub.iloc(1) == 3);
    assert(sub.iloc(2) == 5);
    assert(sub.pidx->iloc(0) == 10);
    cout << "[PASS] test_series_iloc_inst" << endl;
}

void test_series_loc_inst()
{
    Series<int, double> sr(Index<int>(Array<int>({ 10, 20, 30, 40, 50 })), Array<double>({ 1, 2, 3, 4, 5 }, "sr"));
    auto sub = sr.loc_inst(std::vector<int>({ 10, 30, 50 }));
    assert(sub.size() == 3);
    assert(sub.iloc(0) == 1);
    assert(sub.iloc(2) == 5);
    cout << "[PASS] test_series_loc_inst" << endl;
}

void test_series_loc_mask_inst()
{
    Series<int, double> sr(Index<int>(Array<int>({ 10, 20, 30 })), Array<double>({ 1, 2, 3 }, "sr"));
    vector<char> mask = { true, false, true };
    auto sub = sr.loc_mask_inst(mask);
    assert(sub.size() == 2);
    assert(sub.iloc(0) == 1);
    assert(sub.iloc(1) == 3);
    assert(sub.pidx->iloc(0) == 10);
    assert(sub.pidx->iloc(1) == 30);
    cout << "[PASS] test_series_loc_mask_inst" << endl;
}

// ========== Series Visitor Tests ==========
void test_series_visitor_to_series()
{
    Series<int, double> sr(Index<int>(Array<int>({ 1, 2, 3, 4, 5 })), Array<double>({ 10, 20, 30, 40, 50 }, "sr"));
    auto sv = sr.iloc(std::vector<int>({ 1, 3 }));
    auto sub = sv.to_series();
    assert(sub.size() == 2);
    assert(sub.iloc(0) == 20);
    assert(sub.iloc(1) == 40);
    assert(sub.pidx->iloc(0) == 2);
    assert(sub.pidx->iloc(1) == 4);
    cout << "[PASS] test_series_visitor_to_series" << endl;
}

void test_series_visitor_to_array()
{
    Series<int, double> sr(Index<int>(Array<int>({ 1, 2, 3 })), Array<double>({ 100, 200, 300 }, "sr"));
    auto sv = sr.loc(std::vector<int>({ 1, 3 }));
    auto ar = sv.to_array();
    assert(ar.size() == 2);
    assert(ar.iloc(0) == 100);
    assert(ar.iloc(1) == 300);
    cout << "[PASS] test_series_visitor_to_array" << endl;
}

// ========== Series Functional Tests ==========
void test_series_map()
{
    Series<int, double> sr(Index<int>(Array<int>({ 1, 2, 3 })), Array<double>({ 2, 3, 4 }, "sr"));
    sr._map<double>([](const double& a) -> double { return a * a; });
    assert((sr.iloc(0) == 4) && (sr.iloc(1) == 9) && (sr.iloc(2) == 16));
    cout << "[PASS] test_series_map" << endl;
}

void test_series_map_new_type()
{
    Series<int, double> sr(Index<int>(Array<int>({ 1, 2, 3 })), Array<double>({ 1.5, 2.5, 3.5 }, "sr"));
    auto sr2 = sr.map<int>([](const double& a) -> int { return (int)a; });
    assert(sr2.size() == 3);
    assert(sr2.iloc(0) == 1);
    assert(sr2.iloc(2) == 3);
    assert(sr2.get_name() == "sr");
    cout << "[PASS] test_series_map_new_type" << endl;
}

void test_series_dropna()
{
    Series<int, double> sr(Index<int>(Array<int>({ 1, 2, 3 })), Array<double>({ 2, pandas::nan<double>(), 4 }, "sr"));
    auto sr2 = sr.dropna();
    assert(sr2.size() == 2);
    assert(sr2.iloc(0) == 2);
    assert(sr2.iloc(1) == 4);

    sr._dropna();
    assert(sr.size() == 2);
    cout << "[PASS] test_series_dropna" << endl;
}

void test_series_all_nan_dropna()
{
    Series<int, double> sr(Index<int>(Array<int>({ 1, 2, 3 })), Array<double>({ pandas::nan<double>(), pandas::nan<double>(), pandas::nan<double>() }, "sr"));
    auto sr2 = sr.dropna();
    assert(sr2.size() == 0);
    cout << "[PASS] test_series_all_nan_dropna" << endl;
}

void test_series_diff()
{
    Series<int, double> sr(Index<int>(Array<int>({ 1, 2, 3 })), Array<double>({ 2, 3, 4 }, "sr"));
    auto sr3 = sr.diff();
    assert(pandas::isnan(sr3.iloc(0)));
    assert(sr3.iloc(1) == 1);
    assert(sr3.iloc(2) == 1);

    // negative period
    auto sr4 = sr.diff(-1);
    assert(sr4.iloc(0) == 1);
    assert(sr4.iloc(1) == 1);
    assert(pandas::isnan(sr4.iloc(2)));
    cout << "[PASS] test_series_diff" << endl;
}

void test_series_shift()
{
    Series<int, double> sr(Index<int>(Array<int>({ 1, 2, 3, 4 })), Array<double>({ 10, 20, 30, 40 }, "sr"));
    auto shifted = sr.shift(2);
    assert(pandas::isnan(shifted.iloc(0)));
    assert(pandas::isnan(shifted.iloc(1)));
    assert(shifted.iloc(2) == 10);
    assert(shifted.iloc(3) == 20);

    auto shifted_neg = sr.shift(-1);
    assert(shifted_neg.iloc(0) == 20);
    assert(shifted_neg.iloc(2) == 40);
    assert(pandas::isnan(shifted_neg.iloc(3)));
    cout << "[PASS] test_series_shift" << endl;
}

void test_series_fillna()
{
    Series<int, double> sr(Index<int>(Array<int>({ 1, 2, 3, 4 })), Array<double>({ 2, pandas::nan<double>(), 4, pandas::nan<double>() }, "sr"));
    sr._fillna(0);
    assert(sr.iloc(0) == 2);
    assert(sr.iloc(1) == 0);
    assert(sr.iloc(2) == 4);
    assert(sr.iloc(3) == 0);
    cout << "[PASS] test_series_fillna" << endl;
}

void test_series_ffill()
{
    Series<int, double> sr(Index<int>(Array<int>({ 1, 2, 3, 4 })), Array<double>({ 1.0, pandas::nan<double>(), pandas::nan<double>(), 4.0 }, "sr"));
    sr._ffill(0.0, 1);
    assert(sr.iloc(0) == 1.0);
    assert(sr.iloc(1) == 1.0);  // filled from prev
    assert(pandas::isnan(sr.iloc(2)));  // limit=1, so not filled
    assert(sr.iloc(3) == 4.0);
    cout << "[PASS] test_series_ffill" << endl;
}

void test_series_bfill()
{
    Series<int, double> sr(Index<int>(Array<int>({ 1, 2, 3, 4 })), Array<double>({ pandas::nan<double>(), pandas::nan<double>(), 3.0, 4.0 }, "sr"));
    sr._bfill(0.0, 1);
    assert(sr.iloc(0) == 3.0);  // filled from next
    assert(pandas::isnan(sr.iloc(1)));  // limit=1, not filled
    assert(sr.iloc(2) == 3.0);
    assert(sr.iloc(3) == 4.0);
    cout << "[PASS] test_series_bfill" << endl;
}

void test_series_cumsum()
{
    Series<int, double> sr(Index<int>(Array<int>({ 1, 2, 3, 4 })), Array<double>({ 1, 2, 3, 4 }, "sr"));
    auto cs = sr.cumsum();
    assert(cs.iloc(0) == 1);
    assert(cs.iloc(1) == 3);
    assert(cs.iloc(2) == 6);
    assert(cs.iloc(3) == 10);
    assert(sr.iloc(0) == 1);  // original unchanged
    cout << "[PASS] test_series_cumsum" << endl;
}

void test_series_pow()
{
    Series<int, double> sr(Index<int>(Array<int>({ 1, 2, 3 })), Array<double>({ 2, 3, 4 }, "sr"));
    auto p = sr.pow(2);
    assert(p.iloc(0) == 4);
    assert(p.iloc(1) == 9);
    assert(p.iloc(2) == 16);
    cout << "[PASS] test_series_pow" << endl;
}

void test_series_abs()
{
    Series<int, double> sr(Index<int>(Array<int>({ 1, 2, 3 })), Array<double>({ -2, -3, 4 }, "sr"));
    auto a = sr.abs();
    assert(a.iloc(0) == 2);
    assert(a.iloc(1) == 3);
    assert(a.iloc(2) == 4);
    cout << "[PASS] test_series_abs" << endl;
}

void test_series_reciprocal()
{
    Series<int, double> sr(Index<int>(Array<int>({ 1, 2, 3 })), Array<double>({ 2, 4, 8 }, "sr"));
    auto r = sr.reciprocal();
    assert(r.iloc(0) == 0.5);
    assert(r.iloc(1) == 0.25);
    assert(r.iloc(2) == 0.125);
    cout << "[PASS] test_series_reciprocal" << endl;
}

void test_series_replace()
{
    Series<int, double> sr(Index<int>(Array<int>({ 1, 2, 3 })), Array<double>({ 1, 2, 1 }, "sr"));
    sr._replace(1, 99);
    assert(sr.iloc(0) == 99);
    assert(sr.iloc(1) == 2);
    assert(sr.iloc(2) == 99);
    cout << "[PASS] test_series_replace" << endl;
}

void test_series_droplevel()
{
    Index<int> id1(std::vector<int>({ 1, 2, 3 }), "id1");
    Index<int> id2(std::vector<int>({ 1, 4, 5 }), "id2");
    auto id = concat<1>(id1, id2);
    auto sr = Series<decltype(id.iloc(0)), int>(id, std::vector<int>({ 10, 11, 12 }));
    auto sr2 = sr.droplevel<0>();
    assert((sr2.pidx->iloc(0) == 1) && (sr2.iloc(0) == 10));
    cout << "[PASS] test_series_droplevel" << endl;
}

void test_series_where()
{
    Series<int, double> sr(Index<int>(Array<int>({ 1, 2, 3, 4 })), Array<double>({ 2, 3, 4, pandas::nan<double>() }, "sr"));
    auto sr8 = sr.where(sr > 3, 1);
    assert((sr8.iloc(3) == 1) && (sr8.iloc(2) == 4) && (sr8.iloc(0) == 1));
    cout << "[PASS] test_series_where" << endl;
}

void test_series_where_with_vector()
{
    Series<int, double> sr(Index<int>(Array<int>({ 1, 2, 3 })), Array<double>({ 10, 20, 30 }, "sr"));
    auto r = sr.where(vector<char>({ true, false, true }), 0);
    assert(r.iloc(0) == 10);
    assert(r.iloc(1) == 0);
    assert(r.iloc(2) == 30);
    cout << "[PASS] test_series_where_with_vector" << endl;
}

void test_series_duplicated()
{
    Series<int, double> sr(Index<int>(Array<int>({ 1, 2, 2, 3, 3, 3 })), Array<double>({ 1, 2, 3, 4, 5, 6 }, "sr"));
    auto dup = sr.duplicated("first");
    assert(dup.size() == 6);
    // The duplicated method on Series wraps Array::duplicated based on values, not index
    // values are {1,2,3,4,5,6} — all unique
    for (int i = 0; i < 6; i++) {
        assert(dup.iloc(i) == false);
    }
    cout << "[PASS] test_series_duplicated" << endl;
}

void test_series_drop_duplicates()
{
    Series<int, double> sr(Index<int>(Array<int>({ 1, 2, 2, 3 })), Array<double>({ 1, 1, 2, 2 }, "sr"));
    auto dd = sr.drop_duplicates("first");
    assert(dd.size() == 2);  // values {1,2}
    assert(dd.iloc(0) == 1);
    assert(dd.iloc(1) == 2);
    cout << "[PASS] test_series_drop_duplicates" << endl;
}

void test_series_rename()
{
    Series<int, double> sr(Index<int>(Array<int>({ 1, 2, 3 })), Array<double>({ 1, 2, 3 }, "old"));
    auto sr2 = sr.rename(std::string("new"));
    assert(sr2.get_name() == "new");
    assert(sr.get_name() == "old");  // original unchanged
    cout << "[PASS] test_series_rename" << endl;
}

void test_series_astype()
{
    Series<int, double> sr(Index<int>(Array<int>({ 1, 2, 3 })), Array<double>({ 1.5, 2.5, 3.5 }, "sr"));
    auto si = sr.astype<int, int, string, string>();
    assert(si.size() == 3);
    assert(si.iloc(0) == 1);
    assert(si.iloc(2) == 3);
    assert(si.pidx->iloc(0) == 1);
    cout << "[PASS] test_series_astype" << endl;
}

// ========== Series Sort Tests ==========
void test_series_sort_index_ascending()
{
    Series<int, double> sr(Index<int>(Array<int>({ 3, 1, 2 })), Array<double>({ 30, 10, 20 }, "sr"));
    auto sorted = sr.sort_index();
    assert(sorted.size() == 3);
    assert(sorted.iloc(0) == 10);  // index 1
    assert(sorted.iloc(1) == 20);  // index 2
    assert(sorted.iloc(2) == 30);  // index 3
    cout << "[PASS] test_series_sort_index_ascending" << endl;
}

void test_series_sort_index_descending()
{
    Series<int, double> sr(Index<int>(Array<int>({ 1, 2, 3 })), Array<double>({ 10, 20, 30 }, "sr"));
    auto sorted = sr.sort_index(false);
    assert(sorted.iloc(0) == 30);
    assert(sorted.iloc(2) == 10);
    cout << "[PASS] test_series_sort_index_descending" << endl;
}

void test_series_sort_values_ascending()
{
    Series<int, double> sr(Index<int>(Array<int>({ 1, 2, 3, 4, 5 })), Array<double>({ 4, 3, 2, 1, pandas::nan<double>() }, "sr"));
    auto s = sr.sort_values();
    assert(s.iloc(0) == 1);
    cout << "[PASS] test_series_sort_values_ascending" << endl;
}

void test_series_sort_values_by_key()
{
    Series<int, double> sr(Index<int>(Array<int>({ 1, 2, 3, 4, 5 })), Array<double>({ 4, 3, 2, 1, pandas::nan<double>() }, "sr"));
    auto s = sr.sort_values(std::vector<int>({ 4, 3, 1, 2, 0 }));
    assert(pandas::isnan(s.iloc(0)));
    cout << "[PASS] test_series_sort_values_by_key" << endl;
}

void test_series_sort_values_by_series()
{
    Series<int, double> sr(Index<int>(Array<int>({ 1, 2, 3, 4, 5 })), Array<double>({ 4, 3, 2, 1, pandas::nan<double>() }, "sr"));
    auto s = sr.sort_values(sr.fillna(100));
    assert(s.iloc(0) == 1);
    assert(s.iloc(4) == 4);
    cout << "[PASS] test_series_sort_values_by_series" << endl;
}

// ========== Series Stats Tests ==========
void test_series_sum()
{
    Series<int, double> sr(Index<int>(Array<int>({ 1, 2, 3, 4, 5 })), Array<double>({ 1, 2, 3, 4, 5 }, "sr"));
    assert(sr.sum() == 15);
    cout << "[PASS] test_series_sum" << endl;
}

void test_series_sum_with_nan()
{
    Series<int, double> sr(Index<int>(Array<int>({ 1, 2, 3 })), Array<double>({ 1, pandas::nan<double>(), 3 }, "sr"));
    assert(sr.sum() == 4);
    cout << "[PASS] test_series_sum_with_nan" << endl;
}

void test_series_max()
{
    Series<int, double> sr(Index<int>(Array<int>({ 1, 2, 3 })), Array<double>({ 3, 1, 4 }, "sr"));
    assert(sr.max() == 4);
    cout << "[PASS] test_series_max" << endl;
}

void test_series_min()
{
    Series<int, double> sr(Index<int>(Array<int>({ 1, 2, 3 })), Array<double>({ 3, 1, 4 }, "sr"));
    assert(sr.min() == 1);
    cout << "[PASS] test_series_min" << endl;
}

void test_series_count()
{
    Series<int, double> sr(Index<int>(Array<int>({ 1, 2, 3, 4, 5 })), Array<double>({ 1, pandas::nan<double>(), 3, pandas::nan<double>(), 5 }, "sr"));
    assert(sr.count() == 3);
    cout << "[PASS] test_series_count" << endl;
}

void test_series_mean()
{
    Series<int, double> sr(Index<int>(Array<int>({ 1, 2, 3 })), Array<double>({ 2, 4, 6 }, "sr"));
    assert(sr.mean() == 4.0);
    cout << "[PASS] test_series_mean" << endl;
}

void test_series_var()
{
    Series<int, double> sr(Index<int>(Array<int>({ 1, 2, 3 })), Array<double>({ 2, 4, 6 }, "sr"));
    double v = sr.var(1);
    assert(abs(v - 4.0) < 1e-10);
    cout << "[PASS] test_series_var" << endl;
}

void test_series_std()
{
    Series<int, double> sr(Index<int>(Array<int>({ 1, 2, 3 })), Array<double>({ 2, 4, 6 }, "sr"));
    double s = sr.std(1);
    assert(abs(s - 2.0) < 1e-10);
    cout << "[PASS] test_series_std" << endl;
}

void test_series_median()
{
    Series<int, double> sr(Index<int>(Array<int>({ 1, 2, 3, 4, 5 })), Array<double>({ 3, 1, 4, 1, 5 }, "sr"));
    double m = sr.median();
    // After sort: [1,1,3,4,5], median=3
    assert(abs(m - 3.0) < 1e-10);
    cout << "[PASS] test_series_median" << endl;
}

void test_series_corr()
{
    Series<int, double> sr1(Index<int>(Array<int>({ 1, 2, 3, 4, 5 })), Array<double>({ 1, 2, 3, 4, 5 }, "sr1"));
    Series<int, double> sr2(Index<int>(Array<int>({ 1, 2, 3, 4, 5 })), Array<double>({ 2, 4, 6, 8, 10 }, "sr2"));
    double c = sr1.corr(sr2);
    assert(abs(c - 1.0) < 1e-10);
    cout << "[PASS] test_series_corr" << endl;
}

void test_series_corr_mismatched_index()
{
    Series<int, double> sr1(Index<int>(Array<int>({ 1, 2, 3 })), Array<double>({ 1, 2, 3 }, "sr1"));
    // sr2 has same keys but values are measured differently — some may be same causing NaN
    Series<int, double> sr2(Index<int>(Array<int>({ 2, 4, 6 })), Array<double>({ 5, 7, 9 }, "sr2"));
    // corr operates by iterating sr1's index and calling y.loc(id), which throws if id not in y
    // Since 1 is in sr1 but not sr2, this will throw.
    // According to the code, corr doesn't handle mismatched indices gracefully.
    // The key 2 overlaps, keys 1 and 3 don't — corr iterates through ALL of sr1's index
    bool threw = false;
    try {
        double c = sr1.corr(sr2);
        (void)c;
    } catch (...) {
        threw = true;
    }
    assert(threw);
    cout << "[PASS] test_series_corr_mismatched_index" << endl;
}

void test_series_corr_with_array()
{
    Series<int, double> sr(Index<int>(Array<int>({ 1, 2, 3, 4, 5 })), Array<double>({ 1, 2, 3, 4, 5 }, "sr"));
    Array<double> y({ 2, 4, 6, 8, 10 });
    double c = sr.corr(y);
    assert(abs(c - 1.0) < 1e-10);
    cout << "[PASS] test_series_corr_with_array" << endl;
}

// ========== Series Reindex Tests ==========
void test_series_reindex_vector()
{
    Series<int, double> sr(Index<int>(Array<int>({ 1, 2, 3 })), Array<double>({ 10, 20, 30 }, "sr"));
    auto sr2 = sr.reindex(std::vector<int>({ 1, 3, 5 }));
    assert(sr2.size() == 3);
    assert(sr2.iloc(0) == 10);
    assert(sr2.iloc(1) == 30);
    assert(pandas::isnan(sr2.iloc(2)));  // 5 not present
    cout << "[PASS] test_series_reindex_vector" << endl;
}

void test_series_reindex_array()
{
    Series<int, double> sr(Index<int>(Array<int>({ 1, 2, 3 })), Array<double>({ 10, 20, 30 }, "sr"));
    auto sr2 = sr.reindex(Array<int>({ 2, 3 }));
    assert(sr2.size() == 2);
    assert(sr2.iloc(0) == 20);
    cout << "[PASS] test_series_reindex_array" << endl;
}

void test_series_reindex_duplicate_index()
{
    Series<int, double> sr(Index<int>(Array<int>({ 1, 2, 2, 3 })), Array<double>({ 10, 20, 21, 30 }, "sr"));
    bool threw = false;
    try {
        // loc with duplicate key should throw
        sr.loc(2);
    } catch (...) {
        threw = true;
    }
    assert(threw);
    cout << "[PASS] test_series_reindex_duplicate_index" << endl;
}

// ========== Series Edge Cases ==========
void test_series_empty()
{
    Series<int, double> sr;
    assert(sr.size() == 0);
    assert(sr.sum() == 0);
    assert(sr.count() == 0);
    cout << "[PASS] test_series_empty" << endl;
}

void test_series_append()
{
    Series<int, double> sr("sr");
    sr._append(1, 10.0);
    sr._append(2, 20.0);
    assert(sr.size() == 2);
    assert(sr.loc(1) == 10.0);
    assert(sr.loc(2) == 20.0);
    cout << "[PASS] test_series_append" << endl;
}

void test_series_to_string()
{
    Series<int, double> sr(Index<int>(Array<int>({ 1, 2, 3 })), Array<double>({ 1, 2, 3 }, "sr"));
    string s = sr.to_string();
    assert(s.find("sr") != string::npos);
    assert(s.find("1") != string::npos);
    assert(s.find("3") != string::npos);
    cout << "[PASS] test_series_to_string" << endl;
}

void test_series_string_values()
{
    Series<int, std::string> sr(Index<int>(Array<int>({ 1, 2 })), Array<std::string>({ "hello", "world" }, "sr"));
    assert(sr.iloc(0) == "hello");
    assert(sr.iloc(1) == "world");
    auto cmp = sr == std::string("hello");
    assert(cmp.iloc(0) == true);
    assert(cmp.iloc(1) == false);
    cout << "[PASS] test_series_string_values" << endl;
}

void test_series_string_index()
{
    Series<std::string, double> sr(Index<std::string>(Array<std::string>({ "a", "b", "c" })),
        Array<double>({ 1, 2, 3 }, "sr"));
    assert(sr.loc("a") == 1);
    assert(sr.loc("c") == 3);
    sr.loc_ref("b") = 99;
    assert(sr.loc("b") == 99);
    cout << "[PASS] test_series_string_index" << endl;
}

void test_series_groupby_sum()
{
    Series<int, int> sr(Index<int>(Range<int>(10, 0, -1)), Array<int>(Range<int>(0, 10).to_vec(), "sr"));
    Array<int> key(Range<int>(0, 10).to_vec());
    key %= 2;
    auto dg = sr.groupby(key).sum();
    assert(dg.iloc(0) == 20);
    assert(dg.iloc(1) == 25);
    cout << "[PASS] test_series_groupby_sum" << endl;
}

void test_series_groupby_apply()
{
    Series<int, int> sr(Index<int>(Range<int>(10, 0, -1)), Array<int>(Range<int>(0, 10).to_vec(), "sr"));
    Array<int> key(Range<int>(0, 10).to_vec());
    key %= 2;

    using SV = Series<int, int>::SeriesVisitor<RangeVec<int>>;
    auto dg = sr.groupby(key).apply<int, int, std::string, std::string>([](SV& sv) -> Series<int, int> {
        auto ds = sv.to_series().sort_values().iloc(std::vector<int>({ 0, 1 })).to_series();
        return ds;
    });
    assert((dg.iloc(0) == 0) && (dg.iloc(2) == 1));
    cout << "[PASS] test_series_groupby_apply" << endl;
}

void test_series_groupby_inst_sum()
{
    Series<int, int> sr(Index<int>(Range<int>(10, 0, -1)), Array<int>(Range<int>(0, 10).to_vec(), "sr"));
    Array<int> key(Range<int>(0, 10).to_vec());
    key %= 2;
    auto dg = sr.groupbyinst(key).sum();
    assert(dg.iloc(0) == 20);
    assert(dg.iloc(1) == 25);
    cout << "[PASS] test_series_groupby_inst_sum" << endl;
}

void test_series_groupby_inst_apply()
{
    Series<int, int> sr(Index<int>(Range<int>(10, 0, -1)), Array<int>(Range<int>(0, 10).to_vec(), "sr"));
    Array<int> key(Range<int>(0, 10).to_vec());
    key %= 2;

    auto dg = sr.groupbyinst(key).apply<int, int, std::string, std::string>([](Series<int,int>& sv) -> Series<int, int> {
        auto ds = sv.sort_values().iloc(std::vector<int>({ 0, 1 })).to_series();
        return ds;
    });
    assert((dg.iloc(0) == 0) && (dg.iloc(2) == 1));
    cout << "[PASS] test_series_groupby_inst_apply" << endl;
}

int main()
{
    try {
        // Constructor Tests
        test_series_default_construct();
        test_series_name_construct();
        test_series_name_idx_fillna_construct();
        test_series_shared_ptr_construct();
        test_series_from_idx_array();
        test_series_copy_construct();
        test_series_copy_method();
        test_series_move_construct();

        // Operator Tests
        test_series_compound_add();
        test_series_arith_series();
        test_series_vector_ops();
        test_series_comparison();
        test_series_bitwise_ops();
        test_series_logical_ops();

        // Loc/Iloc Tests
        test_series_loc_ref();
        test_series_loc_range();
        test_series_iloc_range();
        test_series_iloc_step();
        test_series_loc_vector();
        test_series_iloc_vector();
        test_series_loc_mask();
        test_series_loc_mask_vector();
        test_series_set_by_loc_with_auto_append();

        // Instantiated loc/iloc Tests
        test_series_iloc_inst();
        test_series_loc_inst();
        test_series_loc_mask_inst();

        // Visitor Tests
        test_series_visitor_to_series();
        test_series_visitor_to_array();

        // Functional Tests
        test_series_map();
        test_series_map_new_type();
        test_series_dropna();
        test_series_all_nan_dropna();
        test_series_diff();
        test_series_shift();
        test_series_fillna();
        test_series_ffill();
        test_series_bfill();
        test_series_cumsum();
        test_series_pow();
        test_series_abs();
        test_series_reciprocal();
        test_series_replace();
        test_series_droplevel();
        test_series_where();
        test_series_where_with_vector();
        test_series_duplicated();
        test_series_drop_duplicates();
        test_series_rename();
        test_series_astype();

        // Sort Tests
        test_series_sort_index_ascending();
        test_series_sort_index_descending();
        test_series_sort_values_ascending();
        test_series_sort_values_by_key();
        test_series_sort_values_by_series();

        // Stats Tests
        test_series_sum();
        test_series_sum_with_nan();
        test_series_max();
        test_series_min();
        test_series_count();
        test_series_mean();
        test_series_var();
        test_series_std();
        test_series_median();
        test_series_corr();
        test_series_corr_mismatched_index();
        test_series_corr_with_array();

        // Reindex Tests
        test_series_reindex_vector();
        test_series_reindex_array();
        test_series_reindex_duplicate_index();

        // Edge Case Tests
        test_series_empty();
        test_series_append();
        test_series_to_string();
        test_series_string_values();
        test_series_string_index();

        // GroupBy Tests
        test_series_groupby_sum();
        test_series_groupby_apply();
        test_series_groupby_inst_sum();
        test_series_groupby_inst_apply();

        cout << "\nAll Series comprehensive tests passed!" << endl;

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
