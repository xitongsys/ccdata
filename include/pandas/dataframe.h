#include <vector>

#include "pandas/array.h"
#include "pandas/pandastype.h"

namespace pandas {
class DataFrame {
public:
    std::vector<std::shared_ptr<void>> arrs;
    std::vector<std::string> columns;
    std::vector<PandasTypeId> dtypes;
};
}