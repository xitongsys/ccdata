#include "pandas/array.h"
#include "pandas/concat.h"
#include "pandas/index.h"
#include "pandas/ops.h"

#include <cassert>
#include <iostream>
#include <vector>
#include <string>
#include <cmath>

using namespace std;
using namespace pandas;

// ========== Array Constructor Tests ==========
void test_array_default_construct()
{
    Array<int> ar;
    assert(ar.size() == 0);
    assert(ar.get_name() == "");
    cout << "[PASS] test_array_default_construct" << endl;
}

void test_array_name_construct()
{
    Array<int> ar("myarr");
    assert(ar.get_name() == "myarr");
    assert(ar.size() == 0);
    cout << "[PASS] test_array_name_construct" << endl;
}

void test_array_size_init_construct()
{
    Array<double> ar(5, 3.14, "fill");
    assert(ar.size() == 5);
    assert(ar.get_name() == "fill");
    for (int i = 0; i < 5; i++) {
        assert(ar.iloc(i) == 3.14);
    }
    cout << "[PASS] test_array_size_init_construct" << endl;
}

void test_array_vector_construct()
{
    vector<int> v = { 1, 2, 3, 4, 5 };
    Array<int> ar(v, "vec");
    assert(ar.size() == 5);
    assert(ar.get_name() == "vec");
    assert(ar.iloc(0) == 1);
    assert(ar.iloc(4) == 5);
    cout << "[PASS] test_array_vector_construct" << endl;
}

void test_array_vector_rvalue_construct()
{
    Array<int> ar(vector<int>{ 10, 20, 30 }, "rv");
    assert(ar.size() == 3);
    assert(ar.iloc(1) == 20);
    cout << "[PASS] test_array_vector_rvalue_construct" << endl;
}

void test_array_initlist_construct()
{
    Array<int> ar({ 7, 8, 9 }, "init");
    assert(ar.size() == 3);
    assert(ar.iloc(2) == 9);
    cout << "[PASS] test_array_initlist_construct" << endl;
}

void test_array_copy_construct()
{
    Array<int> ar1({ 1, 2, 3 }, "orig");
    Array<int> ar2(ar1);
    assert(ar2.size() == 3);
    assert(ar2.get_name() == "orig");
    assert(ar2.iloc(0) == 1);

    // verify independence
    ar1.iloc_ref(0) = 99;
    assert(ar2.iloc(0) == 1);  // copy is independent
    cout << "[PASS] test_array_copy_construct" << endl;
}

void test_array_move_construct()
{
    Array<int> ar1({ 1, 2, 3 }, "move");
    Array<int> ar2(std::move(ar1));
    assert(ar2.size() == 3);
    assert(ar2.iloc(1) == 2);
    cout << "[PASS] test_array_move_construct" << endl;
}

// ========== Array Assignment Tests ==========
void test_array_copy_assign()
{
    Array<int> ar1({ 1, 2 });
    Array<int> ar2({ 3, 4, 5 });
    ar2 = ar1;
    assert(ar2.size() == 2);
    assert(ar2.iloc(0) == 1);
    cout << "[PASS] test_array_copy_assign" << endl;
}

void test_array_move_assign()
{
    Array<int> ar1({ 1, 2 });
    Array<int> ar2;
    ar2 = std::move(ar1);
    assert(ar2.size() == 2);
    assert(ar2.iloc(1) == 2);
    cout << "[PASS] test_array_move_assign" << endl;
}

void test_array_cross_type_assign()
{
    Array<double> ar1({ 1.5, 2.5 });
    Array<int> ar2({ 10, 20 });
    ar2 = ar1;
    assert(ar2.size() == 2);
    assert(ar2.iloc(0) == 1);  // double truncated to int
    assert(ar2.iloc(1) == 2);
    cout << "[PASS] test_array_cross_type_assign" << endl;
}

// ========== Array Modifier Tests ==========
void test_array_append()
{
    Array<int> ar;
    for (int i = 0; i < 10; i++) {
        ar._append(i * i);
    }
    assert(ar.size() == 10);
    assert(ar.iloc(9) == 81);
    cout << "[PASS] test_array_append" << endl;
}

void test_array_append_array()
{
    Array<int> ar1({ 1, 2, 3 });
    Array<double> ar2({ 4.2, 5.7 });
    ar1._append(ar2);
    assert(ar1.size() == 5);
    assert(ar1.iloc(3) == 4);  // double truncated to int
    assert(ar1.iloc(4) == 5);
    cout << "[PASS] test_array_append_array" << endl;
}

