#pragma once

#include <vector>

#include "pandas/array.h"
#include "pandas/pandastype.h"

namespace pandas {

template <class IT = Int, class DT>
class DataFrame {
public:
    std::vector<Array<DT>> values;
    Array<IT> index;
    
    DataFrame()
    {
    }

    template <class DT>
    Array<DT>& get_col(int i)
    {
        return values[i];
    }

    template <class DT>
    void append_col(const Array<DT>& ar)
    {
        arrs.append(ar);
    }

    Array<DT> sum()
    {
    }

    std::string to_string()
    {
        std::stringstream ss;

        for (auto& p : arrs) {
        }

        return ss.str();
    }
};
}