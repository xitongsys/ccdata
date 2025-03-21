#include "pandas/array.h"
#include "pandas/dataframe.h"
#include "pandas/datetime.h"
#include "pandas/index.h"
#include "pandas/pandastype.h"
#include "pandas/series.h"

#include <chrono>
#include <iostream>
#include <map>

using namespace std;
using namespace pandas;
using namespace std::chrono;

void test2()
{

    Array<int> ar1({ 1, 2, 3 });
    Array<int> ar2({ 4, 5 });
    Array<int> ar3 = concat_0<int>(ar1, ar2);
    Array<double> ar4({ 0.1, 0.2, 0.3, 0.4, 0.5 });

    Array<std::tuple<int, int>> ar5 = zip(ar3, ar3);

    cout << ar5 << endl;

    Series<int, double> sr1;
    for (int i = 0; i < 300000; i++) {

        sr1._append(i, i);
    }

    cout << sr1 << endl;

    auto start = high_resolution_clock::now();

    for (int i = 0; i < sr1.size(); i++) {
        double s = 0;
        for (int j = max(0, i - 100); j <= i; j++) {
            s += sr1.iloc(j);
        }
        sr1.iloc_ref(i) = s;
    }
    auto end = high_resolution_clock::now();
    duration<double, nano> dt = end - start;
    cout << double(dt.count()) / 1e6 << endl;

    cout << "-----------------------------" << endl;

    auto start2 = high_resolution_clock::now();
    cout << sr1.rolling(10000, 1).std() << endl;
    auto end2 = high_resolution_clock::now();
    duration<double, nano> dt2 = end2 - start2;
    cout << double(dt2.count()) / 1e6 << endl;

    cout << "--------------------------------" << endl;

    auto start3 = high_resolution_clock::now();
    cout << sr1.groupby(sr1).sum() << endl;
    auto end3 = high_resolution_clock::now();
    duration<double, nano> dt3 = end3 - start3;
    cout << double(dt3.count()) / 1e6 << endl;

    cout << "-------------------" << endl;

    cout << sr1.loc(1) << endl;
}

void test1()
{
    Index<int> si("a");
    for (int i = 0; i < 10; i++) {
        si._append(i);
    }
    si.iloc_ref(1) = 10;
    cout << si << endl;
}

void test3()
{
    DataFrame<int, double> df({ "a", "b", "c" });
    for (int i = 0; i < 100; i++) {
        df._append_row(Array<double, int>({ 1, 2, 3 }, i));
    }
    cout << df << endl;

    cout << df.sum() << endl;

    auto df2 = df.rolling(5, 1).mean();

    cout << df2.iloc(1, 5, 1).to_frame() << endl;

    auto dg = df2.groupby(df2.iloc<1>(1)).sum();
    cout << dg << endl;

    cout << df.shift(2).rolling(3, 3).max().dropna() << endl;

    std::cout << df.median<1>() << std::endl;
}

int main()
{

    try {
        test3();

    } catch (const std::string& s) {
        cout << "ERROR: " << s << endl;

    } catch (...) {
        cout << "ERROR: unknown" << endl;
    }

    return 0;
}