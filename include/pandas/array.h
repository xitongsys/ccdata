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

    template <class RT>
    Array map(std::function<RT(const T&)> const& func)
    {
        Array<RT> res;
        res.name = name;
        for (int i = 0; i < size(); i++) {
            res.append(func(values[i]));
        }
        return res;
    }

    template <class RT>
    RT agg(std::function<RT(const Array&)> const& func)
    {
        return func(*this);
    }

    T max() const
    {
        T res;
        for (auto& v : values) {
            if (res.is_nan || v > res) {
                res = v;
            }
        }
        return res;
    }

    T min() const
    {
        T res;
        for (auto& v : values) {
            if (res.is_nan || v < res) {
                res = v;
            }
        }
        return res;
    }

    long long count() const
    {
        long long cnt = 0;
        for (auto& v : values) {
            cnt += v.is_nan;
        }
        return cnt;
    }

    T sum() const
    {
        T s = 0;
        for (auto& v : values) {
            s = s + v;
        }
        return s;
    }

    Double mean() const
    {
        Double c(double(count()));
        Double s = sum().astype<Double>();
        return s / c;
    }

    Double var() const
    {
        double s = 0;
        double mn = mean().value;
        for (auto& v : values) {
            double vv = double(v.value);
            s += (vv - s) * (vv - s);
        }
        double v = s / count();
        return Double(v);
    }

    Double std() const
    {
        Double v = var();
        return v.pow(0.5);
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

    template <class KT>
    class ArrayGroup {
    public:
        std::map<KT, Array> ars;

        ArrayGroup() { }

        ArrayGroup(const ArrayGroup& ag)
        {
            ars = ag.ars;
        }

        ArrayGroup(ArrayGroup&& ag)
        {
            ars = std::move(ag.ars);
        }

        void append(const KT& key, const T& value)
        {
            if (ars.count(key) == 0) {
                ars[key] = Array();
            }
            ars[key].append(value);
        }

        std::tuple<Array<KT>, Array<Int>> count()
        {
            return agg<Int>([](const Array& ar) -> Int { return Int(ar.size()); });
        }

        std::tuple<Array<KT>, Array> sum()
        {
            return agg<T>([](const Array& ar) -> Int { return ar.sum(); });
        }

        template <class RT>
        std::tuple<Array<KT>, Array<RT>> agg(std::function<RT(const Array&)> const& func)
        {
            Array<KT> keys;
            Array<RT> values;

            for (auto it = ars.begin(); it != ars.end(); it++) {
                KT key = it->first;
                Array ar = it->second;
                RT value = func(ar);
                keys.append(key);
                values.append(value);
            }

            return std::make_tuple<>(keys, values);
        }
    };

    template <class KT>
    ArrayGroup<KT> groupby(const Array<KT>& keys) const
    {
        if (keys.size() != size()) {
            throw std::format("size not match: key size {}!={}", size(), keys.size());
        }

        ArrayGroup<KT> ag;
        for (int i = 0; i < size(); i++) {
            ag.append(keys.iloc(i), iloc(i));
        }
        return ag;
    }

    friend std::ostream& operator<<(std::ostream& os, const Array& ar)
    {
        os << ar.to_string();
        return os;
    }

#include "pandas/array_op.tcc"
};

}