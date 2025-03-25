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
    std::chrono::time_point<std::chrono::system_clock, std::chrono::nanoseconds> t;
    bool isnan = false;

    std::string strftime(const std::string& fmt) const;

    void strptime(const std::string& s, const std::string& fmt);

    Datetime();
    Datetime(
        int year, int month, int day,
        int hour = 0, int minute = 0, int second = 0,
        long long nanosec = 0);

    Datetime(std::chrono::time_point<std::chrono::system_clock, std::chrono::nanoseconds> t_)
    {
        t = t_;
        isnan = false;
    }

    Datetime(const Datetime& dt);

    Datetime& operator=(const Datetime& dt);
    bool operator==(const Datetime& dt) const;
    bool operator>(const Datetime& dt) const;
    bool operator<(const Datetime& dt) const;
    bool operator>=(const Datetime& dt) const;
    bool operator<=(const Datetime& dt) const;
    bool operator!=(const Datetime& dt) const;

    Datetime operator+(const TimeDelta& dt) const;
    Datetime operator-(const TimeDelta& dt) const;
    void operator+=(const TimeDelta& dt);
    void operator-=(const TimeDelta& dt);
    TimeDelta operator-(const Datetime& t) const;

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

}