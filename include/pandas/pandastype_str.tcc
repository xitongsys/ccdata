#pragma once

template <>
std::string PandasType<std::string>::to_string() const
{
    return value;
}