void test_array_clear()
{
    Array<int> ar({ 1, 2, 3 });
    ar._clear();
    assert(ar.size() == 0);
    cout << "[PASS] test_array_clear" << endl;
}

void test_array_rename()
{
    Array<int> ar({ 1, 2 }, "old");
    assert(ar.get_name() == "old");

    ar._rename("new");
    assert(ar.get_name() == "new");

    auto ar2 = ar.rename("another");
    assert(ar2.get_name() == "another");
    assert(ar.get_name() == "new");  // original unchanged
    cout << "[PASS] test_array_rename" << endl;
}

void test_array_iloc_ref()
{
    Array<int> ar({ 10, 20, 30 });
    ar.iloc_ref(1) = 25;
    assert(ar.iloc(1) == 25);

    ar.iloc_ref(0) = ar.iloc_ref(2);
    assert(ar.iloc(0) == 30);
    cout << "[PASS] test_array_iloc_ref" << endl;
}

void test_array_sort_ascending()
{
    Array<int> ar({ 3, 1, 4, 1, 5, 9, 2, 6 });
    auto sorted = ar.sort(true);
    for (int i = 1; i < sorted.size(); i++) {
        assert(sorted.iloc(i - 1) <= sorted.iloc(i));
    }
    cout << "[PASS] test_array_sort_ascending" << endl;
}

void test_array_sort_descending()
{
    Array<int> ar({ 3, 1, 4, 1, 5 });
    auto sorted = ar.sort(false);
    for (int i = 1; i < sorted.size(); i++) {
        assert(sorted.iloc(i - 1) >= sorted.iloc(i));
    }
    cout << "[PASS] test_array_sort_descending" << endl;
}

void test_array_sort_inplace()
{
    Array<int> ar({ 5, 3, 1, 4, 2 });
    ar._sort(true);
    for (int i = 1; i < ar.size(); i++) {
        assert(ar.iloc(i - 1) <= ar.iloc(i));
    }
    cout << "[PASS] test_array_sort_inplace" << endl;
}

void test_array_reverse()
{
    Array<int> ar({ 1, 2, 3, 4, 5 });
    auto rev = ar.reverse();
    assert(rev.iloc(0) == 5);
    assert(rev.iloc(4) == 1);

    ar._reverse();
    assert(ar.iloc(0) == 5);
    assert(ar.iloc(4) == 1);
    cout << "[PASS] test_array_reverse" << endl;
}

// ========== Array Operator Tests ==========
void test_array_arith_scalar()
{
    Array<int> ar({ 1, 2, 3 });
    auto r1 = ar + 10;
    assert(r1.iloc(0) == 11 && r1.iloc(2) == 13);

    auto r2 = ar - 1;
    assert(r2.iloc(0) == 0 && r2.iloc(2) == 2);

    auto r3 = ar * 3;
    assert(r3.iloc(0) == 3 && r3.iloc(2) == 9);

    auto r4 = ar / 2;
    assert(r4.iloc(0) == 0 && r4.iloc(2) == 1);

    auto r5 = ar % 2;
    assert(r5.iloc(0) == 1 && r5.iloc(1) == 0);
    cout << "[PASS] test_array_arith_scalar" << endl;
}

void test_array_arith_array()
{
    Array<int> a1({ 1, 2, 3 });
    Array<int> a2({ 4, 5, 6 });

    auto r1 = a1 + a2;
    assert(r1.iloc(0) == 5 && r1.iloc(2) == 9);

    auto r2 = a1 - a2;
    assert(r2.iloc(0) == -3 && r2.iloc(2) == -3);

    auto r3 = a1 * a2;
    assert(r3.iloc(0) == 4 && r3.iloc(2) == 18);

    cout << "[PASS] test_array_arith_array" << endl;
}

void test_array_cmp_scalar()
{
    Array<int> ar({ 1, 2, 3, 4, 5 });
    auto r = ar > 2;
    assert(r.size() == 5);
    assert(r.iloc(0) == false);
    assert(r.iloc(2) == true);
    assert(r.iloc(4) == true);

    auto r2 = ar == 3;
    assert(r2.iloc(2) == true);
    assert(r2.iloc(0) == false);

    cout << "[PASS] test_array_cmp_scalar" << endl;
}

