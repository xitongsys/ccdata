#include "pandas/datetime.h"

namespace pandas {

Datetime::Datetime()
{
    isnan = true;
}

Datetime::Number Datetime::number() const
{
    std::chrono::seconds sec = std::chrono::duration_cast<std::chrono::seconds>(t.time_since_epoch());
    std::time_t tt = sec.count();
    std::tm tm = *std::localtime(&tt);

    Datetime::Number num = {
        tm.tm_year + 1900,
        tm.tm_mon + 1,
        tm.tm_mday,
        tm.tm_hour,
        tm.tm_min,
        tm.tm_sec,
        0,
    };
    return num;
}

Datetime::Datetime(const Datetime& dt)
{
    t = dt.t;
    isnan = dt.isnan;
}

Datetime& Datetime::operator=(const Datetime& dt)
{
    t = dt.t;
    isnan = dt.isnan;
    return *this;
}

bool Datetime::operator>(const Datetime& dt) const
{
    return t > dt.t;
}

bool Datetime::operator<(const Datetime& dt) const
{
    return t < dt.t;
}

bool Datetime::operator==(const Datetime& dt) const
{
    return t == dt.t;
}

bool Datetime::operator>=(const Datetime& dt) const
{
    return t >= dt.t;
}

bool Datetime::operator<=(const Datetime& dt) const
{
    return t <= dt.t;
}

bool Datetime::operator!=(const Datetime& dt) const
{
    return t != dt.t;
}

Datetime Datetime::date() const
{
    Datetime::Number num = number();
    return Datetime(num.year, num.month, num.day, 0, 0, 0, 0);
}

Datetime::Datetime(int year, int month, int day, int hour, int minute, int second, long long nanosec)
{
    isnan = false;
    std::tm tm = {};
    tm.tm_year = year - 1900;
    tm.tm_mon = month - 1;
    tm.tm_mday = day;
    tm.tm_hour = hour;
    tm.tm_min = minute;
    tm.tm_sec = second;

    std::time_t tt = std::mktime(&tm);
    t = std::chrono::system_clock::from_time_t(tt) + std::chrono::nanoseconds(nanosec);
}

std::string Datetime::strftime(const std::string& fmt) const
{
    if (isnan) {
        return "NaT";
    }
    std::stringstream ss;

    std::chrono::seconds sec = std::chrono::duration_cast<std::chrono::seconds>(t.time_since_epoch());
    std::time_t tt = sec.count();

    std::tm tm = *std::localtime(&tt);
    ss << std::put_time(&tm, fmt.c_str());
    return ss.str();
}

void Datetime::strptime(const std::string& s, const std::string& fmt)
{
    std::stringstream ss(s);
    std::tm tm = {};
    ss >> std::get_time(&tm, fmt.c_str());
    t = std::chrono::system_clock::from_time_t(std::mktime(&tm));
    isnan = false;
}

std::string Datetime::to_string() const
{
    if (isnan) {
        return "NaT";
    } else {
        return strftime("%Y-%m-%d %H:%M:%S");
    }
}

std::ostream& operator<<(std::ostream& os, const Datetime& dt)
{
    os << dt.to_string();
    return os;
}

/////////////////////////

template <>
std::string to_string(const Datetime& dt)
{
    return dt.to_string();
}

template <>
bool isnan(const Datetime& dt)
{
    return dt.isnan;
}

template <>
Datetime nan()
{
    return Datetime();
}

}