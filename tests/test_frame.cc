#include "pandas/array.h"
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

using namespace std;
using namespace pandas;
using namespace std::chrono;

void test_frame_constructors()
{
    Series<int, double> sr1({ 0, 1 }, { 1, 2 }, "sr1");
    Series<int, double> sr2({ 0, 2 }, { 4, 3 }, "sr2");
    DataFrame<int, double> df1(std::vector<Series<int, double>>({ sr1, sr2 }));

    assert(pandas::isnan(df1.iloc(1, 1)) && (df1.iloc(2, 1) == 3));

    cout << "[PASS] test_frame_constructors" << endl;
}

void test_frame_operator()
{
    DataFrame<int, double> df1(std::vector<int>({ 0, 1 }), { "a", "b" });
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

    Series<std::string, int> sr1({ "b", "a" }, { 1, 2 });
    auto df5 = df4.operator+ <1>(sr1);
    assert((df5.iloc(2, 1) == 9) && (df5.iloc(0, 0) == 7));

    cout << df4 << endl;
    cout << df5 << endl;

    auto df6 = (df5 - 6).operator><1>(sr1);
    cout << df6 << endl;

    cout << "[PASS] test_frame_operator" << endl;
}

void test_frame_loc()
{

    cout << "[PASS] test_frame_loc" << endl;
}

void test_frame_functional()
{

    cout << "[PASS] test_frame_functional" << endl;
}

int main()
{

    try {
        test_frame_constructors();
        test_frame_operator();
        test_frame_loc();
        test_frame_functional();

    } catch (const std::string& s) {
        cout << "ERROR: " << s << endl;

    } catch (...) {
        cout << "ERROR: unknown" << endl;
    }

    return 0;
}