void test_array_cmp_array()
{
    Array<int> a1({ 1, 2, 3 });
    Array<int> a2({ 3, 2, 1 });
    auto r = a1 < a2;
    assert(r.iloc(0) == true);
    assert(r.iloc(1) == false);
    assert(r.iloc(2) == false);
    cout << "[PASS] test_array_cmp_array" << endl;
}

void test_array_compound_assign()
{
    Array<int> ar({ 1, 2, 3 });
    ar += 10;
    assert(ar.iloc(0) == 11 && ar.iloc(2) == 13);
    ar -= 1;
    assert(ar.iloc(0) == 10 && ar.iloc(2) == 12);
    ar *= 2;
    assert(ar.iloc(0) == 20 && ar.iloc(2) == 24);
    ar /= 4;
    assert(ar.iloc(0) == 5 && ar.iloc(2) == 6);
    cout << "[PASS] test_array_compound_assign" << endl;
}

void test_array_logical_ops()
{
    Array<char> ar({ true, false, true });
    auto r = !ar;
    assert(r.iloc(0) == false);
    assert(r.iloc(1) == true);
    assert(r.iloc(2) == false);

    Array<char> ar2({ true, true, false });
    auto r2 = ar && ar2;
    assert(r2.iloc(0) == true);
    assert(r2.iloc(1) == false);
    assert(r2.iloc(2) == false);

    auto r3 = ar || ar2;
    assert(r3.iloc(0) == true);
    assert(r3.iloc(1) == true);
    assert(r3.iloc(2) == true);
    cout << "[PASS] test_array_logical_ops" << endl;
}

// ========== Array Functional Tests ==========
void test_array_map()
{
    Array<int> ar({ 1, 2, 3 });
    auto r = ar.map<int>([](const int& a) -> int { return a * a; });
    assert(r.iloc(0) == 1 && r.iloc(2) == 9);

    ar._map<int>([](const int& a) -> int { return a * 10; });
    assert(ar.iloc(0) == 10 && ar.iloc(2) == 30);
    cout << "[PASS] test_array_map" << endl;
}

void test_array_diff()
{
    Array<double> ar({ 1.0, 3.0, 6.0, 10.0 });
    auto d = ar.diff(1);
    assert(pandas::isnan(d.iloc(0)));
    assert(d.iloc(1) == 2.0);
    assert(d.iloc(2) == 3.0);
    assert(d.iloc(3) == 4.0);

    // negative period
    auto dneg = ar.diff(-1);
    assert(dneg.iloc(0) == 2.0);
    assert(dneg.iloc(1) == 3.0);
    assert(dneg.iloc(2) == 4.0);
    assert(pandas::isnan(dneg.iloc(3)));

    // in-place
    Array<double> ar2({ 1.0, 3.0, 6.0, 10.0 });
    ar2._diff(1);
    assert(pandas::isnan(ar2.iloc(0)));
    assert(ar2.iloc(1) == 2.0);

    cout << "[PASS] test_array_diff" << endl;
}

void test_array_shift()
{
    Array<double> ar({ 1.0, 2.0, 3.0, 4.0, 5.0 });
    auto s = ar.shift(2);
    assert(pandas::isnan(s.iloc(0)));
    assert(pandas::isnan(s.iloc(1)));
    assert(s.iloc(2) == 1.0);
    assert(s.iloc(4) == 3.0);

    // negative shift
    auto sneg = ar.shift(-2);
    assert(sneg.iloc(0) == 3.0);
    assert(sneg.iloc(2) == 5.0);
    assert(pandas::isnan(sneg.iloc(3)));
    assert(pandas::isnan(sneg.iloc(4)));

    // shift by 0
    auto s0 = ar.shift(0);
    assert(s0.iloc(0) == 1.0 && s0.iloc(4) == 5.0);

    cout << "[PASS] test_array_shift" << endl;
}

void test_array_fillna()
{
    Array<double> ar({ 1.0, pandas::nan<double>(), 3.0, pandas::nan<double>() });
    ar._fillna(0.0);
    assert(ar.iloc(0) == 1.0);
    assert(ar.iloc(1) == 0.0);
    assert(ar.iloc(2) == 3.0);
    assert(ar.iloc(3) == 0.0);

    Array<double> ar2({ 1.0, pandas::nan<double>(), 3.0 });
    auto r = ar2.fillna(-1.0);
    assert(r.iloc(1) == -1.0);
    assert(ar2.iloc(1) != -1.0);  // original unchanged
    cout << "[PASS] test_array_fillna" << endl;
}

