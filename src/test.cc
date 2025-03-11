#include "pandas/array.h"
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

    } catch (const std::string& s) {
        cout << "ERROR: " << s << endl;

    } catch (...) {
        cout << "ERROR: unknown" << endl;
    }

    return 0;
}