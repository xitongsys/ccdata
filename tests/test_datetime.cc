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

void test_datetime()
{
    Datetime dt(2025, 1, 2, 3, 4, 5);
    assert(dt.to_string() == "2025-01-02 03:04:05.000000000");

    Datetime dt3;
    dt3.strptime("20250102030405", "%Y%m%d%H%M%S");

    assert(dt3.to_string() == "2025-01-02 03:04:05.000000000");

    assert(dt3.date() == Datetime(2025, 1, 2));

    Series<Datetime, double> y("y");
    y._append(Datetime(2025, 1, 1, 9), 1);
    y._append(Datetime(2025, 1, 1, 10), 3);
    y._append(Datetime(2025, 1, 2, 9), -1);
    y._append(Datetime(2025, 1, 2, 10), 2);

    auto dg = y.groupby(y.pidx->values.map<Datetime>([](const Datetime& dt) -> Datetime { return dt.date(); })).sum();
    assert((dg.iloc(0) == 4) && (dg.iloc(1) == 1));

    auto td = Datetime(2025, 1, 1, 0) - Datetime(2025, 1, 1, 1);
    assert(td == TimeDelta(0, -1, 0, 0, 0));

    cout << "[PASS] test_datetime" << endl;
}

int main()
{

    try {
        test_datetime();

    } catch (const std::string& s) {
        cout << "ERROR: " << s << endl;
    }

    return 0;
}