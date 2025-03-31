#include "pandas/array.h"
#include "pandas/concat.h"
#include "pandas/csv/csv.h"
#include "pandas/dataframe.h"
#include "pandas/datetime.h"
#include "pandas/index.h"
#include "pandas/ops.h"
#include "pandas/series.h"

#include <cassert>
#include <chrono>
#include <format>
#include <iostream>
#include <map>
#include <vector>

using namespace std;
using namespace pandas;
using namespace std::chrono;

void test_frame_constructors()
{
    Series<int, double> sr1("sr1", { 0, 1 }, { 1, 2 });
    Series<int, double> sr2("sr2", { 0, 2 }, { 4, 3 });
    DataFrame<int, double> df1(std::vector<Series<int, double>>({ sr1, sr2 }));
    assert(pandas::isnan(df1.iloc(1, 1)) && (df1.iloc(2, 1) == 3));

    auto df2 = df1.set_index<double, std::string>("index", std::vector<double>({ 10, 11, 12 }));
    assert(df2.pidx->iloc(0) == 10);

    Array<std::string> ar = std::vector<std::string>({ "10", "11", "12" });
    auto df3 = df1.set_index<std::string, std::string>(ar);
    assert(df3.pidx->iloc(0) == "10");

    cout << "[PASS] test_frame_constructors" << endl;
}

void test_frame_operator()
{
    DataFrame<int, double> df1(std::vector<int>({ 0, 1 }), { "a", "b" }, pandas::nan<double>());
    df1._fillna(1);
    df1._append_row(2, std::vector<double>({ 2, 3 }));

    assert(df1.iloc(2, 1) == 3);

    auto df2 = df1 + 1;
    assert((df2.iloc(2, 1) == 4) && (df2.iloc(0, 0) == 2));

    df2 += 3;
    assert((df2.iloc(2, 1) == 7) && (df2.iloc(0, 0) == 5));

    auto df3 = df2.operator+ <0>(std::vector<int>({ 1, 2, 3 }));
    assert((df3.iloc(2, 1) == 10) && (df3.iloc(0, 0) == 6));

    auto df4 = df3.operator- <1>(Array<int>({ 1, 2 }));
    assert((df4.iloc(2, 1) == 8) && (df4.iloc(0, 0) == 5));

    Series<std::string, int> sr1("sr1", { "b", "a" }, { 1, 2 });
    auto df5 = df4.operator+ <1>(sr1);
    assert((df5.iloc(2, 1) == 9) && (df5.iloc(0, 0) == 7));

    auto df6 = (df5 - 7).operator><1>(sr1);
    assert((df6.iloc(0, 0) == false) && (df6.iloc(2, 1) == true));

    cout << "[PASS] test_frame_operator" << endl;
}

void test_frame_loc()
{
    cout << "[PASS] test_frame_loc" << endl;
}

void test_frame_functional()
{

    Index<int> id1(std::vector<int>({ 1, 2, 3 }), "id1");
    Index<int> id2(std::vector<int>({ 1, 4, 5 }), "id2");

    auto id = concat<1>(id1, id2);
    DataFrame<std::tuple<int, int>, double> df(id.values.values, std::vector<std::string>({ "a", "b" }), pandas::nan<double>());
    df._fillna(1);

    auto df1 = df.droplevel<0>();
    assert((df1.pidx->iloc(1) == 4) && (df1.iloc(0, 0) == 1));

    cout << "[PASS] test_frame_functional" << endl;
}

void test_frame_groupby()
{
    DataFrame<int, double> df1(std::vector<int>(), { "a", "b" }, pandas::nan<double>());
    df1._append_row(1, std::vector<double>({ 1, 1 }));
    df1._append_row(2, std::vector<double>({ 2, 2 }));
    df1._append_row(3, std::vector<double>({ 3, 3 }));

    auto dg = df1.groupby(std::vector<std::string>({ "t1", "t2", "t2" })).sum();
    assert((dg.iloc(0, 0) == 1) && (dg.iloc(1, 1) == 5));

    df1._append_row(4, std::vector<double>({ pandas::nan<double>(), 1 }));
    auto dg2 = df1.groupby(df1.pidx->values).count();
    assert(dg2.iloc(3, 0) == 0);

    Array<int> ar1({ 1, 1, 2, 2 });
    Array<std::string> ar2({ "a", "a", "b", "c" });
    auto ar3 = concat<1>(ar1, ar2);
    auto dg3 = df1.groupby(ar3).sum();
    assert((dg3.iloc(0, 0) == 3) && (dg3.iloc(2, 0) == 0));

    cout << "[PASS] test_frame_groupby" << endl;
}

void test_frame_sort()
{
    DataFrame<int, double> df1(std::vector<int>(), { "a", "b" }, pandas::nan<double>());
    df1._append_row(1, std::vector<double>({ 3, 1 }));
    df1._append_row(2, std::vector<double>({ 2, 2 }));
    df1._append_row(3, std::vector<double>({ 1, 3 }));

    auto df2 = df1.sort_values("b", false);
    assert((df2.iloc(0,0)==1) && (df2.iloc(0,1)==3));

    cout << "[PASS] test_frame_sort done" << endl;
}

int main()
{

    try {
        test_frame_constructors();
        test_frame_operator();
        test_frame_loc();
        test_frame_functional();
        test_frame_groupby();
        test_frame_sort();

    } catch (const std::string& s) {
        cout << "ERROR: " << s << endl;

    } catch (...) {
        cout << "ERROR: unknown" << endl;
    }

    return 0;
}