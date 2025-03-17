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
#include "pandas/visitor.h"

namespace pandas {

template <class T>
class Array : public Visitor<T> {
public:
    std::string name;
    PandasTypeId dtype;

    std::vector<T> values;

    Array()
    {
        name = "";
        dtype = pandas_type_to_id<T>();
    }

    Array(size_t size, const T& init_val)
        : Array()
    {
        values = std::vector<T>(size, init_val);
    }

    Array(const std::string& nm)
        : Array()
    {
        name = nm;
    }

    template <class T2>
    Array(const Array<T2>& ar)
        : Array()
    {
        name = ar.name;
        for (int i = 0; i < ar.size(); i++) {
            values.push_back(ar.iloc(i));
        }
    }

    template <class T2>
    Array(const std::vector<T2>& vals, const std::string& nm = "")
        : Array()
    {
        name = nm;
        for (int i = 0; i < vals.size(); i++) {
            values.push_back((T)(vals[i]));
        }
    }

    template <class T2>
    Array(const std::initializer_list<T2>& vals, const std::string& nm = "")
    {
        for (const T2& v : vals) {
            values.push_back((T)(v));
        }
        name = nm;
    }

    Array(const Array& ar)
        : Array()
    {
        name = ar.name;
        for (int i = 0; i < ar.size(); i++) {
            values.push_back(ar.iloc(i));
        }
    }

    Array(Array&& ar)
        : Array()
    {
        name = std::move(ar.name);
        values = std::move(ar.values);
    }

    template <class T2>
    Array& operator=(const Array<T2>& ar)
    {
        name = ar.name;
        dtype = pandas_type_to_id<T>();
        values.clear();
        for (int i = 0; i < ar.size(); i++) {
            values.push_back(ar.iloc(i));
        }
        return *this;
    }

    Array& operator=(const Array& ar)
    {
        name = ar.name;
        dtype = pandas_type_to_id<T>();
        values.clear();
        for (int i = 0; i < ar.size(); i++) {
            values.push_back(ar.iloc(i));
        }
        return *this;
    }

    Array& operator=(Array&& ar)
    {
        name = std::move(ar.name);
        dtype = pandas_type_to_id<T>();
        values = std::move(ar.values);
        return *this;
    }

    T& iloc(int i)
    {
        return values[i];
    }

    T iloc(int i) const
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
        values._clear();
    }

    template <class T2>
    void _append(const Array<T2>& ar)
    {
        for (int i = 0; i < ar.size(); i++) {
            _append((T)(ar.iloc(i)));
        }
    }

    template <class T2>
    Array<T2> astype()
    {
        return Array<T2>(*this);
    }

    size_t size() const
    {
        return values.size();
    }

    Array<Int> duplicated(const std::string& keep)
    {
        Array<Int> dup;
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

    Array<T> reverse()
    {
        Array<T> ar = *this;
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

    Array<T> sort(bool ascending = true)
    {
        Array<T> ar = *this;
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

        std::string s = std::format("{}:[{}]", name, ss.str());
        return s;
    }

    friend std::ostream& operator<<(std::ostream& os, const Array& ar)
    {
        os << ar.to_string();
        return os;
    }

#include "pandas/array_op.tcc"
};

template <class T>
Array<T> concat_0(const Array<T>& ar)
{
    Array<T> res = ar;
    return res;
}

template <class T, class... Ts>
Array<T> concat_0(const Array<T>& ar, const Array<Ts...>& ars)
{
    Array<T> res = ar;
    Array<T> res_tail = concat_0(ars);
    for (int i = 0; i < res_tail.size(); i++) {
        res.append(res_tail.iloc(i));
    }
    return res;
}

template <class T>
Array<std::tuple<T>> concat_1(const Array<T>& ar)
{
    Array<std::tuple<T>> res;
    for (int i = 0; i < ar.size(); i++) {
        res.append(std::tuple(ar.iloc(i)));
    }
    return res;
}

template <class T, class... Ts>
Array<std::tuple<T, Ts...>> concat_1(const Array<T>& ar, const Array<Ts...>& ars)
{
    Array<std::tuple<T, Ts...>> res;
    Array<std::tuple<Ts...>> res_tail = concat_1(ars);

    for (int i = 0; i < res_tail.size(); i++) {
        const T& v = ar.iloc(i);
        std::tuple<Ts...> v_tail = res_tail.iloc(i);
        res.append(add_first_element(v, v_tail));
    }
    return res;
}

}