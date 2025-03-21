#include <iomanip>

#include "pandas/string.h"

namespace pandas {

std::vector<std::string> split(const std::string& s, const std::string& delimiter)
{
    size_t pos_start = 0, pos_end, delim_len = delimiter.length();
    std::string token;
    std::vector<std::string> res;

    while ((pos_end = s.find(delimiter, pos_start)) != std::string::npos) {
        token = s.substr(pos_start, pos_end - pos_start);
        pos_start = pos_end + delim_len;
        res.push_back(token);
    }

    res.push_back(s.substr(pos_start));
    return res;
}

std::string join(const std::vector<std::string>& strs, const std::string& connector)
{
    std::stringstream ss;
    for (int i = 0; i < strs.size(); i++) {
        ss << strs[i];
        if (i + 1 < strs.size()) {
            ss << connector;
        }
    }
    return ss.str();
}

std::string line_width_adjust(const std::string& s, int width)
{
    std::stringstream ss;
    std::vector<std::string> vs = split(s, "\n");
    if (width <= 0) {
        for (auto& v : vs) {
            if (v.size() > width) {
                width = v.size();
            }
        }
    }

    for (int i = 0; i < vs.size(); i++) {
        ss << std::left << std::setw(width) << vs[i];
        if (i + 1 < vs.size()) {
            ss << std::endl;
        }
    }
    return ss.str();
}

}