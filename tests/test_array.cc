#include "pandas/array.h"
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

void test_array01()
{
    Array<int> ar1("arr1");
    ar1._append(1.0);
    ar1._append(2);

    assert(ar1.iloc(0) == 1);
    assert(ar1.iloc(1) == 2);

    ar1._rename("arr2");
    assert(ar1.get_name() == "arr2");

    Array<int> ar2("arr2");
    ar2 = ar1;
    assert(ar2.iloc(0) == 1);

    ar1.iloc_ref(0) = 3;
    assert(ar1.iloc(0) == 3);

    ar1._clear();
    assert(ar1.size() == 0);

    Array<char> ar3("ar3");
    for (int i = 0; i < 10; i++) {
        ar3._append(i % 2 == 0);
    }
    ar3.iloc_ref(0) = false;
    assert(ar3.iloc(0) == false);

    cout << "[PASS] test_array01" << endl;
}

void test_array_perf()
{
    Array<double> x, y;
    for (int i = 0; i < 300000; i++) {
        x._append(i);
        y._append(i * i);
    }
    auto start = std::chrono::high_resolution_clock::now();
    double c = x.corr(y);
    auto end = std::chrono::high_resolution_clock::now();

    cout << c <<endl;
    std::chrono::duration<double> elapsed = end - start;
    std::cout << "Time taken: " << elapsed.count() << " seconds" << std::endl;
}

int main()
{

    try {
        test_array01();
        test_array_perf();

    } catch (const std::string& s) {
        cout << "ERROR: " << s << endl;

    } catch (...) {
        cout << "ERROR: unknown" << endl;
    }

    return 0;
}