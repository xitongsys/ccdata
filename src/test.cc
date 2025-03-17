#include "pandas/array.h"
#include "pandas/dataframe.h"
#include "pandas/datetime.h"
#include "pandas/index.h"
#include "pandas/multiindex.h"
#include "pandas/pandastype.h"
#include "pandas/series.h"
#include "pandas/singleindex.h"

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

    Array<std::tuple<int, int>> ar5 = concat_1(ar3, ar3);

    cout << ar5 << endl;

    SingleIndex<int> si1;
    Series<int, double> sr1(si1);
    for (int i = 0; i < 300000; i++) {

        sr1._append(i + 10, i);
    }

    cout << sr1 << endl;

    auto start = high_resolution_clock::now();

    double s = 0;
    for (int i = 0; i < sr1.size(); i++) {
        for (int j = max(0, i - 10000); j <= i; j++) {
            s += sr1.iloc(j);
        }
    }
    cout << s << endl;
    auto end = high_resolution_clock::now();
    duration<double, nano> dt = end - start;
    cout << double(dt.count()) / 1e6 << endl;

    auto start2 = high_resolution_clock::now();
    cout << sr1.rolling(1000, 1).sum() << endl;
    auto end2 = high_resolution_clock::now();
    duration<double, nano> dt2 = end2 - start2;
    cout << double(dt2.count()) / 1e6 << endl;

    auto start3 = high_resolution_clock::now();
    cout << sr1.groupby(sr1).sum() << endl;
    auto end3 = high_resolution_clock::now();
    duration<double, nano> dt3 = end3 - start3;
    cout << double(dt3.count()) / 1e6 << endl;
}

void test1()
{
    Series<int, double> ds1(SingleIndex<int>(Array<int> { 0, 1, 2 }), Array<int> { 1, 2, 3 }, "c1");
    Series<int, double> ds2(SingleIndex<int>(Array<int> { 0, 1, 2 }), Array<int> { 1, 2, 3 }, "c2");

    DataFrame<int, double> df = concat_1(ds1, ds2);

    cout << df << endl;
}

int main()
{

    try {
        test1();

    } catch (const std::string& s) {
        cout << "ERROR: " << s << endl;

    } catch (...) {
        cout << "ERROR: unknown" << endl;
    }

    return 0;
}