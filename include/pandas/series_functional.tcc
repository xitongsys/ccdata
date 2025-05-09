#pragma once

// template<class IT, class DT, class INT, class DNT>
// class Series {
template <class DT2>
Series<IT, DT2, INT, DNT> map(std::function<DT2(const DT&)> const& func) const
{
    Array<IT, INT> ar_idx(pidx->get_name());
    Array<DT, DNT> ar_val(get_name());

    for (int i = 0; i < size(); i++) {
        const IT& id = pidx->iloc(i);
        const DT& val = values.iloc(i);
        DT2 mval = func(val);
        ar_idx._append(id);
        ar_val._append(mval);
    }
    return Series<IT, DT2, INT, DNT>(
        std::move(Index<IT, INT>(std::move(ar_idx))),
        std::move(ar_val));
}
template <class DT2>
void _map(std::function<DT2(const DT&)> const& func)
{
    for (int i = 0; i < size(); i++) {
        const IT& id = pidx->iloc(i);
        const DT& val = values.iloc(i);
        DT2 mval = func(val);
        iloc_ref(i) = mval;
    }
}

/// @dropna
Series dropna() const
{
    Array<IT, INT> ar_idx(pidx->get_name());
    Array<DT, DNT> ar_val(get_name());

    for (int i = 0; i < size(); i++) {
        DT v = iloc(i);
        if (!pandas::isnan(v)) {
            IT id = pidx->iloc(i);
            ar_idx._append(id);
            ar_val._append(v);
        }
    }
    return Series(
        std::move(Index<IT, INT>(std::move(ar_idx))),
        std::move(ar_val));
}
Series& _dropna()
{
    *this = dropna();
    return *this;
}

/// @diff
/// @param periods
Series& _diff(int periods = 1)
{
    values._diff(periods);
    return *this;
}
Series diff(int periods = 1) const
{
    Series sr = this->copy();
    sr._diff(periods);
    return sr;
}

/// @shift
/// @param offset
Series& _shift(int offset)
{
    values._shift(offset);
    return *this;
}
Series shift(int offset) const
{
    Series sr = this->copy();
    sr._shift(offset);
    return sr;
}

/// @fillna
/// @tparam DT2
/// @param v
template <class DT2>
Series& _fillna(const DT2& v)
{
    values._fillna(v);
    return *this;
}
template <class DT2>
Series fillna(const DT2& v) const
{
    Series sr = this->copy();
    sr._fillna(v);
    return sr;
}

/// @ffill
/// @tparam DT2
/// @param v
/// @param limit
template <class DT2>
Series& _ffill(const DT2& v, int limit = 1)
{
    values._ffill(v, limit);
    return *this;
}
template <class DT2>
Series ffill(const DT2& v, int limit = 1) const
{
    Series sr = this->copy();
    sr._ffill(v, limit);
    return sr;
}

/// @bfill
/// @tparam DT2
/// @param v
/// @param limit
template <class DT2>
Series& _bfill(const DT2& v, int limit = 1)
{
    values._bfill(v, limit);
    return *this;
}
template <class DT2>
Series bfill(const DT2& v, int limit = 1) const
{
    Series sr = this->copy();
    sr._bfill(v, limit);
    return sr;
}

/// @cumsum
Series& _cumsum()
{
    values._cumsum();
    return *this;
}
Series cumsum() const
{
    Series sr = this->copy();
    sr._cumsum();
    return sr;
}

/// @pow
/// @param n
Series& _pow(double n)
{
    values._pow(n);
    return *this;
}
Series pow(double n) const
{
    Series sr = this->copy();
    sr._pow(n);
    return sr;
}

/// @brief reciprocal
Series& _reciprocal()
{
    values._reciprocal();
    return *this;
}
Series reciprocal() const
{
    Series sr = this->copy();
    sr._reciprocal();
    return sr;
}

/// @brief abs
Series& _abs()
{
    values._abs();
    return *this;
}
Series abs() const
{
    Series sr = this->copy();
    sr._abs();
    return sr;
}

/// @replace
/// @tparam DT2
/// @tparam DT3
/// @param v_old
/// @param v_new
template <class DT2, class DT3>
Series& _replace(const DT2& v_old, const DT3& v_new)
{
    values._replace(v_old, v_new);
    return *this;
}
template <class DT2, class DT3>
Series replace(const DT2& v_old, const DT3& v_new) const
{
    Series ds = this->copy();
    ds._replace(v_old, v_new);
    return ds;
}

template <int level>
auto droplevel() const
{
    using IT2 = decltype(pidx->template droplevel<level>().iloc(0));
    auto idx = Index<IT2, INT>(pidx->template droplevel<level>());
    return Series<IT2, DT, INT, DNT>(idx, values);
}

/// @brief duplicated
/// @param keep  first/last/false
/// @return
Series<IT, char, INT, DNT> duplicated(const std::string& keep)
{
    return Series<IT, char, INT, DNT>(*pidx, values.duplicated(keep));
}

Series drop_duplicates(const std::string& keep)
{
    return loc(!duplicated(keep)).to_series();
}

DT sum() const
{
    return values.sum();
}

DT max() const
{
    return values.max();
}

DT min() const
{
    return values.min();
}

int count() const
{
    return values.count();
}

double mean() const
{
    return values.mean();
}

double var(double ddof = 1) const
{
    return values.var(ddof);
}

double std(double ddof = 1) const
{
    return values.std(ddof);
}

double median() const
{
    return values.median();
}

template <class DT2>
double corr(const std::vector<DT2>& y) const
{
    return values.corr(y);
}
template <class DT2, class DNT2>
double corr(const Array<DT2, DNT2>& y) const
{
    return corr(y.values);
}
template <class IT2, class DT2, class INT2, class DNT2>
double corr(const Series<IT2, DT2, INT2, DNT2>& y) const
{
    double mn_x = mean(), mn_y = y.mean();
    double s_up = 0, s_down_x = 0, s_down_y = 0;
    for (int i = 0; i < size(); i++) {
        IT id = pidx->iloc(i);
        double xi = iloc(i);
        double yi = y.loc(id);
        if (pandas::isnan<double>(xi) || pandas::isnan<double>(yi)) {
            continue;
        }
        s_up += (xi - mn_x) * (yi - mn_y);
        s_down_x += (xi - mn_x) * (xi - mn_x);
        s_down_y += (yi - mn_y) * (yi - mn_y);
    }
    double c = s_up / std::pow(s_down_x * s_down_y, 0.5);
    return c;
}