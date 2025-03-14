#pragma once

#include <chrono>
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

    const long long NANO = 1000000000LL;

    TimeDelta() { }

    TimeDelta(long long d = 0, long long h = 0, long long m = 0, long long s = 0, long long ns = 0)
        : days(d)
        , hours(h)
        , minutes(m)
        , seconds(s)
        , nanosecs(ns)
    {
    }

    long long total_nanosecs()
    {
        long long nanos = days * 3600 * 24 * NANO + hours * 3600 * NANO + minutes * 60 * NANO + seconds * NANO + nanosecs;
        return nanos;
    }

    double total_seconds()
    {
        double nanos = total_nanosecs();
        double secs = nanos / NANO;
        return secs;
    }
};

class Datetime {

public:
    std::chrono::time_point<std::chrono::system_clock, std::chrono::nanoseconds> t;

    std::string strftime(const std::string& fmt);

    void strptime(const std::string& s, const std::string& fmt);

    Datetime();
    Datetime(int year, int month, int day, int hour = 0, int minute = 0, int second = 0, int nanosec = 0);
    Datetime(const Datetime& dt);
    Datetime& operator=(const Datetime& dt);
    bool operator==(const Datetime& dt);
    bool operator>(const Datetime& dt);
    bool operator<(const Datetime& dt);

    // Datetime operator+(const TimeDelta& dt);
    // Datetime operator-(const TimeDelta& dt);
    // Datetime operator+=(const TimeDelta& dt);
    // Datetime operator-=(const TimeDelta& dt);
    // TimeDelta operator-(const Datetime& t);
};

}