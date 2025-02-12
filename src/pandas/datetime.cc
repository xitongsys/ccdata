#include "pandas/datetime.h"

namespace pandas {

Datetime::Datetime()
{
}

Datetime::Datetime(const Datetime& dt)
{
    t = dt.t;
}

Datetime& Datetime::operator=(const Datetime& dt)
{
    t = dt.t;
    return *this;
}

bool Datetime::operator>(const Datetime& dt)
{
    return t > dt.t;
}

bool Datetime::operator<(const Datetime& dt)
{
    return t < dt.t;
}

bool Datetime::operator==(const Datetime& dt)
{
    return t == dt.t;
}

Datetime::Datetime(int year, int month, int day, int hour, int minute, int second, int nanosec)
{
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

std::string Datetime::strftime(const std::string& fmt)
{
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
}

}