#pragma once

#include <sstream>
#include <string>
#include <vector>

namespace pandas {
std::vector<std::string> split(const std::string& s, const std::string& delimiter);

std::string join(const std::vector<std::string>& strs, const std::string& connector);

std::string line_width_adjust(const std::string& s, int width = 0);

}