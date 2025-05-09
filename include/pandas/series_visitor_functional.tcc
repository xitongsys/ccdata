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
    DT s = 0;
    it.reset();
    while (it.has_left()) {
        DT v = sr.iloc(it.next());
        if (!pandas::isnan(v)) {
            s += v;
        }
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

double count()
{
    double cnt = 0;
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
    return sum() / count();
}

double var(double ddof = 1)
{
    double mn = mean();
    double s = 0, cnt = 0;
    it.reset();
    while (it.has_left()) {
        DT v = sr.iloc(it.next());
        if (!pandas::isnan(v)) {
            s += (v - mn) * (v - mn);
            cnt++;
        }
    }
    return s / (cnt - ddof);
}

double std(double ddof = 1)
{
    double v = var(ddof);
    return std::pow(v, 0.5);
}

double median()
{
    return to_array().median();
}