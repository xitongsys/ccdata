#pragma once

// template<class T, class NT>
// class Array {
template <class T2>
Array<T2, NT> map(std::function<T2(const T&)> const& func) const
{
    Array<T2, NT> ar(get_name());
    for (int i = 0; i < size(); i++) {
        T val = iloc(i);
        T2 mval = func(val);
        ar._append(mval);
    }
    return ar;
}
template <class T2>
Array& _map(std::function<T2(const T&)> const& func)
{
    for (int i = 0; i < size(); i++) {
        T val = iloc(i);
        T2 mval = func(val);
        iloc_ref(i) = mval;
    }
    return *this;
}

/// @diff
/// @param periods
Array& _diff(int periods = 1)
{
    if (periods < 0) {
        for (int i = 0; i < size(); i++) {
            int j = i - periods;
            if (j >= 0 && j < size()) {
                iloc_ref(i) = iloc(i) - iloc(j);
            } else {
                iloc_ref(i) = pandas::nan<T>();
            }
        }
    } else {
        for (int i = size() - 1; i >= 0; i--) {
            int j = i - periods;
            if (j >= 0 && j < size()) {
                iloc_ref(i) = iloc(i) - iloc(j);
            } else {
                iloc_ref(i) = pandas::nan<T>();
            }
        }
    }
    return *this;
}
Array diff(int periods = 1) const
{
    Array ar = *this;
    ar._diff(periods);
    return ar;
}

/// @shift
/// @param offset
Array& _shift(int offset)
{
    if (abs(offset) >= size()) {
        for (int i = 0; i < size(); i++) {
            iloc_ref(i) = pandas::nan<T>();
        }

    } else if (offset > 0) {
        for (int i = offset; i < size(); i++) {
            iloc_ref(i) = iloc(i - offset);
        }
        for (int i = 0; i < offset; i++) {
            iloc_ref(i) = pandas::nan<T>();
        }

    } else if (offset < 0) {
        for (int i = 0; i < size() + offset; i++) {
            iloc_ref(i) = iloc(i - offset);
        }
        for (int i = size() - offset; i < size(); i++) {
            iloc_ref(i) = pandas::nan<T>();
        }
    }
    return *this;
}
Array shift(int offset) const
{
    Array ar = *this;
    ar._shift(offset);
    return ar;
}

template <class T2>
Array& _fillna(const T2& v)
{
    for (int i = 0; i < size(); i++) {
        const T& val = iloc(i);
        if (isnan(val)) {
            iloc_ref(i) = v;
        }
    }
    return *this;
}
template <class T2>
Array fillna(const T2& v) const
{
    Array ar = *this;
    ar._fillna(v);
    return ar;
}

template <class T2>
Array& _ffill(const T2& v, int limit = 1)
{
    for (int i = size() - 1; i >= 0; i--) {
        if (!pandas::isnan(iloc(i))) {
            continue;
        }
        for (int j = i - 1; j >= i - limit && j >= 0; j--) {
            T v = iloc(j);
            if (!pandas::isnan(v)) {
                iloc_ref(i) = v;
                break;
            }
        }
    }
    return *this;
}
template <class T2>
Array ffill(const T2& v, int limit = 1) const
{
    Array ar = *this;
    ar._ffill(v, limit);
    return ar;
}

template <class T2>
Array& _bfill(const T2& v, int limit = 1)
{
    for (int i = 0; i < size(); i++) {
        if (!pandas::isnan(iloc(i))) {
            continue;
        }
        for (int j = i + 1; j <= i + limit && j < size(); j++) {
            T v = iloc(j);
            if (!pandas::isnan(v)) {
                iloc_ref(i) = v;
                break;
            }
        }
    }
    return *this;
}
template <class T2>
Array bfill(const T2& v, int limit = 1) const
{
    Array ar = *this;
    ar._bfill(v, limit);
    return ar;
}

/// @cumsum
Array& _cumsum()
{
    T s = 0;
    for (int i = 0; i < size(); i++) {
        if (!pandas::isnan(iloc(i))) {
            s = s + iloc(i);
            iloc_ref(i) = s;
        }
    }
    return *this;
}
Array cumsum() const
{
    Array ar = *this;
    ar._cumsum();
    return ar;
}

Array drop_duplicates(const std::string& keep)
{
    return loc(!duplicated(keep));
}

