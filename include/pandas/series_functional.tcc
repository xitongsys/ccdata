#pragma once

// template<class IT, class DT>
// class Series {
template <class DT2>
Series<IT, DT2> map(std::function<DT2(const DT&)> const& func)
{
    Array<DT2> vals;
    for (int i = 0; i < size(); i++) {
        const DT& val = values.iloc(i);
        DT2 mval = func(val);
        vals.append(mval);
    }
    return Series(*pidx, vals);
}

template <class DT2>
Series<IT, DT2> map(std::function<DT2(const IT&, const DT&)> const& func)
{
    Array<DT2> vals;
    for (int i = 0; i < size(); i++) {
        const IT& id = pidx->iloc(i);
        const DT& val = values.iloc(i);
        DT2 mval = func(id, val);
        vals.append(mval);
    }
    return Series(*pidx, vals);
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
    DT res;
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
    DT res;
    for (int i = 0; i < size(); i++) {
        const DT& v = iloc(i);
        if (isnan(res) || v < res) {
            res = v;
        }
    }
    return res;
}

Int count() const
{
    Int cnt = 0;
    for (int i = 0; i < size(); i++) {
        const DT& v = iloc(i);
        if (!isnan(v)) {
            cnt += 1;
        }
    }
    return cnt;
}

Double mean() const
{
    Double s(sum());
    Double mn = s / count();
    return mn;
}

Double var() const
{
    Double mn = mean();
    Double s = 0;
    for (int i = 0; i < size(); i++) {
        const Double& v = iloc(i);
        if (isnan(v)) {
            continue;
        }
        s += (v - mn) * (v - mn);
    }
    return s / count();
}

Double std() const
{
    Double v = var();
    return v.pow(0.5);
}

Double median() const
{
    Series<IT,DT> sr = sort_values();
    int n = sr.size();
    if (n == 0) {
        return Double {};
    }

    if (n % 2 == 0) {
        return (sr.iloc(n / 2) + sr.iloc(n / 2 - 1)) / 2;
    } else {
        return sr.iloc(n / 2);
    }
}