void test_array_ffill()
{
    Array<double> ar({ 1.0, pandas::nan<double>(), pandas::nan<double>(), 4.0 });
    ar._ffill(0.0, 1);
    // only first NaN after 1.0 gets filled
    assert(ar.iloc(0) == 1.0);
    assert(ar.iloc(1) == 1.0);
    assert(pandas::isnan(ar.iloc(2)));
    assert(ar.iloc(3) == 4.0);

    // ffill with limit=2
    Array<double> ar2({ 1.0, pandas::nan<double>(), pandas::nan<double>(), pandas::nan<double>(), 5.0 });
    ar2._ffill(0.0, 2);
    assert(ar2.iloc(1) == 1.0);
    assert(ar2.iloc(2) == 1.0);
    assert(pandas::isnan(ar2.iloc(3)));  // beyond limit
    assert(ar2.iloc(4) == 5.0);

    cout << "[PASS] test_array_ffill" << endl;
}

void test_array_bfill()
{
    Array<double> ar({ pandas::nan<double>(), pandas::nan<double>(), 3.0, pandas::nan<double>(), 5.0 });
    ar._bfill(0.0, 1);
    // only first NaN after position is backfilled by 1
    assert(ar.iloc(0) == 3.0);
    assert(pandas::isnan(ar.iloc(1)));  // beyond limit
    assert(ar.iloc(2) == 3.0);
    assert(ar.iloc(3) == 5.0);
    assert(ar.iloc(4) == 5.0);

    cout << "[PASS] test_array_bfill" << endl;
}

void test_array_cumsum()
{
    Array<int> ar({ 1, 2, 3, 4 });
    ar._cumsum();
    assert(ar.iloc(0) == 1);
    assert(ar.iloc(1) == 3);
    assert(ar.iloc(2) == 6);
    assert(ar.iloc(3) == 10);

    Array<int> ar2({ 1, 2, 3 });
    auto r = ar2.cumsum();
    assert(r.iloc(2) == 6);
    assert(ar2.iloc(2) == 3);  // original unchanged
    cout << "[PASS] test_array_cumsum" << endl;
}

void test_array_cumsum_with_nan()
{
    Array<double> ar({ 1.0, pandas::nan<double>(), 3.0 });
    ar._cumsum();
    assert(ar.iloc(0) == 1.0);
    assert(pandas::isnan(ar.iloc(1)));  // NaN stays NaN
    assert(ar.iloc(2) == 4.0);
    cout << "[PASS] test_array_cumsum_with_nan" << endl;
}

void test_array_pow()
{
    Array<double> ar({ 2.0, 3.0, 4.0 });
    ar._pow(2.0);
    assert(ar.iloc(0) == 4.0);
    assert(ar.iloc(1) == 9.0);
    assert(ar.iloc(2) == 16.0);

    auto r = ar.pow(0.5);  // sqrt
    assert(abs(r.iloc(0) - 2.0) < 1e-10);
    assert(abs(r.iloc(1) - 3.0) < 1e-10);
    cout << "[PASS] test_array_pow" << endl;
}

void test_array_reciprocal()
{
    Array<double> ar({ 2.0, 4.0, 8.0 });
    ar._reciprocal();
    assert(ar.iloc(0) == 0.5);
    assert(ar.iloc(1) == 0.25);
    assert(ar.iloc(2) == 0.125);

    auto r = ar.reciprocal();
    assert(r.iloc(0) == 2.0);
    cout << "[PASS] test_array_reciprocal" << endl;
}

void test_array_abs()
{
    Array<int> ar({ -3, -1, 0, 1, 2 });
    ar._abs();
    assert(ar.iloc(0) == 3);
    assert(ar.iloc(1) == 1);
    assert(ar.iloc(2) == 0);

    auto r = ar.abs();
    assert(r.iloc(0) == 3);
    cout << "[PASS] test_array_abs" << endl;
}

