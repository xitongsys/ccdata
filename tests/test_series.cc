#include "pandas/array.h"
#include "pandas/concat.h"
#include "pandas/csv/csv.h"
#include "pandas/dataframe.h"
#include "pandas/datetime.h"
#include "pandas/index.h"
#include "pandas/range.h"
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
    Series<int, double> sr2("sr2", idx1, pandas::nan<double>());
    sr2.pidx->iloc_ref(0) = 1;
    assert(idx1.iloc(0) == 0);
    assert(sr2.pidx->iloc(0) == 1);

    std::shared_ptr<Index<int>> pidx = std::make_shared<Index<int>>();
    pidx->_append(0);
    pidx->_append(1);
    Series<int, double> sr3("sr3", pidx, pandas::nan<double>());
    sr3.pidx->_append(2);
    assert(pidx->size() == 3);
    assert(sr3.pidx->size() == 3);

    std::vector<int> ids({ 1, 2 });
    std::vector<int> vals({ 2, 3 });
    Series<int, int> sr4(Index<int>(Array<int>(ids)), Array<int>(vals, "sr4"));
    assert(sr4.size() == 2);

    Index<int> ids2(Array<int>({ 1, 2, 3 }));
    Array<int> vals2({ 2, 3, 4 }, "sr5");
    Series<int, int> sr5(ids2, vals2);
    assert(sr5.size() == 3);

    cout << "[PASS] test_series_constructors" << endl;
}

void test_series_groupby()
{
    Series<int, int> sr1(Index<int>(Range<int>(10, 0, -1)), Array<int>(Range<int>(0, 10).to_vec(), "sr1"));
    Array<int> key1(Range<int>(0, 10).to_vec());
    key1 %= 2;
    auto dg1 = sr1.groupby(key1).sum();
    assert((dg1.iloc(0) == 20) && (dg1.iloc(1) == 25));

    using SV = Series<int, int>::SeriesVisitor<RangeVec<int>>;

    auto dg2 = sr1.groupby(key1).apply<int, int, std::string, std::string>([](SV& sv) -> Series<int, int> {
        auto ds = sv.to_series().sort_values().iloc(std::vector<int>({ 0, 1 })).to_series();
        return ds;
    });
    assert((dg2.iloc(0) == 0) && (dg2.iloc(2) == 1));

    cout << "[PASS] test_sereis_groupby" << endl;
}

void test_series_operator()
{
    Series<int, double> sr(Index<int>(Array<int>({ 0, 1, 2 })), Array<double>({ 1, 2, 3 }, "sr"));
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

    auto sr4 = sr > sr2;
    assert((sr4.astype<int, int>().sum()) == 3);

    auto sr5 = sr < sr2;
    assert(sr5.sum() == 0);

    Series<int, double> sr6(Index<int>(Array<int>({ 1, 2 })), Array<double>({ 3, 4 }, "sr6"));
    sr6 += 1;
    assert((sr6.iloc(0) == 4) && (sr6.iloc(1) == 5));

    auto sr7 = sr6 + vector<int>({ 1, 2 });
    assert((sr7.iloc(0) == 5) && (sr7.iloc(1) == 7));

    cout << "[PASS] test_series_operator" << endl;
}

void test_series_loc()
{
    Series<int, double> sr(Index<int>(Array<int>({ 1, 2, 3 })), Array<double>({ 2, 3, 4 }, "sr"));
    sr.loc_ref(1) = 4;
    assert((sr.iloc(0) == 4) && (sr.loc(1) == 4) && (sr.loc(2) == 3));

    sr.loc(2, 3) = 5;
    assert((sr.loc(2) == 5) & (sr.loc(3) == 5));

    sr.iloc(1, 2) = 6;
    assert((sr.iloc(1) == 6) && (sr.iloc(2) == 5));

    sr.loc(std::vector<int>({ 1, 2 })) = 7;
    assert((sr.iloc(0) == 7) && (sr.iloc(1) == 7));

    sr.iloc({ 1, 2 }) = 8;
    assert((sr.iloc(1) == 8) && (sr.iloc(2) == 8));

    sr.loc_mask(sr > 7) = 9;
    assert((sr.iloc(1) == 9) && sr.iloc(2) == 9);

    sr.loc({ 1, 1 }) = 1;
    assert(sr.iloc(0) == 1);

    cout << "[PASS] test_series_loc" << endl;
}

