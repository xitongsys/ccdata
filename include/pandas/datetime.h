#pragma once

#include <chrono>
#include <sstream>
#include <string>

namespace pandas {
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
};

}