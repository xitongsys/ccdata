#include "pandas/array.h"
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

void test_series_constructors()
{
    Series<int, int> sr1("sr1");
    assert(sr1.get_name() == "sr1");

    Index<int> idx1;
    idx1._append(0);
    Series<int, double> sr2(idx1);
    sr2.pidx->iloc_ref(0) = 1;
    assert(idx1.iloc(0) == 0);
    assert(sr2.pidx->iloc(0) == 1);

    std::shared_ptr<Index<int>> pidx = std::make_shared<Index<int>>();
    pidx->_append(0);
    pidx->_append(1);
    Series<int, double> sr3(pidx);
    sr3.pidx->_append(2);
    assert(pidx->size() == 3);
    assert(sr3.pidx->size() == 3);

    std::vector<int> ids({ 1, 2 });
    std::vector<int> vals({ 2, 3 });
    Series<int, int> sr4(ids, vals, "sr4");
    assert(sr4.size() == 2);

    Index<int> ids2(Array<int>({ 1, 2, 3 }));
    Array<int> vals2({ 2, 3, 4 }, "sr5");
    Series<int, int> sr5(ids2, vals2);
    assert(sr5.size() == 3);
}

void test_series_operator()
{
    Series<int, double> sr({ 0, 1, 2 }, { 1, 2, 3 }, "sr1");
    sr += 1;
    assert(sr.sum() == 9);

    sr = sr + 2;
    assert(sr.sum() == 15);

    sr = sr * 2;
    assert(sr.sum() == 30);

    sr = sr / 2;
    assert(sr.sum() == 15);

    auto sr2 = sr.astype<int, int>();
    sr2 = sr2 % 2;
    assert(sr2.sum() == 1);

    auto sr3 = sr + sr2;
    assert(sr3.sum() == 16);

    std::cout << sr3 << std::endl;
}

int main()
{

    try {
        test_series_constructors();
        test_series_operator();

    } catch (const std::string& s) {
        cout << "ERROR: " << s << endl;

    } catch (...) {
        cout << "ERROR: unknown" << endl;
    }

    return 0;
}