void test_array_replace()
{
    Array<int> ar({ 1, 2, 3, 2, 1 });
    ar._replace(2, 99);
    assert(ar.iloc(0) == 1);
    assert(ar.iloc(1) == 99);
    assert(ar.iloc(2) == 3);
    assert(ar.iloc(3) == 99);
    assert(ar.iloc(4) == 1);

    auto r = ar.replace(99, 100);
    assert(r.iloc(1) == 100);
    assert(ar.iloc(1) == 99);
    cout << "[PASS] test_array_replace" << endl;
}

// ========== Array Loc/Where Tests ==========
void test_array_loc_vector()
{
    Array<int> ar({ 10, 20, 30, 40, 50 });
    vector<char> mask = { true, false, true, false, true };
    auto r = ar.loc(mask);
    assert(r.size() == 3);
    assert(r.iloc(0) == 10);
    assert(r.iloc(1) == 30);
    assert(r.iloc(2) == 50);
    cout << "[PASS] test_array_loc_vector" << endl;
}

void test_array_loc_array()
{
    Array<int> ar({ 10, 20, 30, 40, 50 });
    Array<char> mask({ true, false, true, false, true });
    auto r = ar.loc(mask);
    assert(r.size() == 3);
    assert(r.iloc(1) == 30);
    cout << "[PASS] test_array_loc_array" << endl;
}

void test_array_where()
{
    Array<int> ar({ 1, 2, 3, 4, 5 });
    Array<char> mask({ true, false, true, false, true });
    auto r = ar.where(mask, 0);
    assert(r.iloc(0) == 1);
    assert(r.iloc(1) == 0);
    assert(r.iloc(2) == 3);
    assert(r.iloc(4) == 5);

    // where with Array mask (vector doesn't work, Array does)
    auto mask_arr = Array<char>({ true, true, false, false, true });
    auto r2 = ar.where(mask_arr, -1);
    assert(r2.iloc(0) == 1);
    assert(r2.iloc(2) == -1);
    assert(r2.iloc(4) == 5);
    cout << "[PASS] test_array_where" << endl;
}

void test_array_duplicated()
{
    Array<int> ar({ 1, 2, 2, 3, 3, 3 });

    auto dup_first = ar.duplicated("first");
    assert(dup_first.iloc(0) == false);  // 1 unique
    assert(dup_first.iloc(1) == false);  // first 2
    assert(dup_first.iloc(2) == true);   // second 2
    assert(dup_first.iloc(3) == false);  // first 3
    assert(dup_first.iloc(4) == true);   // second 3
    assert(dup_first.iloc(5) == true);   // third 3

    auto dup_last = ar.duplicated("last");
    assert(dup_last.iloc(0) == false);
    assert(dup_last.iloc(1) == true);    // first 2 (because last 2 isn't dup'd)
    assert(dup_last.iloc(2) == false);   // last 2
    assert(dup_last.iloc(3) == true);
    assert(dup_last.iloc(4) == true);
    assert(dup_last.iloc(5) == false);   // last 3

    auto dup_false = ar.duplicated("false");
    assert(dup_false.iloc(0) == false);   // 1 appears once, not dup
    assert(dup_false.iloc(1) == true);    // 2 appears >1
    assert(dup_false.iloc(2) == true);
    assert(dup_false.iloc(3) == true);    // 3 appears >1
    assert(dup_false.iloc(4) == true);
    assert(dup_false.iloc(5) == true);
    cout << "[PASS] test_array_duplicated" << endl;
}

void test_array_drop_duplicates()
{
    Array<int> ar({ 1, 2, 2, 3, 3, 3 });
    auto r = ar.drop_duplicates("first");
    assert(r.size() == 3);
    assert(r.iloc(0) == 1);
    assert(r.iloc(1) == 2);
    assert(r.iloc(2) == 3);
    cout << "[PASS] test_array_drop_duplicates" << endl;
}

// ========== Array Stats Tests ==========
void test_array_sum()
{
    Array<int> ar({ 1, 2, 3, 4, 5 });
    assert(ar.sum() == 15);
    cout << "[PASS] test_array_sum" << endl;
}

void test_array_sum_with_nan()
{
    Array<double> ar({ 1.0, pandas::nan<double>(), 3.0 });
    assert(ar.sum() == 4.0);
    cout << "[PASS] test_array_sum_with_nan" << endl;
}

void test_array_max()
{
    Array<int> ar({ 3, 1, 4, 1, 5, 9, 2 });
    assert(ar.max() == 9);

    Array<int> empty_ar;
    assert(pandas::isnan(empty_ar.max()));
    cout << "[PASS] test_array_max" << endl;
}

