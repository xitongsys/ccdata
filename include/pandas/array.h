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

template <class T>
class Array {
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
            values.push_back((T)(ar.iloc(i)));
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

    T& iloc(int iid)
    {
        int n = values.size();
        iid = ((iid % n) + n) % n;
        return values[iid];
    }

    const T& iloc(int iid) const
    {
        int n = values.size();
        iid = ((iid % n) + n) % n;
        return values[iid];
    }

    void append(const T& v)
    {
        values.push_back(v);
    }

    template <class T2>
    void append(const Array<T2>& ar)
    {
        for (int i = 0; i < ar.size(); i++) {
            append((T)(ar.iloc(i)));
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

    Array<T> sort(bool ascending = true)
    {
        Array<T> ar = *this;
        sort(ar.values.begin(), ar.values.end());
        if (!ascending) {
            return ar.reverse();
        }
        return ar;
    }

    std::string to_string(int mx_cnt = 10) const
    {
        std::string vs = "";
        if (size() > mx_cnt) {
            for (int i = 0; i < mx_cnt / 2; i++) {
                vs += values[i].to_string() + ",";
            }
            vs += "...";
            for (int i = size() - mx_cnt / 2; i < size(); i++) {
                vs += values[i].to_string() + ",";
            }
        } else {
            for (int i = 0; i < size(); i++) {
                vs += values[i].to_string() + ",";
            }
        }

        std::string s = std::format("{}:[{}]", name, vs);
        return s;
    }

    friend std::ostream& operator<<(std::ostream& os, const Array& ar)
    {
        os << ar.to_string();
        return os;
    }

#include "pandas/array_op.tcc"
};

}