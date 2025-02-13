#include <vector>

#include "pandas/array.h"
#include "pandas/pandastype.h"

namespace pandas {
class DataFrame {
public:
    std::vector<std::shared_ptr<void>> arrs;
    std::vector<std::string> columns;
    std::vector<PandasTypeId> dtypes;

    template <class T>
    Array<T>& get_col(int i)
    {
        return *std::static_pointer_cast<Array<T>>(arrs[i]);
    }

    template <class T>
    void append_col(Array<T>& ar)
    {
        std::make_shared
    }
};
}