/// @pow
/// @param n
Array& _pow(double n)
{
    for (int i = 0; i < size(); i++) {
        T v = iloc(i);
        iloc_ref(i) = std::pow(v, n);
    }
    return *this;
}
Array pow(double n) const
{
    Array ar = *this;
    ar._pow(n);
    return ar;
}

/// @brief reciprocal
Array& _reciprocal()
{
    for (int i = 0; i < size(); i++) {
        iloc_ref(i) = 1 / iloc_ref(i);
    }
    return *this;
}
Array reciprocal() const
{
    Array ar = *this;
    ar._reciprocal();
    return ar;
}

/// @brief abs
Array& _abs()
{
    for (int i = 0; i < size(); i++) {
        iloc_ref(i) = std::abs(iloc_ref(i));
    }
    return *this;
}
Array abs() const
{
    Array ar = *this;
    ar._abs();
    return ar;
}

template <class DT2, class DT3>
Array& _replace(const DT2& v_old, const DT3& v_new)
{
    for (int i = 0; i < size(); i++) {
        if (iloc(i) == v_old) {
            iloc_ref(i) = v_new;
        }
    }
    return *this;
}
template <class DT2, class DT3>
Array replace(const DT2& v_old, const DT3& v_new) const
{
    Array ar = *this;
    ar._replace(v_old, v_new);
    return ar;
}

T sum() const
{
    T s(0);
    for (int i = 0; i < size(); i++) {
        const T& v = iloc(i);
        if (isnan(v)) {
            continue;
        }
        s += v;
    }
    return s;
}

T max() const
{
    if (size() == 0) {
        return pandas::nan<T>();
    }
    T res = iloc(0);
    for (int i = 0; i < size(); i++) {
        const T& v = iloc(i);
        if (isnan(res) || v > res) {
            res = v;
        }
    }
    return res;
}

T min() const
{
    if (size() == 0) {
        return pandas::nan<T>();
    }
    T res = iloc(0);
    for (int i = 0; i < size(); i++) {
        const T& v = iloc(i);
        if (isnan(res) || v < res) {
            res = v;
        }
    }
    return res;
}

double count() const
{
    double cnt = 0;
    for (int i = 0; i < size(); i++) {
        const T& v = iloc(i);
        if (!isnan(v)) {
            cnt += 1;
        }
    }
    return cnt;
}

double mean() const
{
    double s(sum());
    double mn = s / count();
    return mn;
}

double var(double ddof = 1) const
{
    double mn = mean();
    double s = 0, cnt = 0;
    for (int i = 0; i < size(); i++) {
        const double& v = iloc(i);
        if (isnan(v)) {
            continue;
        }
        s += (v - mn) * (v - mn);
        cnt++;
    }
    return s / (cnt - ddof);
}

double std(double ddof = 1) const
{
    double v = var(ddof);
    return std::pow(v, 0.5);
}

double median() const
{
    Array ar = sort();
    int n = ar.size();
    if (n == 0) {
        return pandas::nan<double>();
    }

    if (n % 2 == 0) {
        return (ar.iloc(n / 2) + ar.iloc(n / 2 - 1)) / 2;
    } else {
        return ar.iloc(n / 2);
    }
}

template <class T2>
double corr(const std::vector<T2>& y) const
{
    if (y.size() != size()) {
        PANDAS_THROW(std::format("size not match: {}!={}", y.size(), size()));
    }
    double sum_y = 0, sum_x = 0;
    for (int i = 0; i < y.size(); i++) {
        sum_y += y[i];
        sum_x += values[i];
    }

    double mn_x = sum_x / size(), mn_y = sum_y / size();
    double s_up = 0, s_down_x = 0, s_down_y = 0;
    for (int i = 0; i < size(); i++) {
        double xi = iloc(i);
        double yi = y[i];
        if (pandas::isnan<double>(xi) || pandas::isnan<double>(yi)) {
            continue;
        }
        s_up += (xi - mn_x) * (yi - mn_y);
        s_down_x += (xi - mn_x) * (xi - mn_x);
        s_down_y += (yi - mn_y) * (yi - mn_y);
    }
    double c = s_up / std::sqrt(s_down_x * s_down_y);
    return c;
}

template <class T2, class NT2>
double corr(const Array<T2, NT2>& ar) const
{
    if (ar.size() != size()) {
        PANDAS_THROW(std::format("size not match: {}!={}", ar.size(), size()));
    }
    return corr(ar.values);
}