void test_array_min()
{
    Array<int> ar({ 3, 1, 4, 1, 5, 9, 2 });
    assert(ar.min() == 1);

    Array<int> empty_ar;
    assert(pandas::isnan(empty_ar.min()));
    cout << "[PASS] test_array_min" << endl;
}

void test_array_count()
{
    Array<double> ar({ 1.0, pandas::nan<double>(), 3.0, 4.0, pandas::nan<double>() });
    assert(ar.count() == 3);
    cout << "[PASS] test_array_count" << endl;
}

void test_array_mean()
{
    Array<double> ar({ 2.0, 4.0, 6.0 });
    assert(ar.mean() == 4.0);
    cout << "[PASS] test_array_mean" << endl;
}

void test_array_var_std()
{
    Array<double> ar({ 2.0, 4.0, 6.0 });
    double v = ar.var(1);  // ddof=1
    double expected_var = 4.0;  // ((2-4)^2 + (4-4)^2 + (6-4)^2) / (3-1) = 8/2 = 4
    assert(abs(v - expected_var) < 1e-10);

    double s = ar.std(1);
    assert(abs(s - 2.0) < 1e-10);
    cout << "[PASS] test_array_var_std" << endl;
}

void test_array_median_even()
{
    Array<int> ar({ 1, 3, 5, 7 });
    double m = ar.median();
    assert(abs(m - 4.0) < 1e-10);
    cout << "[PASS] test_array_median_even" << endl;
}

void test_array_median_odd()
{
    Array<int> ar({ 1, 3, 5, 7, 9 });
    double m = ar.median();
    assert(abs(m - 5.0) < 1e-10);
    cout << "[PASS] test_array_median_odd" << endl;
}

void test_array_median_empty()
{
    Array<double> ar;
    assert(pandas::isnan(ar.median()));
    cout << "[PASS] test_array_median_empty" << endl;
}

void test_array_corr()
{
    Array<double> x({ 1.0, 2.0, 3.0, 4.0, 5.0 });
    Array<double> y({ 2.0, 4.0, 6.0, 8.0, 10.0 });
    double c = x.corr(y);
    assert(abs(c - 1.0) < 1e-10);

    Array<double> z({ 5.0, 4.0, 3.0, 2.0, 1.0 });
    double cz = x.corr(z);
    assert(abs(cz + 1.0) < 1e-10);
    cout << "[PASS] test_array_corr" << endl;
}

void test_array_corr_with_nan()
{
    Array<double> x({ 1.0, pandas::nan<double>(), 3.0, 4.0, 5.0 });
    Array<double> y({ 2.0, 4.0, pandas::nan<double>(), 8.0, 10.0 });
    double c = x.corr(y);  // should compute over non-NaN pairs
    assert(!pandas::isnan(c));
    cout << "[PASS] test_array_corr_with_nan" << endl;
}

void test_array_astype()
{
    Array<int> ar({ 1, 2, 3 }, "int_arr");
    auto ar_d = ar.astype<double, string>();
    assert(ar_d.size() == 3);
    assert(ar_d.get_name() == "int_arr");
    assert(ar_d.iloc(0) == 1.0);
    cout << "[PASS] test_array_astype" << endl;
}

// ========== Edge Case Tests ==========
void test_array_empty_sort()
{
    Array<int> ar;
    auto s = ar.sort();
    assert(s.size() == 0);
    cout << "[PASS] test_array_empty_sort" << endl;
}

void test_array_empty_cumsum()
{
    Array<int> ar;
    ar._cumsum();
    assert(ar.size() == 0);
    cout << "[PASS] test_array_empty_cumsum" << endl;
}

void test_array_single_element()
{
    Array<int> ar({ 42 });
    assert(ar.max() == 42);
    assert(ar.min() == 42);
    assert(ar.sum() == 42);
    assert(abs(ar.median() - 42.0) < 1e-10);
    cout << "[PASS] test_array_single_element" << endl;
}

void test_array_string()
{
    Array<string> ar({ "hello", "world", "!" }, "str_arr");
    assert(ar.size() == 3);
    assert(ar.get_name() == "str_arr");
    assert(ar.iloc(0) == "hello");
    assert(ar.iloc(2) == "!");

    // operator==
    auto cmp = ar == string("hello");
    assert(cmp.iloc(0) == true);
    assert(cmp.iloc(1) == false);
    cout << "[PASS] test_array_string" << endl;
}

