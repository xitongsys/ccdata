#pragma once

#include <chrono>
#include <ostream>
#include <sstream>
#include <string>

#include "pandas/ops.h"

namespace pandas {

class TimeDelta {
public:
    long long nanosecs;

    const long long SECOND = 1000000000LL;
    const long long MINUTE = SECOND * 60LL;
    const long long HOUR = MINUTE * 60LL;
    const long long DAY = HOUR * 24LL;

    TimeDelta();
    TimeDelta(const TimeDelta& dt);
    TimeDelta(long long total_nanosecs);
    TimeDelta(long long d, long long h, long long m, long long s, long long ns = 0);

    inline long long days() const
    {
        return nanosecs / DAY;
    }

    inline long long hours() const
    {
        return (nanosecs % DAY) / HOUR;
    }

    inline long long minutes() const
    {
        return (nanosecs % HOUR) / MINUTE;
    }

    inline long long seconds() const
    {
        return (nanosecs % MINUTE) / SECOND;
    }

    inline long long nanoseconds() const
    {
        return (nanosecs % SECOND);
    }

    inline long long total_nanosecs() const
    {
        return nanosecs;
    }

    inline double total_seconds() const
    {
        return (double)(nanosecs) / SECOND;
    }

    inline bool operator==(const TimeDelta& dt) const
    {
        return nanosecs == dt.nanosecs;
    }
    inline bool operator>(const TimeDelta& dt) const
    {
        return nanosecs > dt.nanosecs;
    }
    inline bool operator<(const TimeDelta& dt) const
    {
        return nanosecs < dt.nanosecs;
    }
    inline bool operator>=(const TimeDelta& dt) const
    {
        return nanosecs >= dt.nanosecs;
    }
    inline bool operator<=(const TimeDelta& dt) const
    {
        return nanosecs <= dt.nanosecs;
    }
    inline bool operator!=(const TimeDelta& dt) const
    {
        return nanosecs != dt.nanosecs;
    }

    inline TimeDelta operator+(const TimeDelta& dt) const
    {
        return TimeDelta(nanosecs + dt.nanosecs);
    }
    inline TimeDelta operator-(const TimeDelta& dt) const
    {
        return TimeDelta(nanosecs - dt.nanosecs);
    }

    inline TimeDelta& operator=(const TimeDelta& dt)
    {
        nanosecs = dt.nanosecs;
        return *this;
    }

    inline TimeDelta& operator+=(const TimeDelta& dt)
    {
        *this = ((*this) + dt);
        return *this;
    }
    inline TimeDelta& operator-=(const TimeDelta& dt)
    {
        *this = ((*this) - dt);
        return *this;
    }

    std::string to_string() const;

    friend std::ostream& operator<<(std::ostream& os, const TimeDelta& dt);
};

class Datetime {

public:
    static Datetime now();

public:
    long long t;

    std::string strftime(const std::string& fmt) const;

    void strptime(const std::string& s, const std::string& fmt);

    Datetime();
    Datetime(const std::string& s, const std::string& fmt = "%Y-%m-%d %H:%M:%S");
    Datetime(
        int year, int month, int day,
        int hour = 0, int minute = 0, int second = 0,
        long long nanosec = 0);

    Datetime(long long nanosecs);
    Datetime(const Datetime& dt);

    inline Datetime& operator=(const Datetime& dt)
    {
        t = dt.t;
        return *this;
    }

    inline bool operator>(const Datetime& dt) const
    {
        return t > dt.t;
    }

    inline bool operator<(const Datetime& dt) const
    {
        return t < dt.t;
    }

    inline bool operator==(const Datetime& dt) const
    {
        return t == dt.t;
    }

    inline bool operator>=(const Datetime& dt) const
    {
        return t >= dt.t;
    }

    inline bool operator<=(const Datetime& dt) const
    {
        return t <= dt.t;
    }

    inline bool operator!=(const Datetime& dt) const
    {
        return t != dt.t;
    }

    //// Datetime with TimeDelta

    inline Datetime operator+(const TimeDelta& dt) const
    {
        return Datetime(t + dt.total_nanosecs());
    }

    inline Datetime operator-(const TimeDelta& dt) const
    {
        return Datetime(t - dt.total_nanosecs());
    }

    inline void operator+=(const TimeDelta& dt)
    {
        t += dt.total_nanosecs();
    }

    inline void operator-=(const TimeDelta& dt)
    {
        t -= dt.total_nanosecs();
    }

    inline TimeDelta operator-(const Datetime& t_) const
    {
        return TimeDelta(t - t_.t);
    }

    struct Number {
        int year, month, day;
        int hour, minute, second;
        long long nanosec;
    };

    inline Datetime::Number number() const
    {
        using namespace std::chrono;

        auto tp = std::chrono::time_point<std::chrono::system_clock, std::chrono::nanoseconds>(std::chrono::nanoseconds(t));
        auto local_tp = zoned_time { current_zone(), tp }.get_local_time();

        auto tp_day = std::chrono::floor<days>(local_tp);
        year_month_day ymd { tp_day };
        auto year = int(ymd.year());
        auto month = unsigned(ymd.month());
        auto day = unsigned(ymd.day());

        auto time_of_day = local_tp - tp_day;
        hh_mm_ss time_in_day(time_of_day);
        auto hour = time_in_day.hours().count();
        auto minute = time_in_day.minutes().count();
        auto second = time_in_day.seconds().count();
        auto nanosecond = time_in_day.subseconds().count();

        Datetime::Number num = {
            year, month, day, hour, minute, second, nanosecond
        };
        return num;
    }

    inline long long total_nanosecs() const
    {
        return t;
    }

    inline double total_seconds() const
    {
        return t / 1e9;
    }

    inline int year() const
    {
        return number().year;
    }
    inline int month() const
    {
        return number().month;
    }
    inline int day() const
    {
        return number().day;
    }
    inline int hour() const
    {
        return number().hour;
    }
    inline int minute() const
    {
        return number().minute;
    }
    inline int second() const
    {
        return number().second;
    }
    inline long long nansec() const
    {
        return number().nanosec;
    }

    inline Datetime date() const
    {
        Datetime::Number num = number();
        return Datetime(num.year, num.month, num.day, 0, 0, 0, 0);
    }

    std::string to_string() const;

    friend std::ostream& operator<<(std::ostream& os, const Datetime& dt);
};

template <>
std::string to_string(const Datetime& dt);

template <>
bool isnan(const Datetime& dt);

template <>
Datetime nan();

std::size_t hash_value(const Datetime& dt);

}

namespace std {
template <>
struct hash<pandas::Datetime> {
    size_t operator()(const pandas::Datetime& dt) const
    {
        return hash<long long>()(dt.t);
    }
};
}
