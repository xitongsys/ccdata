#include "pandas/array.h"
#include "pandas/concat.h"
#include "pandas/csv/csv.h"
#include "pandas/dataframe.h"
#include "pandas/datetime.h"
#include "pandas/index.h"
#include "pandas/pandastype.h"
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

    cout << "test_concat_array PASSED" << endl;
}

void test_concat_index()
{
    Index<int> id1(Array<int>({ 1, 2, 3 }, "id1"));
    Index<int> id2(Array<int>({ 2, 3, 4 }, "id2"));

    auto id_0 = concat<0>(id1, id2);
    assert((id_0.size() == 4) && (id_0.iloc(3) == 4));

    auto id_1 = concat<1>(id1, id2);
    assert((id_1.size() == 3) && (id_1.iloc(0) == std::tuple<int, int>(1, 2)));

    cout << "test_concat_index PASSED" << endl;
}

void test_concat_series()
{
    Series<int, double> sr1({ 1, 2 }, { 1, 1 }, "sr1");
    Series<int, double> sr2({ 3, 4 }, { 2, 2 }, "sr2");

    auto sr_0 = concat<0>(sr1, sr2);
    assert((sr_0.size() == 4) && (sr_0.iloc(3) == 2));

    auto df = concat<1>(sr1, sr2);
    assert((df.size<0>() == 4) && (df.size<1>() == 2) && pandas::isnan(df.iloc(0, 1)) && (df.iloc(3, 1) == 2));

    cout << "test_concat_series PASSED" << endl;
}

int main()
{
    try {
        test_concat_array();
        test_concat_index();
        test_concat_series();

    } catch (const std::string& s) {
        cout << "ERROR: " << s << endl;

    } catch (...) {
        cout << "ERROR: unknown" << endl;
    }

    return 0;
}