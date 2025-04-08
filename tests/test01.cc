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

#define CHECK                                                                                  \
    {                                                                                          \
        std::cout << __LINE__ << ": " << (Datetime::now() - bgn).total_seconds() << std::endl; \
        bgn = Datetime::now();                                                                 \
    }

void test01()
{
    Datetime bgn = Datetime::now();

    using IT = Datetime;

    int N = 260000;
    Array<IT> ar_idx;
    Array<double> ar_val;
    Array<double> ar_key;

    for (int i = 0; i < N; i++) {
        ar_idx._append(i);
        ar_val._append(i);
        ar_key._append(i % 6);
    }

    CHECK

    Index idx(std::move(ar_idx));

    CHECK

    std::sort(idx.value2iid.begin(), idx.value2iid.end());

    CHECK

    Series<IT, double> sr(std::move(ar_idx), std::move(ar_val));

    CHECK

    using ST = Series<IT, double>;
    using SV = ST::SeriesVisitor<RangeVec<int>>;
    auto a = sr.groupby(ar_key).apply<IT, double, std::string, std::string>([](SV& sv) -> ST {
        Datetime bgn = Datetime::now();
        auto sr = sv.to_series();
        CHECK
        auto ds = sr.iloc(0, 5, 1).to_series();
        CHECK
        return ds;
    });

    CHECK

    cout << a << endl;
}

int main()
{

    try {
        test01();

    } catch (const std::string& s) {
        cout << "ERROR: " << s << endl;

    } catch (...) {
        cout << "ERROR: unknown" << endl;
    }

    return 0;
}