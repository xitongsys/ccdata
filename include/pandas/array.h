#pragma once

#include <functional>
#include <initializer_list>
#include <iostream>
#include <list>
#include <map>
#include <optional>
#include <vector>

#include "pandas/pandastype.h"
#include "pandas/util.h"

namespace pandas {

template <class T, class NT = std::string>
class Array {
public:
    std::vector<T> values;
    NT name;

    Array()
    {
    }

    Array(const NT& name)
    {
        this->name = name;
    }

    Array(size_t size, const T& init_val, const NT& name = NT {})
        : Array(name)
    {
        values = std::vector<T>(size, init_val);
    }

    template <class T2>
    Array(const Array<T2>& ar, const NT& name = NT {})
        : Array(name)
    {
        for (int i = 0; i < ar.size(); i++) {
            values.push_back(ar.iloc(i));
        }
    }

    template <class T2>
    Array(const std::vector<T2>& vals, const NT& name = NT {})
        : Array(name)
    {
        for (int i = 0; i < vals.size(); i++) {
            values.push_back((T)(vals[i]));
        }
    }

    template <class T2>
    Array(const std::initializer_list<T2>& vals, const NT& name = NT {})
        : Array(name)
    {
        for (const T2& v : vals) {
            values.push_back((T)(v));
        }
    }

    Array(const Array& ar)
        : Array()
    {
        for (int i = 0; i < ar.size(); i++) {
            values.push_back(ar.iloc(i));
        }
        name = ar.get_name();
    }

    Array(Array&& ar)
        : Array()
    {
        values = std::move(ar.values);
        name = std::move(ar.name);
    }

    template <class T2>
    Array& operator=(const Array<T2>& ar)
    {
        values.clear();
        for (int i = 0; i < ar.size(); i++) {
            values.push_back(ar.iloc(i));
        }
        name = ar.name;
        return *this;
    }

    Array& operator=(const Array& ar)
    {
        values.clear();
        for (int i = 0; i < ar.size(); i++) {
            values.push_back(ar.iloc(i));
        }
        name = ar.name;
        return *this;
    }

    Array& operator=(Array&& ar)
    {
        values = std::move(ar.values);
        name = ar.name;
        return *this;
    }

    NT get_name() const
    {
        return name;
    }

    Array rename(const NT& name) const
    {
        Array res = *this;
        res._rename(name);
        return res;
    }

    void _rename(const NT& name)
    {
        this->name = name;
    }

    inline T& iloc_ref(int i)
    {
        return values[i];
    }

    inline T iloc(int i) const
    {
        return values[i];
    }

    void _append(const T& v)
    {
        values.push_back(v);
    }

    Array<T> append(const T& v)
    {
        Array<T> ar = *this;
        ar._append(v);
        return ar;
    }

    void _clear()
    {
        values.clear();
    }

    template <class T2>
    void _append(const Array<T2>& ar)
    {
        for (int i = 0; i < ar.size(); i++) {
            _append((T)(ar.iloc(i)));
        }
    }

    template <class T2, class NT2 = std::string>
    Array<T2, NT2> astype()
    {
        return Array<T2, NT2>(*this);
    }

    size_t size() const
    {
        return values.size();
    }

    Array<Int, NT> duplicated(const std::string& keep)
    {
        Array<Int, NT> dup;
        std::map<T, int> mp;
        if (keep == "first") {
            for (int i = 0; i < size(); i++) {
                T& v = iloc(i);
                if (mp.count(v)) {
                    dup.append(1);
                } else {
                    dup.append(0);
                }
                mp[v] = i;
            }
        } else if (keep == "last") {
            for (int i = 0; i < size(); i++) {
                dup.append(0);
            }
            for (int i = size() - 1; i >= 0; i--) {
                T& v = iloc(i);
                if (mp.count(v)) {
                    dup.iloc(i) = 1;
                } else {
                    dup.iloc(i) = 0;
                }
                mp[v] = i;
            }

        } else if (keep == "false") {
            for (int i = 0; i < size(); i++) {
                T& v = iloc(i);
                dup.append(0);
                if (mp.count(v) == 0) {
                    mp[v] = 0;
                }
                mp[v]++;
            }
            for (int i = 0; i < size(); i++) {
                T& v = iloc(i);
                if (mp.count(v) > 1) {
                    dup.iloc(i) = 1;
                }
            }
        } else {
            throw std::format("unknow dedup method: {}", keep);
        }

        return dup;
    }

    Array<T, NT> reverse()
    {
        Array<T, NT> ar = *this;
        for (int i = 0, j = size() - 1; i < j; i++, j--) {
            std::swap(ar.values[i], ar.values[j]);
        }
        return ar;
    }

    void _reverse()
    {
        for (int i = 0, j = size() - 1; i < j; i++, j--) {
            std::swap(values[i], values[j]);
        }
    }

    Array<T, NT> sort(bool ascending = true)
    {
        Array<T, NT> ar = *this;
        sort(ar.values.begin(), ar.values.end());
        if (!ascending) {
            return ar.reverse();
        }
        return ar;
    }

    void _sort(bool ascending = true)
    {
        sort(values.begin(), values.end());
        if (!ascending) {
            _reverse();
        }
    }

    std::string to_string(int mx_cnt = 10) const
    {
        std::stringstream ss;
        if (size() > mx_cnt) {
            for (int i = 0; i < mx_cnt / 2; i++) {
                ss << pandas::to_string(values[i]) << ",";
            }
            ss << "...";
            for (int i = size() - mx_cnt / 2; i < size(); i++) {
                ss << pandas::to_string(values[i]) << ",";
            }
        } else {
            for (int i = 0; i < size(); i++) {
                ss << pandas::to_string(values[i]) << ",";
            }
        }

        std::string s = std::format("{}:[{}]", get_name(), ss.str());
        return s;
    }

    friend std::ostream& operator<<(std::ostream& os, const Array& ar)
    {
        os << ar.to_string();
        return os;
    }

#include "pandas/array_op.tcc"
};

template <class T, class NT>
Array<T, NT> concat_0(const Array<T, NT>& ar1, const Array<T, NT>& ar2)
{
    Array<T, NT> res = ar1;
    for (int i = 0; i < ar2.size(); i++) {
        res._append(ar2.iloc(i));
    }
    return res;
}

template <class T, class NT>
Array<std::tuple<T>, NT> zip(const Array<T, NT>& ar)
{
    Array<std::tuple<T>, NT> res;
    for (int i = 0; i < ar.size(); i++) {
        res._append(std::tuple(ar.iloc(i)));
    }
    return res;
}

template <class T, class NT, class... Ts>
auto zip(const Array<T, NT>& ar, const Ts&... ars)
{
    Array<std::tuple<T>, NT> res;
    for (int i = 0; i < ar.size(); i++) {
        res._append(std::tuple(ar.iloc(i)));
    }
    constexpr int N_TAIL = sizeof...(ars);
    if constexpr (N_TAIL > 0) {
        auto res_tail = zip(ars...);

        if (res_tail.size() != res.size()) {
            throw std::format("size not match: {}!={}", res_tail.size(), res.size());
        }

        Array<decltype(add_first_element(std::get<0>(res.iloc(0)), res_tail.iloc(0))), NT> res_new;

        for (int i = 0; i < res_tail.size(); i++) {
            auto v = add_first_element(std::get<0>(res.iloc(i)), res_tail.iloc(i));
            res_new._append(v);
        }

        return res_new;

    } else {
        return res;
    }
}

}