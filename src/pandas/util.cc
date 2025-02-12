#include "pandas/util.h"

namespace pandas {

std::vector<int> range(int b, int e, int d)
{
    std::vector<int> is;
    if (d > 0) {
        for (int i = b; i < e; i += d) {
            is.push_back(i);
        }
    } else if (d < 0) {
        for (int i = b; i > e; i += d) {
            is.push_back(i);
        }
    } else {
        is.push_back(b);
    }
    return is;
}

} // namespace pandas
