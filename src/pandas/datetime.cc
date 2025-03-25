#include "pandas/datetime.h"

namespace pandas {

///// TimeDelta //////////////////

TimeDelta::TimeDelta()
{
    days = 0;
    hours = 0;
    minutes = 0;
    seconds = 0;
    nanosecs = 0;
}

TimeDelta::TimeDelta(const TimeDelta& dt)
{
    days = dt.days;
    hours = dt.hours;
    minutes = dt.minutes;
    seconds = dt.seconds;
    nanosecs = dt.nanosecs;
}

TimeDelta::TimeDelta(long long total_nanosecs)
{
    days = total_nanosecs / DAY;
    total_nanosecs %= DAY;

    hours = total_nanosecs / HOUR;
    total_nanosecs %= HOUR;

    minutes = total_nanosecs / MINUTE;
    total_nanosecs %= MINUTE;

    seconds = total_nanosecs / SECOND;
    total_nanosecs %= SECOND;

    nanosecs = total_nanosecs;
}

TimeDelta::TimeDelta(long long d, long long h, long long m, long long s, long long ns)
    : days(d)
    , hours(h)
    , minutes(m)
    , seconds(s)
    , nanosecs(ns)
{
}

long long TimeDelta::total_nanosecs() const
{
    long long nanos = days * DAY + hours * HOUR + minutes * MINUTE + seconds * SECOND + nanosecs;
    return nanos;
}

double TimeDelta::total_seconds() const
{
    double nanos = total_nanosecs();
    double secs = nanos / SECOND;
    return secs;
}

bool TimeDelta::operator==(const TimeDelta& dt) const
{
    return total_nanosecs() == dt.total_nanosecs();
}
bool TimeDelta::operator>(const TimeDelta& dt) const
{
    return total_nanosecs() > dt.total_nanosecs();
}
bool TimeDelta::operator<(const TimeDelta& dt) const
{
    return total_nanosecs() < dt.total_nanosecs();
}
bool TimeDelta::operator>=(const TimeDelta& dt) const
{
    return total_nanosecs() >= total_nanosecs();
}
bool TimeDelta::operator<=(const TimeDelta& dt) const
{
    return total_nanosecs() <= total_nanosecs();
}
bool TimeDelta::operator!=(const TimeDelta& dt) const
{
    return total_nanosecs() != total_nanosecs();
}

TimeDelta TimeDelta::operator+(const TimeDelta& dt) const
{
    return TimeDelta(total_nanosecs() + dt.total_nanosecs());
}
TimeDelta TimeDelta::operator-(const TimeDelta& dt) const
{
    return TimeDelta(total_nanosecs() - dt.total_nanosecs());
}

void TimeDelta::operator=(const TimeDelta& dt)
{
    days = dt.days;
    hours = dt.hours;
    minutes = dt.minutes;
    seconds = dt.seconds;
    nanosecs = dt.nanosecs;
}

void TimeDelta::operator+=(const TimeDelta& dt)
{
    *this = ((*this) + dt);
}
void TimeDelta::operator-=(const TimeDelta& dt)
{
    *this = ((*this) - dt);
}

std::string TimeDelta::to_string() const
{
    return std::format("{} {:0>2}:{:0>2}:{:0>2}.{}", days, hours, minutes, seconds, nanosecs);
}

std::ostream& operator<<(std::ostream& os, const TimeDelta& dt)
{
    return os << dt.to_string();
}

///// Datetime //////////////////////

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

int Datetime::year() const
{
    return number().year;
}
int Datetime::month() const
{
    return number().month;
}
int Datetime::day() const
{
    return number().day;
}
int Datetime::hour() const
{
    return number().hour;
}
int Datetime::minute() const
{
    return number().minute;
}
int Datetime::second() const
{
    return number().second;
}
long long Datetime::nansec() const
{
    return number().nanosec;
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

//// Datetime with TimeDelta

Datetime Datetime::operator+(const TimeDelta& dt) const
{
    return Datetime(t + std::chrono::nanoseconds(dt.total_nanosecs()));
}

Datetime Datetime::operator-(const TimeDelta& dt) const
{
    return Datetime(t - std::chrono::nanoseconds(dt.total_nanosecs()));
}

void Datetime::operator+=(const TimeDelta& dt)
{
    t += std::chrono::nanoseconds(dt.total_nanosecs());
}

void Datetime::operator-=(const TimeDelta& dt)
{
    t -= std::chrono::nanoseconds(dt.total_nanosecs());
}

TimeDelta Datetime::operator-(const Datetime& t_) const
{
    auto dt = t - t_.t;
    long long nanos = std::chrono::duration_cast<std::chrono::nanoseconds>(dt).count();
    return TimeDelta(nanos);
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