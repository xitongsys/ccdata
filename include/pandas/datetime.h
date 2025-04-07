#pragma once

#include <chrono>
#include <ostream>
#include <sstream>
#include <string>

#include "pandas/ops.h"

namespace pandas {

class TimeDelta {
public:
    long long days;
    long long hours;
    long long minutes;
    long long seconds;
    long long nanosecs;

    const long long SECOND = 1000000000LL;
    const long long MINUTE = SECOND * 60LL;
    const long long HOUR = MINUTE * 60LL;
    const long long DAY = HOUR * 24LL;

    TimeDelta();
    TimeDelta(const TimeDelta& dt);
    TimeDelta(long long total_nanosecs);
    TimeDelta(long long d, long long h, long long m, long long s, long long ns = 0);
    long long total_nanosecs() const;
    double total_seconds() const;

    bool operator==(const TimeDelta& dt) const;
    bool operator>(const TimeDelta& dt) const;
    bool operator<(const TimeDelta& dt) const;
    bool operator>=(const TimeDelta& dt) const;
    bool operator<=(const TimeDelta& dt) const;
    bool operator!=(const TimeDelta& dt) const;

    TimeDelta operator+(const TimeDelta& dt) const;
    TimeDelta operator-(const TimeDelta& dt) const;
    void operator+=(const TimeDelta& dt);
    void operator-=(const TimeDelta& dt);
    void operator=(const TimeDelta& dt);

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

    // Datetime(std::chrono::time_point<std::chrono::system_clock, std::chrono::nanoseconds> t_);

    Datetime(const Datetime& dt);

    long long total_nanosecs() const;
    long long total_seconds() const;

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

    Number number() const;

    int year() const;
    int month() const;
    int day() const;
    int hour() const;
    int minute() const;
    int second() const;
    long long nansec() const;

    Datetime date() const;

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
        return hash<long long>()(dt.total_nanosecs());
    }
};
}
