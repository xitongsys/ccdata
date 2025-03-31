#pragma once

#include <format>
#include <string>

namespace pandas {

#define PANDAS_THROW(msg) \
    throw std::format("[ERROR][{}][{}] {}", __FILE__, __LINE__, msg);

inline void pandas_throw(const std::string& msg)
{
    throw std::format("[ERROR][{}][{}] {}", __FILE__, __LINE__, msg);
}
}