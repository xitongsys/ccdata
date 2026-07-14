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


void test01()
{
    auto TP = std::chrono::high_resolution_clock::now();
    auto PRETP=TP;
    #define COST PRETP=TP;TP=std::chrono::high_resolution_clock::now(); std::cout << __LINE__ << " COST: " << std::chrono::duration_cast<std::chrono::microseconds>(TP-PRETP).count()/1000.0 << "ms" << std::endl;


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

    COST

    Index idx(std::move(ar_idx),false);

    COST

    std::sort(idx.value2iid.begin(), idx.value2iid.end());

    COST

    Series<IT, double> sr(std::move(idx), std::move(ar_val));

    COST

    using ST = Series<IT, double>;
    using SV = ST::SeriesVisitor<RangeVec<int>>;

    std::cout << sr.size() << "=====" << std::endl;

    std::vector<bool> mask(sr.size(), true);
    COST
    auto a = sr.loc_mask(mask);
    COST
    ST b(a.to_series(false));
    COST

    auto mask2 = b.values>10;
    COST
    auto c = b.where(mask2, pandas::nan<double>());
    COST
    
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