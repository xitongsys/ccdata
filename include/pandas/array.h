#pragma once

#include <format>
#include <functional>
#include <initializer_list>
#include <iostream>
#include <list>
#include <map>
#include <optional>
#include <string>
#include <vector>

#include "pandas/ops.h"
#include "pandas/string.h"
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

    template <class T2, class NT2>
    Array(const Array<T2, NT2>& ar)
        : Array(ar.name)
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

    Array(std::vector<T>&& vals, const NT& name = NT {})
        : Array(name)
    {
        values = vals;
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

    template <class T2, class NT2>
    Array& operator=(const Array<T2, NT2>& ar)
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

    template <class NT2>
    void _rename(const NT2& name)
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

    void _clear()
    {
        values.clear();
    }

    template <class T2, class NT2>
    void _append(const Array<T2, NT2>& ar)
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

    Array<char, NT> duplicated(const std::string& keep)
    {
        Array<char, NT> dup;
        std::map<T, int> mp;
        if (keep == "first") {
            for (int i = 0; i < size(); i++) {
                T v = iloc(i);
                if (mp.count(v)) {
                    dup._append(true);
                } else {
                    dup._append(false);
                }
                mp[v] = i;
            }
        } else if (keep == "last") {
            for (int i = 0; i < size(); i++) {
                dup._append(false);
            }
            for (int i = size() - 1; i >= 0; i--) {
                T v = iloc(i);
                if (mp.count(v)) {
                    dup.iloc_ref(i) = true;
                } else {
                    dup.iloc_ref(i) = false;
                }
                mp[v] = i;
            }

        } else if (keep == "false") {
            for (int i = 0; i < size(); i++) {
                T v = iloc(i);
                dup._append(false);
                if (mp.count(v) == 0) {
                    mp[v] = 0;
                }
                mp[v]++;
            }
            for (int i = 0; i < size(); i++) {
                T v = iloc(i);
                if (mp.count(v) > 1) {
                    dup.iloc_ref(i) = true;
                }
            }
        } else {
            PANDAS_THROW(std::format("unknow dedup method: {}", keep));
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

    template <class T2>
    Array loc(const std::vector<T2>& mask)
    {
        if (mask.size() != size()) {
            PANDAS_THROW(std::format("size not match: {}!={}", mask.size(), size()));
        }

        Array ar(get_name());
        for (int i = 0; i < size(); i++) {
            if (mask[i]) {
                ar._append(iloc(i));
            }
        }
        return ar;
    }

    template <class T2, class NT2>
    Array loc(const Array<T2, NT2>& mask)
    {
        return loc(mask.values);
    }

    std::string to_string(int mx_cnt = 10) const
    {
        std::stringstream ss;

        ss << pandas::to_string(get_name()) << "\n";

        if (size() > mx_cnt) {
            for (int i = 0; i < mx_cnt / 2; i++) {
                ss << pandas::to_string(values[i]) << "\n";
            }
            ss << "...\n";
            for (int i = size() - mx_cnt / 2; i < size(); i++) {
                ss << pandas::to_string(values[i]) << "\n";
            }
        } else {
            for (int i = 0; i < size(); i++) {
                ss << pandas::to_string(values[i]) << "\n";
            }
        }

        ss << std::format("\n[{} rows]", size());

        return pandas::line_width_adjust(ss.str());
    }

    friend std::ostream& operator<<(std::ostream& os, const Array& ar)
    {
        os << ar.to_string();
        return os;
    }

#include "pandas/array_functional.tcc"
#include "pandas/array_op.tcc"
};

}