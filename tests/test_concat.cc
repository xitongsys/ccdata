#include "pandas/array.h"
#include "pandas/concat.h"
#include "pandas/csv/csv.h"
#include "pandas/dataframe.h"
#include "pandas/datetime.h"
#include "pandas/index.h"
#include "pandas/series.h"

#include <cassert>
#include <chrono>
#include <iostream>
#include <map>

using namespace std;
using namespace pandas;
using namespace std::chrono;

void test_concat_array()
{
    Array<int> id1({ 1, 2, 3 }, "id1");
    Array<int> id2({ 2, 3, 4 }, "id2");

    auto id_0 = concat<0>(id1, id2);
    assert((id_0.size() == 6) && (id_0.iloc(3) == 2));

    auto id_1 = concat<1>(id1, id2);
    assert((id_1.size() == 3) && (id_1.iloc(0) == std::tuple<int, int>(1, 2)));

    cout << "[PASS] test_concat_array" << endl;
}

void test_concat_index()
{
    Index<int> id1(Array<int>({ 1, 2, 3 }, "id1"));
    Index<int> id2(Array<int>({ 2, 3, 4 }, "id2"));

    auto id_0 = concat<0>(id1, id2);
    assert((id_0.size() == 4) && (id_0.iloc(3) == 4));

    auto id_1 = concat<1>(id1, id2);
    assert((id_1.size() == 3) && (id_1.iloc(0) == std::tuple<int, int>(1, 2)));

    auto ar1 = id_1.get_level_values<0>();
    assert((ar1.iloc(0) == 1) && (ar1.iloc(2) == 3));

    auto id_3 = concat<1>(id1, id2, id2);
    assert((id_3.iloc(0) == std::tuple<int, int, int>(1, 2, 2)));

    auto id_4 = id_3.droplevel<0>();
    assert((id_4.iloc(0) == std::tuple<int, int>(2, 2)));

    auto id_5 = id_1.droplevel<1>();
    assert(id_5.iloc(0) == 1);

    cout << "[PASS] test_concat_index" << endl;
}

void test_concat_series()
{
    Series<int, double> sr1("sr1", { 1, 2 }, { 1, 1 });
    Series<int, double> sr2("sr2", { 3, 4 }, { 2, 2 });
    auto sr_0 = concat<0>(sr1, sr2);
    assert((sr_0.size() == 4) && (sr_0.iloc(3) == 2));

    auto df = concat<1>(sr1, sr2);
    assert((df.size<0>() == 4) && (df.size<1>() == 2) && pandas::isnan(df.iloc(0, 1)) && (df.iloc(3, 1) == 2));

    cout << "[PASS] test_concat_series" << endl;
}

void test_concat_dataframe()
{
    DataFrame<int, double> df1(std::vector<int>({ 0, 1 }), std::vector<std::string>({ "a", "b" }), pandas::nan<double>());
    df1._fillna(1);
    DataFrame<int, double> df2(std::vector<int>({ 2, 3 }), std::vector<std::string>({ "a", "b", "c" }), pandas::nan<double>());
    df2._fillna(2);

    auto df_0 = concat<0>(df1, df2);
    assert((df_0.size<0>() == 4) && (pandas::isnan(df_0.iloc(0, 2))) && (df_0.iloc(3, 2) == 2));

    DataFrame<int, double> df3(std::vector<int>({ 0, 1, 2 }), std::vector<std::string>({ "a", "b" }), pandas::nan<double>());
    df3._fillna(1);
    DataFrame<int, double> df4(std::vector<int>({ 2, 3 }), std::vector<std::string>({ "c", "d" }), pandas::nan<double>());
    df4._fillna(2);

    auto df_1 = concat<1>(df3, df4);
    assert((df_1.size<0>() == 4) && (pandas::isnan(df_1.iloc(0, 2))) && (pandas::isnan(df_1.iloc(3, 0))));

    cout << "[PASS] test_concat_dataframe" << endl;
}

int main()
{
    try {
        test_concat_array();
        test_concat_index();
        test_concat_series();
        test_concat_dataframe();

    } catch (const std::string& s) {
        cout << "ERROR: " << s << endl;

    } catch (...) {
        cout << "ERROR: unknown" << endl;
    }

    return 0;
}