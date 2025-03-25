#pragma once

// template<class IT, class DT, class INT, class DNT>
// class Series {
// class SeriesVisitor

template <class DT2>
Series<IT, DT2, INT, DNT> map(std::function<DT2(const DT&)> const& func)
{
    Series sr = to_series();
    return sr.map(func);
}

DT sum()
{
    DT s(0);
    it.reset();
    while (it.has_left()) {
        s += sr.iloc(it.next());
    }
    return s;
}

DT max()
{
    DT res = pandas::nan<DT>();
    it.reset();
    while (it.has_left()) {
        DT v = sr.iloc(it.next());
        if (pandas::isnan(res) || v > res) {
            res = v;
        }
    }
    return res;
}

DT min()
{
    DT res = pandas::nan<DT>();
    it.reset();
    while (it.has_left()) {
        DT v = sr.iloc(it.next());
        if (pandas::isnan(res) || v < res) {
            res = v;
        }
    }
    return res;
}

int count()
{
    int cnt = 0;
    it.reset();
    while (it.has_left()) {
        DT v = sr.iloc(it.next());
        if (!pandas::isnan(v)) {
            cnt++;
        }
    }
    return cnt;
}

double mean()
{
    double s(sum());
    double mn = s / count();
    return mn;
}

double var()
{
    double mn = mean();
    double s = 0;
    it.reset();
    while (it.has_left()) {
        DT v = sr.iloc(it.next());
        s += (v - mn) * (v - mn);
    }
    return s / count();
}

double std()
{
    double v = var();
    return std::pow(v, 0.5);
}

double median()
{
    return to_series().median();
}