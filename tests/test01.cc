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
    Index<int> id1(Array<int>({ 1, 2, 3 }, "id1"));
    Index<int> id2(Array<int>({ 2, 3, 4 }, "id2"));

    auto id_0 = concat<0>(id1, id2);
    cout << id_0 << endl;

    auto id_1 = concat<1>(id1, id2);
    cout << id_1 << endl;
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