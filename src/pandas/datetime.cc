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
    std::chrono::year_month_day ymd{std::chrono::year{year}, std::chrono::month{(unsigned int)month}, std::chrono::day{(unsigned int)day}};
    std::chrono::hh_mm_ss hms{std::chrono::hours{hour} + std::chrono::minutes{minute} + std::chrono::seconds{second} + std::chrono::nanoseconds{nanosec}};
    std::chrono::sys_days sys_days = ymd;
    auto total_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(hms.hours()).count() +
                    std::chrono::duration_cast<std::chrono::nanoseconds>(hms.minutes()).count() +
                    std::chrono::duration_cast<std::chrono::nanoseconds>(hms.seconds()).count() +
                    hms.subseconds().count();
    auto time_of_day_in_ns = std::chrono::nanoseconds(total_ns);
    auto time_point = sys_days + time_of_day_in_ns;

    auto duration_since_epoch = time_point.time_since_epoch();
    auto nanoseconds_since_epoch = std::chrono::duration_cast<std::chrono::nanoseconds>(duration_since_epoch).count();
    t = nanoseconds_since_epoch;    
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
    Number num = this->number();
    return std::format("{:04}-{:02}-{:02} {:02}:{:02}:{:02}.{:09}", num.year, num.month, num.day, num.hour, num.minute, num.second, num.nanosec);
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