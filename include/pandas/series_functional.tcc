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
        if (v.isnan()) {
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
        if (res.isnan() || v > res) {
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
        if (res.isnan() || v < res) {
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
        if (!v.isnan()) {
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
        if (v.isnan()) {
            continue;
        }
        s += (v - mn) * (v - mn);
    }
    return s / count();
}