void test_array_to_string()
{
    Array<int> ar({ 1, 2, 3 }, "test");
    string s = ar.to_string();
    assert(s.find("test") != string::npos);
    assert(s.find("1") != string::npos);
    assert(s.find("3") != string::npos);
    assert(s.find("[3 rows]") != string::npos);
    cout << "[PASS] test_array_to_string" << endl;
}

// ========== Index Tests ==========
void test_index_default_construct()
{
    Index<int> idx;
    assert(idx.size() == 0);
    cout << "[PASS] test_index_default_construct" << endl;
}

void test_index_array_construct()
{
    Array<int> ar({ 3, 1, 2 }, "idx");
    Index<int> idx(ar);
    assert(idx.size() == 3);
    assert(idx.get_name() == "idx");
    // sorted: value2iid should be sorted by value
    assert(idx.iloc(0) == 3);
    assert(idx.iloc(1) == 1);
    assert(idx.iloc(2) == 2);
    cout << "[PASS] test_index_array_construct" << endl;
}

void test_index_append_flush()
{
    Index<int> idx;
    idx._append(3, true);
    idx._append(1, false);
    idx._append(2, false);
    assert(idx.size() == 3);

    // With flush_index=true on each append, it may re-sort
    // Just test that values are stored
    idx._flush_index();
    assert(idx.has(1));
    assert(idx.has(2));
    assert(idx.has(3));
    cout << "[PASS] test_index_append_flush" << endl;
}

void test_index_lower_upper_bound()
{
    Index<int> idx(Array<int>({ 1, 2, 2, 2, 5 }));
    assert(idx.lower_bound(2) >= 0);
    assert(idx.upper_bound(2) >= 0);
    assert(idx.upper_bound(2) - idx.lower_bound(2) == 3);  // count of 2's

    // value not present
    assert(idx.lower_bound(3) >= 0);
    assert(idx.lower_bound(3) <= idx.upper_bound(3));
    assert(idx.lower_bound(10) == -1);
    assert(idx.lower_bound(0) == -1);
    cout << "[PASS] test_index_lower_upper_bound" << endl;
}

void test_index_has()
{
    Index<int> idx(Array<int>({ 1, 2, 3 }));
    assert(idx.has(2) == true);
    assert(idx.has(4) == false);
    cout << "[PASS] test_index_has" << endl;
}

void test_index_count()
{
    Index<int> idx(Array<int>({ 1, 2, 2, 2, 3 }));
    assert(idx.count(2) == 3);
    assert(idx.count(1) == 1);
    assert(idx.count(4) == 0);
    cout << "[PASS] test_index_count" << endl;
}

void test_index_loc_i()
{
    Index<int> idx(Array<int>({ 10, 20, 30 }));
    assert(idx.loc_i(10) >= 0);
    assert(idx.loc_i(30) >= 0);

    bool threw = false;
    try {
        idx.loc_i(99);
    } catch (...) {
        threw = true;
    }
    assert(threw);
    cout << "[PASS] test_index_loc_i" << endl;
}

void test_index_loc()
{
    Index<int> idx(Array<int>({ 10, 20, 30 }));
    assert(idx.loc(20) == 20);
    cout << "[PASS] test_index_loc" << endl;
}

void test_index_duplicate_throws()
{
    Index<int> idx(Array<int>({ 1, 2, 2, 3 }));
    bool threw = false;
    try {
        idx.loc_i(2);
    } catch (...) {
        threw = true;
    }
    assert(threw);
    cout << "[PASS] test_index_duplicate_throws" << endl;
}

void test_index_drop_duplicates()
{
    Index<int> idx(Array<int>({ 1, 2, 2, 3, 3 }));
    auto dedup = idx.drop_duplicates("first");
    assert(dedup.size() == 3);
    cout << "[PASS] test_index_drop_duplicates" << endl;
}

void test_index_astype()
{
    Index<int> idx(Array<int>({ 1, 2, 3 }));
    auto idx_d = idx.astype<double, string>();
    assert(idx_d.size() == 3);
    assert(idx_d.iloc(0) == 1.0);
    cout << "[PASS] test_index_astype" << endl;
}

void test_index_sort()
{
    Index<int> idx(Array<int>({ 3, 1, 2 }));
    idx._sort(false);  // descending
    assert(idx.iloc(0) == 3);
    assert(idx.iloc(2) == 1);
    cout << "[PASS] test_index_sort" << endl;
}