void test_series_functional()
{
    Series<int, double> sr(Index<int>(Array<int>({ 1, 2, 3 })), Array<double>({ 2, 3, 4 }, "sr"));
    sr._map<double>([](const double& a) -> double { return a * a; });
    assert((sr.iloc(0) == 4) && (sr.iloc(1) == 9) & (sr.iloc(2) == 16));

    sr.iloc_ref(0) = pandas::nan<double>();
    auto sr2 = sr.dropna();
    assert(sr2.size() == 2);

    sr._dropna();
    assert(sr.size() == 2);

    auto sr3 = sr.diff();
    assert((pandas::isnan<double>(sr3.iloc(0))) && (sr3.iloc(1) == 7));

    auto sr4 = sr.diff(-1);
    assert((pandas::isnan<double>(sr4.iloc(1))) && (sr4.iloc(0) == -7));

    Index<int> id1(std::vector<int>({ 1, 2, 3 }), "id1");
    Index<int> id2(std::vector<int>({ 1, 4, 5 }), "id2");

    auto id = concat<1>(id1, id2);
    auto sr5 = Series<decltype(id.iloc(0)), int>(id, std::vector<int>({ 10, 11, 12 }));
    auto sr6 = sr5.droplevel<0>();
    assert((sr6.pidx->iloc(0) == 1) && (sr6.iloc(0) == 10));

    Series<int, double> sr7(Index<int>(Array<int>({ 1, 2, 3, 4 })), Array<double>({ 2, 3, 4, pandas::nan<double>() }, "sr7"));
    auto sr8 = sr7.where(sr7 > 3, 1);
    assert((sr8.iloc(3) == 1) && (sr8.iloc(2) == 4) && (sr8.iloc(0) == 1));

    cout << "[PASS] test_series_functional" << endl;
}

void test_series_sort()
{
    Series<int, double> sr1(Index<int>(Array<int>({ 1, 2, 3, 4, 5 })),
        Array<double>({ 4, 3, 2, 1, pandas::nan<double>() }, "sr1"));

    auto sr2 = sr1.sort_values();
    assert(sr2.iloc(0) == 1);

    auto sr3 = sr1.sort_values(std::vector<int>({ 4, 3, 1, 2, 0 }));
    assert(pandas::isnan(sr3.iloc(0)));

    auto sr4 = sr1.sort_values(sr1.fillna(100));
    assert(sr4.iloc(0) == 1);

    cout << "[PASS] test_series_sort" << endl;
}

void test_series_perf()
{
    Series<int, double> sr;
    Index<int> idx;
    std::vector<int> idx2;
    Array<int> ar;
    for (int i = 0; i < 300000; i++) {
        sr._append(i, i * 2, false);
        idx._append(i + 1, false);
        idx2.push_back(i + 10);
        ar._append(i);
    }
    sr.pidx->_flush_index();
    idx._flush_index();

    Datetime bgn, end;
    bgn = Datetime::now();
    auto sr2 = sr.reindex(idx2);
    // #ar.cumsum();
    end = Datetime::now();

    cout << sr2 << endl;
    cout << (end - bgn).total_seconds() << endl;
}

void fun1()
{
    Datetime bgn, end;

    int N = 300000;

    Array<int> ar;
    vector<int> vs;

    bgn = Datetime::now();
    for (int i = 0; i < N; i++) {
        ar._append(i);
    }
    end = Datetime::now();
    cout << ar << endl
         << (end - bgn).total_seconds() << endl;

    bgn = Datetime::now();
    Index<int> idx(std::move(ar));
    end = Datetime::now();
    cout << idx << endl
         << (end - bgn).total_seconds() << endl;

    cout << ar.values[100] << endl;
}

int main()
{

    try {
        test_series_constructors();
        test_series_operator();
        test_series_loc();
        test_series_functional();
        test_series_groupby();
        test_series_sort();
        test_series_perf();
        fun1();

    } catch (const std::string& s) {
        cout << "ERROR: " << s << endl;

    } catch (...) {
        cout << "ERROR: unknown" << endl;
    }

    return 0;
}