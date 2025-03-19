#pragma once

// template<class IT, class DT, class INT, class DNT>
// class Series {
template <class DT2>
Series<IT, DT2, INT, DNT> map(std::function<DT2(const DT&)> const& func)
{
    Series<IT, DT2, INT, DNT> sr;
    for (int i = 0; i < size(); i++) {
        const IT& id = pidx->iloc(i);
        const DT& val = values.iloc(i);
        DT2 mval = func(val);
        sr._append(id, val);
        }
    sr._rename(this->get_name());
    return sr;
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