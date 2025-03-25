#pragma once

#include <fstream>
#include <string>
#include <vector>

#include "pandas/array.h"

namespace pandas {
namespace csv {

    std::vector<std::string> read_row(std::istream& in, char delimiter = ',');

    std::vector<Array<std::string, std::string>> read_csv(const std::string& filename, bool has_header = true, char delimiter = ',');

    template <class T>
    std::string format_csv_string(const T& v)
    {
        return pandas::to_string(v);
    }

    std::string format_csv_string(const std::string& s);

    void write_csv(const std::string& filename, const std::vector<std::string>& cols, char delimiter = ',');

}
}