void test_index_tuple_get_level_values()
{
    Index<std::tuple<int, int>> idx(
        Array<std::tuple<int, int>>({ std::tuple(1, 10), std::tuple(2, 20), std::tuple(3, 30) }, "tuple_idx"));
    auto lv0 = idx.get_level_values<0>();
    assert(lv0.size() == 3);
    assert(lv0.iloc(0) == 1);

    auto lv1 = idx.get_level_values<1>();
    assert(lv1.iloc(2) == 30);
    cout << "[PASS] test_index_tuple_get_level_values" << endl;
}

void test_index_tuple_droplevel()
{
    Index<std::tuple<int, int>> idx(
        Array<std::tuple<int, int>>({ std::tuple(1, 10), std::tuple(2, 20) }, "tuple_idx"));
    auto dropped = idx.droplevel<0>();
    assert(dropped.size() == 2);
    assert(dropped.iloc(0) == 10);
    assert(dropped.iloc(1) == 20);

    auto dropped2 = idx.droplevel<1>();
    assert(dropped2.iloc(0) == 1);
    cout << "[PASS] test_index_tuple_droplevel" << endl;
}

void test_index_range()
{
    Index<int> idx(Array<int>({ 1, 2, 3, 4, 5 }));
    auto rng = idx.range(2, 4);
    assert(rng.has_left());
    cout << "[PASS] test_index_range" << endl;
}

void test_index_duplicated()
{
    Index<int> idx(Array<int>({ 1, 2, 2, 3 }));
    auto dup = idx.duplicated("first");
    assert(dup.size() == 4);
    assert(dup.iloc(0) == false);
    assert(dup.iloc(1) == false);
    assert(dup.iloc(2) == true);
    cout << "[PASS] test_index_duplicated" << endl;
}

int main()
{
    try {
        // Array Constructor Tests
        test_array_default_construct();
        test_array_name_construct();
        test_array_size_init_construct();
        test_array_vector_construct();
        test_array_vector_rvalue_construct();
        test_array_initlist_construct();
        test_array_copy_construct();
        test_array_move_construct();

        // Array Assignment Tests
        test_array_copy_assign();
        test_array_move_assign();
        test_array_cross_type_assign();

        // Array Modifier Tests
        test_array_append();
        test_array_append_array();
        test_array_clear();
        test_array_rename();
        test_array_iloc_ref();
        test_array_sort_ascending();
        test_array_sort_descending();
        test_array_sort_inplace();
        test_array_reverse();

        // Array Operator Tests
        test_array_arith_scalar();
        test_array_arith_array();
        test_array_cmp_scalar();
        test_array_cmp_array();
        test_array_compound_assign();
        test_array_logical_ops();

        // Array Functional Tests
        test_array_map();
        test_array_diff();
        test_array_shift();
        test_array_fillna();
        test_array_ffill();
        test_array_bfill();
        test_array_cumsum();
        test_array_cumsum_with_nan();
        test_array_pow();
        test_array_reciprocal();
        test_array_abs();
        test_array_replace();

        // Array Loc/Where Tests
        test_array_loc_vector();
        test_array_loc_array();
        test_array_where();
        test_array_duplicated();
        test_array_drop_duplicates();

        // Array Stats Tests
        test_array_sum();
        test_array_sum_with_nan();
        test_array_max();
        test_array_min();
        test_array_count();
        test_array_mean();
        test_array_var_std();
        test_array_median_even();
        test_array_median_odd();
        test_array_median_empty();
        test_array_corr();
        test_array_corr_with_nan();
        test_array_astype();

        // Edge Case Tests
        test_array_empty_sort();
        test_array_empty_cumsum();
        test_array_single_element();
        test_array_string();
        test_array_to_string();

        // Index Tests
        test_index_default_construct();
        test_index_array_construct();
        test_index_append_flush();
        test_index_lower_upper_bound();
        test_index_has();
        test_index_count();
        test_index_loc_i();
        test_index_loc();
        test_index_duplicate_throws();
        test_index_drop_duplicates();
        test_index_astype();
        test_index_sort();
        test_index_tuple_get_level_values();
        test_index_tuple_droplevel();
        test_index_range();
        test_index_duplicated();

        cout << "\nAll Array comprehensive tests passed!" << endl;

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
