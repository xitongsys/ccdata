#include "pandas/datetime.h"

namespace pandas {

///// TimeDelta //////////////////

TimeDelta::TimeDelta()
{
    nanosecs = 0;
}

TimeDelta::TimeDelta(const TimeDelta& dt)
{
    nanosecs = dt.nanosecs;
}

TimeDelta::TimeDelta(long long total_nanosecs)
{
    nanosecs = total_nanosecs;
}

TimeDelta::TimeDelta(long long days, long long hours, long long minutes, long long seconds, long long ns)
{
    nanosecs = days * DAY + hours * HOUR + minutes * MINUTE + seconds * SECOND + ns;
}

std::string TimeDelta::to_string() const
{
    return std::format("{} {:0>2}:{:0>2}:{:0>2}.{}", days(), hours(), minutes(), seconds(), nanoseconds());
}

std::ostream& operator<<(std::ostream& os, const TimeDelta& dt)
{
    return os << dt.to_string();
}

///// Datetime //////////////////////

Datetime::Datetime()
{
    t = 0;
}

Datetime::Datetime(const std::string& s, const std::string& fmt)
{
    this->strptime(s, fmt);
}

Datetime Datetime::now()
{
    auto tp = std::chrono::system_clock::now();
    long long t = std::chrono::duration_cast<std::chrono::nanoseconds>(tp.time_since_epoch()).count();
    return Datetime(t);
}

Datetime::Datetime(const Datetime& dt)
{
    t = dt.t;
}

Datetime::Datetime(long long nanosecs)
{
    // t = std::chrono::time_point<std::chrono::system_clock, std::chrono::nanoseconds>(std::chrono::nanoseconds(nanosecs));
    t = nanosecs;
}

Datetime::Datetime(int year, int month, int day, int hour, int minute, int second, long long nanosec)
{
    std::tm tm = {};
    tm.tm_year = year - 1900;
    tm.tm_mon = month - 1;
    tm.tm_mday = day;
    tm.tm_hour = hour;
    tm.tm_min = minute;
    tm.tm_sec = second;

    std::time_t tt = std::mktime(&tm);
    auto tp = std::chrono::system_clock::from_time_t(tt) + std::chrono::nanoseconds(nanosec);
    t = std::chrono::duration_cast<std::chrono::nanoseconds>(tp.time_since_epoch()).count();
}

std::string Datetime::strftime(const std::string& fmt) const
{
    std::stringstream ss;

    auto tp = std::chrono::time_point<std::chrono::system_clock, std::chrono::nanoseconds>(std::chrono::nanoseconds(t));

    std::chrono::seconds sec = std::chrono::duration_cast<std::chrono::seconds>(tp.time_since_epoch());
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
    auto tp = std::chrono::system_clock::from_time_t(std::mktime(&tm));
    t = std::chrono::duration_cast<std::chrono::nanoseconds>(tp.time_since_epoch()).count();
}

std::string Datetime::to_string() const
{
    return strftime("%Y-%m-%d %H:%M:%S");
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
    return false;
}

template <>
Datetime nan()
{
    PANDAS_THROW(std::format("Datetime not support NaN"));
}

std::size_t hash_value(const Datetime& dt)
{
    return std::hash<long long>()(dt.total_nanosecs());
}

}