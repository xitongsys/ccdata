#include "pandas/array.h"
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
    SingleIndex<int> si1;
    Series<int, double> sr1(si1);
    for (int i = 0; i < 300000; i++) {

        sr1.append(i + 10, 10.5);
    }

    cout << sr1 << endl;

    vector<int> vs(300000, 100);

    auto start = high_resolution_clock::now();

    double s = 0;
    for (int i = 0; i < sr1.size(); i++) {
        for (int j = max(0, i - 10000); j <= i; j++) {
            s += sr1.iloc(j);
            // s += sr1.values.values[j];
            //  s += vs[j];
        }
    }
    cout << s << endl;
    auto end = high_resolution_clock::now();

    duration<double, nano> dt = end - start;
    cout << double(dt.count()) / 1e6 << endl;

    auto start2 = high_resolution_clock::now();
    cout << sr1.rolling(10000, 1).sum() << endl;
    auto end2 = high_resolution_clock::now();

    duration<double, nano> dt2 = end2 - start2;
    cout << double(dt2.count()) / 1e6 << endl;
}

int main()
{

    try {
        test2();

    } catch (const std::string& s) {
        cout << "ERROR: " << s << endl;

    } catch (...) {
        cout << "ERROR: unknown" << endl;
    }

    return 0;
}