#pragma once

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
    std::vector<T> values;

    std::optional<std::map<T, int>> indexs;

    void create_index()
    {
        indexs = std::move(std::map<T, int>());
        update_index();
    }

    void clear_index()
    {
        indexs = std::nullopt;
    }

    void update_index()
    {
        if (!indexs.has_value()) {
            return;
        }

        auto& mp = indexs.value();
        mp.clear();
        for (int i = 0; i < size(); i++) {
            mp[values[i]] = i;
        }
    }

    Array()
    {
        name = "Array";
    }

    Array(const Array& ar)
    {
        name = ar.name;
        values = ar.values;
    }

    Array(Array&& ar)
    {
        name = std::move(ar.name);
        values = std::move(ar.values);
    }

    Array& operator=(const Array& ar)
    {
        name = ar.name;
        values = ar.values;
        return *this;
    }

    Array& operator=(Array&& ar)
    {
        name = std::move(ar.name);
        values = std::move(ar.values);
        return *this;
    }

    T& iloc(int iid)
    {
        int n = values.size();
        iid = ((iid % n) + n) % n;
        return values[iid];
    }

    T iloc(int iid) const
    {
        int n = values.size();
        iid = ((iid % n) + n) % n;
        return values[iid];
    }

    void append(const T& v)
    {
        values.push_back(v);
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

    Array<Int> operator>(Array ar) const
    {
        if (size() != ar.size()) {
            throw std::format("size not match: {}!={}", size(), ar.size());
        }

        Array<Int> res;

        for (int i = 0; i < size(); i++) {
            if (values[i] > ar.values[i]) {
                res.append(Int(1));
            } else {
                res.append(Int(0));
            }
        }
        return res;
    }

    Array<Int> operator>=(Array ar) const
    {
        if (size() != ar.size()) {
            throw std::format("size not match: {}!={}", size(), ar.size());
        }

        Array<Int> res;

        for (int i = 0; i < size(); i++) {
            if (values[i] >= ar.values[i]) {
                res.append(Int(1));
            } else {
                res.append(Int(0));
            }
        }
        return res;
    }

    Array<Int> operator<(Array ar) const
    {
        if (size() != ar.size()) {
            throw std::format("size not match: {}!={}", size(), ar.size());
        }

        Array<Int> res;

        for (int i = 0; i < size(); i++) {
            if (values[i] < ar.values[i]) {
                res.append(Int(1));
            } else {
                res.append(Int(0));
            }
        }
        return res;
    }

    Array<Int> operator<=(Array ar) const
    {
        if (size() != ar.size()) {
            throw std::format("size not match: {}!={}", size(), ar.size());
        }

        Array<Int> res;

        for (int i = 0; i < size(); i++) {
            if (values[i] <= ar.values[i]) {
                res.append(Int(1));
            } else {
                res.append(Int(0));
            }
        }
        return res;
    }

    Array<Int> operator==(Array ar) const
    {
        if (size() != ar.size()) {
            throw std::format("size not match: {}!={}", size(), ar.size());
        }

        Array<Int> res;

        for (int i = 0; i < size(); i++) {
            if (values[i] == ar.values[i]) {
                res.append(Int(1));
            } else {
                res.append(Int(0));
            }
        }
        return res;
    }

    Array operator&(Array ar) const
    {
        if (size() != ar.size()) {
            throw std::format("size not match: {}!={}", size(), ar.size());
        }

        Array<T> res = *this;

        for (int i = 0; i < size(); i++) {
            res.values[i] &= ar.values[i];
        }
        return res;
    }

    Array operator&=(Array ar)
    {
        if (size() != ar.size()) {
            throw std::format("size not match: {}!={}", size(), ar.size());
        }

        for (int i = 0; i < size(); i++) {
            values[i] &= ar.values[i];
        }
        return *this;
    }

    Array operator|(Array ar) const
    {
        if (size() != ar.size()) {
            throw std::format("size not match: {}!={}", size(), ar.size());
        }

        Array<T> res = *this;

        for (int i = 0; i < size(); i++) {
            res.values[i] |= ar.values[i];
        }
        return res;
    }

    Array operator|=(Array ar)
    {
        if (size() != ar.size()) {
            throw std::format("size not match: {}!={}", size(), ar.size());
        }

        for (int i = 0; i < size(); i++) {
            values[i] |= ar.values[i];
        }
        return *this;
    }

    Array operator^(Array ar) const
    {
        if (size() != ar.size()) {
            throw std::format("size not match: {}!={}", size(), ar.size());
        }

        Array<T> res = *this;

        for (int i = 0; i < size(); i++) {
            res.values[i] ^= ar.values[i];
        }
        return res;
    }

    Array operator^=(Array ar)
    {
        if (size() != ar.size()) {
            throw std::format("size not match: {}!={}", size(), ar.size());
        }

        for (int i = 0; i < size(); i++) {
            values[i] ^= ar.values[i];
        }
        return *this;
    }

    Array operator~() const
    {
        Array<T> res = *this;

        for (int i = 0; i < size(); i++) {
            res.values[i] = ~values[i];
        }
        return res;
    }

    Array operator+(Array ar) const
    {
        Array<T> res = *this;
        if (size() != ar.size()) {
            throw std::format("size not match: {}!={}", size(), ar.size());
        }

        for (int i = 0; i < size(); i++) {
            res.values[i] += ar.values[i];
        }
        return res;
    }

    Array operator+=(Array ar)
    {
        if (size() != ar.size()) {
            throw std::format("size not match: {}!={}", size(), ar.size());
        }

        for (int i = 0; i < size(); i++) {
            values[i] += ar.values[i];
        }
        return *this;
    }

    Array operator-(Array ar) const
    {
        Array<T> res = *this;
        if (size() != ar.size()) {
            throw std::format("size not match: {}!={}", size(), ar.size());
        }

        for (int i = 0; i < size(); i++) {
            res.values[i] -= ar.values[i];
        }
        return res;
    }

    Array operator-=(Array ar)
    {
        if (size() != ar.size()) {
            throw std::format("size not match: {}!={}", size(), ar.size());
        }

        for (int i = 0; i < size(); i++) {
            values[i] -= ar.values[i];
        }
        return *this;
    }

    Array operator*(Array ar) const
    {
        Array<T> res = *this;
        if (size() != ar.size()) {
            throw std::format("size not match: {}!={}", size(), ar.size());
        }

        for (int i = 0; i < size(); i++) {
            res.values[i] *= ar.values[i];
        }
        return res;
    }

    Array operator*=(Array ar)
    {
        if (size() != ar.size()) {
            throw std::format("size not match: {}!={}", size(), ar.size());
        }

        for (int i = 0; i < size(); i++) {
            values[i] *= ar.values[i];
        }
        return *this;
    }

    Array operator/(Array ar) const
    {
        Array<T> res = *this;
        if (size() != ar.size()) {
            throw std::format("size not match: {}!={}", size(), ar.size());
        }

        for (int i = 0; i < size(); i++) {
            res.values[i] /= ar.values[i];
        }
        return res;
    }

    Array operator/=(Array ar)
    {
        if (size() != ar.size()) {
            throw std::format("size not match: {}!={}", size(), ar.size());
        }

        for (int i = 0; i < size(); i++) {
            values[i] /= ar.values[i];
        }
        return *this;
    }

    Array operator%(Array ar) const
    {
        Array<T> res = *this;
        if (size() != ar.size()) {
            throw std::format("size not match: {}!={}", size(), ar.size());
        }

        for (int i = 0; i < size(); i++) {
            res.values[i] %= ar.values[i];
        }
        return res;
    }

    Array operator%=(Array ar)
    {
        if (size() != ar.size()) {
            throw std::format("size not match: {}!={}", size(), ar.size());
        }

        for (int i = 0; i < size(); i++) {
            values[i] %= ar.values[i];
        }
        return *this;
    }

    template <class T2>
    Array<Int> operator>(T2 val) const
    {
        Array<Int> res;
        for (auto& v : values) {
            if (v > val) {
                res.append(Int(1));
            } else {
                res.append(Int(0));
            }
        }
        return res;
    }

    template <class T2>
    Array<Int> operator>=(T2 val) const
    {
        Array<Int> res;
        for (auto& v : values) {
            if (v >= val) {
                res.append(Int(1));
            } else {
                res.append(Int(0));
            }
        }
        return res;
    }

    template <class T2>
    Array<Int> operator<(T2 val) const
    {
        Array<Int> res;
        for (auto& v : values) {
            if (v < val) {
                res.append(Int(1));
            } else {
                res.append(Int(0));
            }
        }
        return res;
    }

    template <class T2>
    Array<Int> operator<=(T2 val) const
    {
        Array<Int> res;
        for (auto& v : values) {
            if (v <= val) {
                res.append(Int(1));
            } else {
                res.append(Int(0));
            }
        }
        return res;
    }

    template <class T2>
    Array<Int> operator!=(T2 val) const
    {
        Array<Int> res;
        for (auto& v : values) {
            if (v != val) {
                res.append(Int(1));
            } else {
                res.append(Int(0));
            }
        }
        return res;
    }

    template <class T2>
    Array<Int> operator==(T2 val) const
    {
        Array<Int> res;
        for (auto& v : values) {
            if (v == val) {
                res.append(Int(1));
            } else {
                res.append(Int(0));
            }
        }
        return res;
    }

    template <class T2>
    Array operator&(T2 val) const
    {
        Array<T> res = *this;
        for (auto& v : res.values) {
            v &= val;
        }
        return res;
    }

    template <class T2>
    Array operator&=(T2 val)
    {
        for (auto& v : values) {
            v &= val;
        }
        return *this;
    }

    template <class T2>
    Array operator|(T2 val) const
    {
        Array<T> res = *this;
        for (auto& v : res.values) {
            v |= val;
        }
        return res;
    }

    template <class T2>
    Array operator|=(T2 val)
    {
        for (auto& v : values) {
            v |= val;
        }
        return *this;
    }

    template <class T2>
    Array operator^(T2 val) const
    {
        Array<T> res = *this;
        for (auto& v : res.values) {
            v ^= val;
        }
        return res;
    }

    template <class T2>
    Array operator^=(T2 val)
    {
        for (auto& v : values) {
            v ^= val;
        }
        return *this;
    }

    template <class T2>
    Array operator+(T2 val) const
    {
        Array<T> res = *this;
        for (auto& v : res.values) {
            v += val;
        }
        return res;
    }

    template <class T2>
    Array operator+=(T2 val)
    {
        for (auto& v : values) {
            v += val;
        }
        return *this;
    }

    template <class T2>
    Array operator-(T2 val) const
    {
        Array<T> res = *this;
        for (auto& v : res.values) {
            v -= val;
        }
        return res;
    }

    template <class T2>
    Array operator-=(T2 val)
    {
        for (auto& v : values) {
            v -= val;
        }
        return *this;
    }

    template <class T2>
    Array operator*(T2 val) const
    {
        Array<T> res = *this;
        for (auto& v : res.values) {
            v *= val;
        }
        return res;
    }

    template <class T2>
    Array operator*=(T2 val)
    {
        for (auto& v : values) {
            v *= val;
        }
        return *this;
    }

    template <class T2>
    Array operator/(T2 val) const
    {
        Array<T> res = *this;
        for (auto& v : res.values) {
            v /= val;
        }
        return res;
    }

    template <class T2>
    Array operator/=(T2 val)
    {
        for (auto& v : values) {
            v /= val;
        }
        return *this;
    }

    template <class T2>
    Array operator%(T2 val) const
    {
        Array<T> res = *this;
        for (auto& v : res.values) {
            v %= val;
        }
        return res;
    }

    template <class T2>
    Array operator%=(T2 val)
    {
        for (auto& v : values) {
            v %= val;
        }
        return *this;
    }

    size_t size() const
    {
        return values.size();
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

    class ArrayPick {
    public:
        Array& ar;
        std::vector<int> ids;

        ArrayPick(Array& ar_, const std::vector<int>& ids_)
            : ar(ar_)
            , ids(ids_)
        {
        }

        size_t size() const
        {
            return ids.size();
        }

        void operator=(const T& v)
        {
            for (int i : ids) {
                ar.iloc(i) = v;
            }
        }

        void operator=(const Array& ar_)
        {
            for (int i = 0; i < ids.size() && i < ar_.size(); i++) {
                ar.iloc(ids[i]) = ar_.iloc(i);
            }
        }

        void operator=(const ArrayPick& ap)
        {
            for (int i = 0; i < ids.size() && i < ap.size(); i++) {
                ar.iloc(ids[i]) = ap.ar.iloc(ap.ids[i]);
            }
        }

        Array operator&(Array ar) const
        {
            Array res = *this;
            if (size() != ar.size()) {
                throw std::format("size not match: {}!={}", size(), ar.size());
            }

            for (int i = 0; i < size(); i++) {
                res.values[i] &= ar.values[i];
            }
            return res;
        }

        void operator&=(Array br)
        {
            if (size() != br.size()) {
                throw std::format("size not match: {}!={}", size(), br.size());
            }

            for (int i = 0; i < size(); i++) {
                ar.iloc(ids[i]) &= br.iloc(i);
            }
        }

        Array operator|(Array ar) const
        {
            Array res = *this;
            if (size() != ar.size()) {
                throw std::format("size not match: {}!={}", size(), ar.size());
            }

            for (int i = 0; i < size(); i++) {
                res.values[i] |= ar.values[i];
            }
            return res;
        }

        void operator|=(Array br)
        {
            if (size() != br.size()) {
                throw std::format("size not match: {}!={}", size(), br.size());
            }

            for (int i = 0; i < size(); i++) {
                ar.iloc(ids[i]) |= br.iloc(i);
            }
        }

        Array operator^(Array ar) const
        {
            Array res = *this;
            if (size() != ar.size()) {
                throw std::format("size not match: {}!={}", size(), ar.size());
            }

            for (int i = 0; i < size(); i++) {
                res.values[i] ^= ar.values[i];
            }
            return res;
        }

        void operator^=(Array br)
        {
            if (size() != br.size()) {
                throw std::format("size not match: {}!={}", size(), br.size());
            }

            for (int i = 0; i < size(); i++) {
                ar.iloc(ids[i]) ^= br.iloc(i);
            }
        }

        Array operator~() const
        {
            Array res = *this;
            if (size() != ar.size()) {
                throw std::format("size not match: {}!={}", size(), ar.size());
            }

            for (int i = 0; i < size(); i++) {
                res.values[i] = ~ar.values[i];
            }
            return res;
        }

        Array operator+(Array ar) const
        {
            Array res = *this;
            if (size() != ar.size()) {
                throw std::format("size not match: {}!={}", size(), ar.size());
            }

            for (int i = 0; i < size(); i++) {
                res.values[i] += ar.values[i];
            }
            return res;
        }

        void operator+=(Array br)
        {
            if (size() != br.size()) {
                throw std::format("size not match: {}!={}", size(), br.size());
            }

            for (int i = 0; i < size(); i++) {
                ar.iloc(ids[i]) += br.iloc(i);
            }
        }

        Array operator-(Array ar) const
        {
            Array res = *this;
            if (size() != ar.size()) {
                throw std::format("size not match: {}!={}", size(), ar.size());
            }

            for (int i = 0; i < size(); i++) {
                res.values[i] -= ar.values[i];
            }
            return res;
        }

        void operator-=(Array br)
        {
            if (size() != br.size()) {
                throw std::format("size not match: {}!={}", size(), br.size());
            }

            for (int i = 0; i < size(); i++) {
                ar.iloc(ids[i]) -= br.iloc(i);
            }
        }

        Array operator*(Array ar) const
        {
            Array res = *this;
            if (size() != ar.size()) {
                throw std::format("size not match: {}!={}", size(), ar.size());
            }

            for (int i = 0; i < size(); i++) {
                res.values[i] *= ar.values[i];
            }
            return res;
        }

        void operator*=(Array br)
        {
            if (size() != br.size()) {
                throw std::format("size not match: {}!={}", size(), br.size());
            }

            for (int i = 0; i < size(); i++) {
                ar.iloc(ids[i]) *= br.iloc(i);
            }
        }

        Array operator/(Array ar) const
        {
            Array res = *this;
            if (size() != ar.size()) {
                throw std::format("size not match: {}!={}", size(), ar.size());
            }

            for (int i = 0; i < size(); i++) {
                res.values[i] /= ar.values[i];
            }
            return res;
        }

        void operator/=(Array br)
        {
            if (size() != br.size()) {
                throw std::format("size not match: {}!={}", size(), br.size());
            }

            for (int i = 0; i < size(); i++) {
                ar.iloc(ids[i]) /= br.iloc(i);
            }
        }

        Array operator%(Array ar) const
        {
            Array res = *this;
            if (size() != ar.size()) {
                throw std::format("size not match: {}!={}", size(), ar.size());
            }

            for (int i = 0; i < size(); i++) {
                res.values[i] %= ar.values[i];
            }
            return res;
        }

        void operator%=(Array br)
        {
            if (size() != br.size()) {
                throw std::format("size not match: {}!={}", size(), br.size());
            }

            for (int i = 0; i < size(); i++) {
                ar.iloc(ids[i]) %= br.iloc(i);
            }
        }

        template <class T2>
        Array operator&(T2 val) const
        {
            Array<T> res = *this;
            for (auto& v : res.values) {
                v &= val;
            }
            return res;
        }

        template <class T2>
        void operator&=(T2 val)
        {
            for (int id : ids) {
                ar.iloc(id) &= val;
            }
        }

        template <class T2>
        Array operator|(T2 val) const
        {
            Array<T> res = *this;
            for (auto& v : res.values) {
                v |= val;
            }
            return res;
        }

        template <class T2>
        void operator|=(T2 val)
        {
            for (int id : ids) {
                ar.iloc(id) |= val;
            }
        }

        template <class T2>
        Array operator^(T2 val) const
        {
            Array<T> res = *this;
            for (auto& v : res.values) {
                v ^= val;
            }
            return res;
        }

        template <class T2>
        void operator^=(T2 val)
        {
            for (int id : ids) {
                ar.iloc(id) ^= val;
            }
        }

        template <class T2>
        Array operator+(T2 val) const
        {
            Array<T> res = *this;
            for (auto& v : res.values) {
                v += val;
            }
            return res;
        }

        template <class T2>
        void operator+=(T2 val)
        {
            for (int id : ids) {
                ar.iloc(id) += val;
            }
        }

        template <class T2>
        Array operator-(T2 val) const
        {
            Array<T> res = *this;
            for (auto& v : res.values) {
                v -= val;
            }
            return res;
        }

        template <class T2>
        void operator-=(T2 val)
        {
            for (int id : ids) {
                ar.iloc(id) -= val;
            }
        }

        template <class T2>
        Array operator*(T2 val) const
        {
            Array<T> res = *this;
            for (auto& v : res.values) {
                v *= val;
            }
            return res;
        }

        template <class T2>
        void operator*=(T2 val)
        {
            for (int id : ids) {
                ar.iloc(id) *= val;
            }
        }

        template <class T2>
        Array operator/(T2 val) const
        {
            Array<T> res = *this;
            for (auto& v : res.values) {
                v /= val;
            }
            return res;
        }

        template <class T2>
        void operator/=(T2 val)
        {
            for (int id : ids) {
                ar.iloc(id) /= val;
            }
        }

        template <class T2>
        Array operator%(T2 val) const
        {
            Array<T> res = *this;
            for (auto& v : res.values) {
                v %= val;
            }
            return res;
        }

        template <class T2>
        void operator%=(T2 val)
        {
            for (int id : ids) {
                ar.iloc(id) %= val;
            }
        }
    };

    Array(const ArrayPick& ap)
    {
        name = ap.ar.name;
        for (int id : ap.ids) {
            values.push_back(ap.ar.iloc(id));
        }
    }

    Array& operator=(const ArrayPick& ap)
    {
        name = ap.ar.name;
        for (int id : ap.ids) {
            values.push_back(ap.ar.iloc(id));
        }
        return *this;
    }

    ArrayPick iloc(const std::vector<int>& ids)
    {
        return ArrayPick(*this, ids);
    }

    ArrayPick iloc(int bgn, int end, int step = 1)
    {
        return iloc(range(bgn, end, step));
    }

    ArrayPick loc(const T& v)
    {
        std::vector<int> ids;
        if (indexs.has_value()) {
            auto& mp = indexs.value();
            ids.push_back(mp[v]);
        } else {
            for (int i = 0; i < size(); i++) {
                if (values[i] == v) {
                    ids.push_back(i);
                }
            }
        }
        return ArrayPick(*this, ids);
    }

    ArrayPick loc(const T& bgn, const T& end)
    {
        std::vector<int> ids;
        if (indexs.has_value()) {
            auto& mp = indexs.value();
            auto it = mp.lower_bound(bgn);
            while (it != mp.end() && it->first < end) {
                ids.push_back(it->second);
                it++;
            }

        } else {
            for (int i = 0; i < size(); i++) {
                auto& v = values[i];
                if (v >= bgn && v < end) {
                    ids.push_back(i);
                }
            }
        }
        return ArrayPick(*this, ids);
    }

    ArrayPick loc(const Array<Int>& mask)
    {
        std::vector<int> ids;
        if (size() != mask.size()) {
            throw std::format("size not match: {}!={}", size(), mask.size());
        }

        for (int i = 0; i < size(); i++) {
            if (mask.iloc(i) == Int(1)) {
                ids.push_back(i);
            }
        }

        return ArrayPick(*this, ids);
    }
};

}