#include "pandas/array.h"
#include "pandas/datetime.h"
#include "pandas/pandastype.h"

#include <iostream>
#include <map>

int& fun(int& p)
{
    return p;
}

int main()
{
    using namespace pandas;
    using namespace std;
    Datetime dt;

    std::string fmt = "%Y-%m-%d %H:%M:%S";
    dt.strptime("2024-01-01 12:13:14", fmt);

    std::cout << dt.strftime(fmt) << std::endl;

    Datetime dt2(2024, 1, 1, 12, 13, 14);
    std::cout << dt2.strftime(fmt) << std::endl;

    Double a(1), b(1);

    std::cout << (a / b).to_string() << std::endl;

    a = b;

    Array<Int> ar;
    ar.append(1);
    ar.append(5);
    ar.append(10);

    ar.create_index();
    ar.loc((ar >= 5) & (ar < 10)) = 100;
    // ar.iloc(0,2) = 10;

    auto br = (ar > 5);

    cout << ar.to_string() << endl;
    cout << br.to_string() << endl;

    auto cr = ar.map<Int>([](Int a) -> Int { return a * a; });

    auto af = [](const Array<Int>& ds) -> Int {
        Int s = 0;
        for (int i = 0; i < ds.size(); i++) {
            s += ds.iloc(i);
        }
        return s;
    };

    cout << ar.agg<Int>(af).to_string() << endl;

    cout << cr.to_string() << endl;

    auto res = ar.groupby(br).sum();

    cout << "key: " << std::get<0>(res).to_string() << endl;
    cout << "value:" << std::get<1>(res).to_string() << endl;

    return 0;
}