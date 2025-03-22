#pragma once

#include <fstream>
#include <string>
#include <vector>

#include "pandas/array.h"

namespace pandas {
namespace csv {

    std::vector<std::string> read_row(std::istream& in, char delimiter);

    std::vector<Array<std::string, std::string>> read_csv(const std::string& filename, bool has_header, char delimiter);

}
}