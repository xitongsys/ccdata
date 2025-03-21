#pragma once

// template<class IT, class DT, class INT, class DNT>
// class Series {
template <class DT2>
Series<IT, DT2, INT, DNT> map(std::function<DT2(const DT&)> const& func) const
{
    Series<IT, DT2, INT, DNT> sr;
    for (int i = 0; i < size(); i++) {
        const IT& id = pidx->iloc(i);
        const DT& val = values.iloc(i);
        DT2 mval = func(val);
        sr._append(id, mval);
    }
    sr._rename(this->get_name());
    return sr;
}

/// @dropna
Series dropna()
{
    Series sr(get_name());
    for (int i = 0; i < size(); i++) {
        DT v = iloc(i);
        if (!pandas::isnan(v)) {
            IT id = pidx->iloc(i);
            sr._append(id, v);
        }
    }
    return sr;
}
void _dropna()
{
    *this = dropna();
}

/// @diff
/// @param periods
void _diff(int periods)
{
    if (periods < 0) {
        for (int i = 0; i < size(); i++) {
            int j = i - periods;
            if (j >= 0 && j < size()) {
                iloc_ref(i) = iloc(i) - iloc(j);
            } else {
                iloc_ref(i) = pandas::nan<DT>();
            }
        }
    } else {
        for (int i = size() - 1; i >= 0; i--) {
            int j = i - periods;
            if (j >= 0 && j < size()) {
                iloc_ref(i) = iloc(i) - iloc(j);
            } else {
                iloc_ref(i) = pandas::nan<DT>();
            }
        }
    }
}
Series diff(int periods) const
{
    Series sr = *this;
    sr._diff(periods);
    return sr;
}

/// @shift
/// @param offset
void _shift(int offset)
{
    if (abs(offset) >= size()) {
        for (int i = 0; i < size(); i++) {
            iloc_ref(i) = pandas::nan<DT>();
        }

    } else if (offset > 0) {
        for (int i = offset; i < size(); i++) {
            iloc_ref(i) = iloc(i - offset);
        }
        for (int i = 0; i < offset; i++) {
            iloc_ref(i) = pandas::nan<DT>();
        }

    } else if (offset < 0) {
        for (int i = 0; i < size() + offset; i++) {
            iloc_ref(i) = iloc(i - offset);
        }
        for (int i = size() - offset; i < size(); i++) {
            iloc_ref(i) = pandas::nan<DT>();
        }
    }
}
Series shift(int offset) const
{
    Series sr = *this;
    sr._shift(offset);
    return sr;
}

/// @fillna
/// @tparam DT2
/// @param v
template <class DT2>
void _fillna(const DT2& v)
{
    for (int i = 0; i < size(); i++) {
        const IT& id = pidx->iloc(i);
        const DT& val = values.iloc(i);
        if (isnan(val)) {
            iloc(i) = v;
        }
    }
}
template <class DT2>
Series fillna(const DT2& v)
{
    Series sr = *this;
    sr._fillna(v);
    return sr;
}

/// @ffill
/// @tparam DT2
/// @param v
/// @param limit
template <class DT2>
void _ffill(const DT2& v, int limit = 1)
{
    for (int i = size() - 1; i >= 0; i--) {
        if (!pandas::isnan(iloc(i))) {
            continue;
        }
        for (int j = i - 1; j >= i - limit && j >= 0; j--) {
            DT v = iloc(j);
            if (!pandas::isnan(v)) {
                iloc_ref(i) = v;
                break;
            }
        }
    }
}
template <class DT2>
Series ffill(const DT2& v, int limit = 1) const
{
    Series sr = *this;
    sr._ffill(v, limit);
    return sr;
}

/// @bfill
/// @tparam DT2
/// @param v
/// @param limit
template <class DT2>
void _bfill(const DT2& v, int limit = 1)
{
    for (int i = 0; i < size(); i++) {
        if (!pandas::isnan(iloc(i))) {
            continue;
        }
        for (int j = i + 1; j <= i + limit && j < size(); j++) {
            DT v = iloc(j);
            if (!pandas::isnan(v)) {
                iloc_ref(i) = v;
                break;
            }
        }
    }
}
template <class DT2>
Series bfill(const DT2& v, int limit = 1) const
{
    Series sr = *this;
    sr._bfill(v, limit);
    return sr;
}

/// @cumsum
void _cumsum()
{
    DT s = 0;
    for (int i = 0; i < size(); i++) {
        if (!pandas::isnan(iloc(i))) {
            s = s + iloc(i);
            iloc_ref(i) = s;
        }
    }
}
Series cumsum()
{
    Series sr = *this;
    sr._cumsum();
    return sr;
}

/// @pow
/// @param n
void _pow(double n)
{
    for (int i = 0; i < size(); i++) {
        DT v = iloc(i);
        iloc_ref(i) = std::pow(v, n);
    }
}
Series pow(double n)
{
    Series sr = *this;
    sr._pow(n);
    return sr;
}

/// @replace
/// @tparam DT2
/// @tparam DT3
/// @param v_old
/// @param v_new
template <class DT2, class DT3>
void _replace(const DT2& v_old, const DT3& v_new)
{
    for (int i = 0; i < size(); i++) {
        if (iloc(i) == v_old) {
            iloc_ref(i) = v_new;
        }
    }
}
template <class DT2, class DT3>
Series replace(const DT2& v_old, const DT3& v_new)
{
    Series ds = *this;
    ds._replace(v_old, v_new);
    return ds;
}

DT sum() const
{
    DT s(0);
    for (int i = 0; i < size(); i++) {
        const DT& v = iloc(i);
        if (isnan(v)) {
            continue;
        }
        s += v;
    }
    return s;
}

DT max() const
{
    DT res = pandas::nan<DT>();
    for (int i = 0; i < size(); i++) {
        const DT& v = iloc(i);
        if (isnan(res) || v > res) {
            res = v;
        }
    }
    return res;
}

DT min() const
{
    DT res = pandas::nan<DT>();
    for (int i = 0; i < size(); i++) {
        const DT& v = iloc(i);
        if (isnan(res) || v < res) {
            res = v;
        }
    }
    return res;
}

int count() const
{
    int cnt = 0;
    for (int i = 0; i < size(); i++) {
        const DT& v = iloc(i);
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

double var() const
{
    double mn = mean();
    double s = 0;
    for (int i = 0; i < size(); i++) {
        const double& v = iloc(i);
        if (isnan(v)) {
            continue;
        }
        s += (v - mn) * (v - mn);
    }
    return s / count();
}

double std() const
{
    double v = var();
    return v.pow(0.5);
}

double median() const
{
    Series<IT, DT> sr = sort_values();
    int n = sr.size();
    if (n == 0) {
        return pandas::nan<double>();
    }

    if (n % 2 == 0) {
        return (sr.iloc(n / 2) + sr.iloc(n / 2 - 1)) / 2;
    } else {
        return sr.iloc(n / 2);
    }
}