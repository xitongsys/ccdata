#include <map>
#include <tuple>

#include <boost/functional/hash.hpp>

#include "pandas/util.h"

namespace std {

template <typename... Args>
struct hash<std::tuple<Args...>> {
    std::size_t operator()(const std::tuple<Args...>& t) const
    {
        return boost::hash_value(t);
    }
};

}