#pragma once

#include <vector>

#include "pandas/array.h"
#include "pandas/pandastype.h"

namespace pandas {

template <class IT = Int>
class DataFrame {
public:
    std::vector<std::shared_ptr<ArrayBase>> arrs;
    Array<IT> index;

    DataFrame()
    {
    }

    template <class T>
    Array<T>& get_col(int i)
    {
        return *std::static_pointer_cast<Array<T>>(arrs[i]);
    }

    template <class T>
    void append_col(const Array<T>& ar)
    {
        auto p_ar = std::make_shared<Array<T>>(ar);
        arrs.push_back(p_ar);
    }

    DataFrame sum()
    {
    }

    template<class T>

    std::string to_string()
    {
        std::stringstream ss;

        for (auto& p : arrs) {
           
        }

        return ss.str();
    }
};
}