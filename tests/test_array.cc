#include "pandas/array.h"
#include "pandas/csv/csv.h"
#include "pandas/dataframe.h"
#include "pandas/datetime.h"
#include "pandas/index.h"
#include "pandas/pandastype.h"
#include "pandas/series.h"

#include <chrono>
#include <iostream>
#include <map>
#include <cassert>

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
    assert(ar1.iloc(0)==3);

    ar1._clear();
    assert(ar1.size() == 0);

}

int main()
{

    try {
        test_array01();

    } catch (const std::string& s) {
        cout << "ERROR: " << s << endl;

    } catch (...) {
        cout << "ERROR: unknown" << endl;
    }

    return 0;
}