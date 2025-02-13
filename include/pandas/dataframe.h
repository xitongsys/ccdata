#include <vector>

#include "pandas/array.h"
#include "pandas/pandastype.h"

namespace pandas {

template <class IT>
class DataFrame {
public:
    std::vector<std::shared_ptr<ArrayBase>> arrs;
    Array<IT> index;

    template <class T>
    Array<T>& get_col(int i)
    {
        return *std::static_pointer_cast<Array<T>>(arrs[i]);
    }

    template <class T>
    void append_col(Array<T>& ar)
    {
        auto p_ar = std::make_shared<Array<T>>(ar);
        arrs.append(p_ar);
    }

    DataFrame sum()
};
}