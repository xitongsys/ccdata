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

void test01()
{
    DataFrame<int, double> df(std::vector<int>({ 1, 2 }), std::vector<std::string>({ "a", "b" }));
    df = df.fillna(1);
    cout << df << endl;

    cout << df.corr<1>() << endl;
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