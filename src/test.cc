#include "pandas/array.h"
#include "pandas/dataframe.h"
#include "pandas/datetime.h"
#include "pandas/index.h"
#include "pandas/pandastype.h"
#include "pandas/series.h"

#include <iostream>
#include <map>

int main()
{
    using namespace pandas;
    using namespace std;

    try {
        {
            Datetime dt;

            std::string fmt = "%Y-%m-%d %H:%M:%S";
            dt.strptime("2024-01-01 12:13:14", fmt);

            std::cout << dt.strftime(fmt) << std::endl;

            Datetime dt2(2024, 1, 1, 12, 13, 14);
            std::cout << dt2.strftime(fmt) << std::endl;

            Double a = 1.0;
            Int b = 2;
            Double c = a + b;

            cout << c << " " << (a + 1) << endl;

            Array<Int> ar1({ 1, 2 });
            Array<Double> ar2({ 0.5, 1.0 });

            cout << ar1 << endl;
            cout << ar2 << endl;

            cout << (ar2 * ar1) << endl;

            Array<Int> ar3({
                1,
                2,
            });

            cout << ar3.iloc(0) << endl;

            Index<Int> idx1(ar3);

            cout << idx1.loc(2.0) << endl;

            idx1 = Array<Int>({ 1, 2, 3, 4 });

            cout << idx1 << endl;

            Series<Int, Int> ds1(Array<Int>({ 1, 2, 3 }), Array<Int>({ 2, 3, 4 }));

            cout << ds1 << endl;

            auto ds2 = ds1 + 1;

            cout << ds2 - ds1 << endl;

            ds1.iloc(0, 2) = 100;

            Series<Int, Double> ds3 = ds1.astype<Int, Double>();
            ds3.loc(ds1 > 10) += Array<Int>({ 99, 99 });

            cout << ds1 << endl;
            cout << ds3 << endl;

            cout << ds3.sum() << endl;

            cout << ds3.sort_values() << endl;

            cout << ds2.sort_index(false) << endl;

            cout << ds3.groupby(ds3).median() << endl;

            Series<Int, Int> ds5(Array<Int>({ 1, 2, 3 }), Array<Int>({ Int {}, Int(2), Int(3) }));

            cout << ds5 << endl;

            cout << ds5.rolling(5, 0).sum() << endl;

            cout << "===================" << endl;
        }

        Series<Int, Int> ds1(Array<Int>({ 0, 1 }), Array<Int>({ 10, 20 }, "ds1"));
        Series<Int, Int> ds2(Array<Int>({ 1, 2 }), Array<Int>({ 1, 2 }, "ds2"));

        DataFrame df = concat(ds1, ds2);

        cout << df.to_string() << endl;

        cout << df.sum() << endl;

    } catch (const std::string& s) {
        cout << "ERROR: " << s << endl;

    } catch (...) {
        cout << "ERROR: unknown" << endl;
    }

    return 0;
}