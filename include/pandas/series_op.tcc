#pragma once

// template<class IT, class DT>
// class Series {

#define DEFINE_SERIES_OPERATOR(OP)                                            \
    template <class T2>                                                       \
    Series operator OP(const T2& val) const                                   \
    {                                                                         \
        Series<IT, DT> res = *this;                                           \
        res.values = res.values OP val;                                       \
        return res;                                                           \
    }                                                                         \
                                                                              \
    template <class DT2, class DNT2>                                          \
    Series operator OP(const Array<DT2, DNT2>& ar) const                      \
    {                                                                         \
        if (size() != ar.size()) {                                            \
            throw std::format("size not match: {} != {}", ar.size(), size()); \
        }                                                                     \
        Series res = *this;                                                   \
        for (int i = 0; i < size(); i++) {                                    \
            res.iloc(i) OP ar.iloc(i);                                        \
        }                                                                     \
        return res;                                                           \
    }                                                                         \
                                                                              \
    template <class IT2, class DT2, class INT2, class DNT2>                   \
    Series operator OP(const Series<IT2, DT2, INT2, DNT2>& sr) const          \
    {                                                                         \
        SingleIndex<IT, INT> index;                                           \
        for (int i = 0; i < pidx->size(); i++) {                              \
            index._append(pidx->iloc(i));                                      \
        }                                                                     \
        for (int i = 0; i < sr.size(); i++) {                                 \
            index._append(sr.pidx->iloc(i));                                   \
        }                                                                     \
        auto sr1 = this->reindex(index);                                      \
        auto sr2 = sr.reindex(index);                                         \
        Array<DT> vals = sr1.values OP sr2.values;                            \
        return Series(index, vals);                                           \
    }

DEFINE_SERIES_OPERATOR(+)
DEFINE_SERIES_OPERATOR(-)
DEFINE_SERIES_OPERATOR(*)
DEFINE_SERIES_OPERATOR(/)
DEFINE_SERIES_OPERATOR(%)
DEFINE_SERIES_OPERATOR(&)
DEFINE_SERIES_OPERATOR(|)
DEFINE_SERIES_OPERATOR(^)

#define DEFINE_SERIES_OPERATOR(OP)                                          \
    template <class T>                                                      \
    Series& operator OP(T val)                                              \
    {                                                                       \
        values OP val;                                                      \
        return *this;                                                       \
    }                                                                       \
                                                                            \
    template <class DT2, class DNT2>                                        \
    Series& operator OP(const Array<DT2, DNT2>& ar)                         \
    {                                                                       \
        if (ar.size() != size()) {                                          \
            throw std::format("size not match: {}!={}", ar.size(), size()); \
        }                                                                   \
        for (int i = 0; i < size(); i++) {                                  \
            iloc(i) OP ar.iloc(i);                                          \
        }                                                                   \
        return *this;                                                       \
    }                                                                       \
                                                                            \
    template <class IT2, class DT2, class INT2, class DNT2>                 \
    Series& operator OP(const Series<IT2, DT2, INT2, DNT2>& sr)             \
    {                                                                       \
        for (int i = 0; i < size(); i++) {                                  \
            IT& id = pidx->iloc(i);                                         \
            DT& val = values.iloc(i);                                       \
            if (sr.pidx->has(id)) {                                         \
                val OP sr.loc(id);                                          \
            }                                                               \
        }                                                                   \
        return *this;                                                       \
    }

DEFINE_SERIES_OPERATOR(+=)
DEFINE_SERIES_OPERATOR(-=)
DEFINE_SERIES_OPERATOR(*=)
DEFINE_SERIES_OPERATOR(/=)
DEFINE_SERIES_OPERATOR(%=)
DEFINE_SERIES_OPERATOR(&=)
DEFINE_SERIES_OPERATOR(|=)
DEFINE_SERIES_OPERATOR(^=)

#define DEFINE_SERIES_OPERATOR(OP)           \
    Series operator OP() const               \
    {                                        \
        Series<IT, DT, INT, DT> res = *this; \
        res.values = OP res.values;          \
        return res;                          \
    }

DEFINE_SERIES_OPERATOR(~)

/////////// cmp operator /////////////////
#define DEFINE_SERIES_OPERATOR(OP)                                                       \
    template <class T2>                                                                  \
    Series<IT, bool, INT, DNT> operator OP(const T2 & val) const                         \
    {                                                                                    \
        Array<bool, DNT> values = this->values OP val;                                   \
        return Series(this->pidx, values);                                               \
    }                                                                                    \
                                                                                         \
    template <class DT2, class DNT2>                                                     \
    Series<IT, bool, INT, DNT> operator OP(const Array<DT2, DNT2>& ar) const             \
    {                                                                                    \
        if (ar.size() != size()) {                                                       \
            throw std::format("size not match: {}!={}", ar.size(), size());              \
        }                                                                                \
        Array<bool, DNT> vals = values OP ar;                                            \
        return Series(pidx, vals);                                                       \
    }                                                                                    \
                                                                                         \
    template <class IT2, class DT2, class INT2, class DNT2>                              \
    Series<IT, bool, INT, DNT> operator OP(const Series<IT2, DT2, INT2, DNT2>& sr) const \
    {                                                                                    \
        SingleIndex<IT, INT> index;                                                      \
        for (int i = 0; i < pidx->size(); i++) {                                         \
            index._append(pidx->iloc(i));                                                 \
        }                                                                                \
        for (int i = 0; i < sr.size(); i++) {                                            \
            index._append(sr.pidx->iloc(i));                                              \
        }                                                                                \
        Series<IT, DT, INT, DNT> sr1 = this->reindex(index);                             \
        Series<IT, DT2, INT, DNT2> sr2 = sr.reindex(index);                              \
        Array<bool, DNT> vals = sr1.values OP sr2.values;                                \
        return Series(index, vals);                                                      \
    }

DEFINE_SERIES_OPERATOR(>)
DEFINE_SERIES_OPERATOR(>=)
DEFINE_SERIES_OPERATOR(<)
DEFINE_SERIES_OPERATOR(<=)
DEFINE_SERIES_OPERATOR(==)
DEFINE_SERIES_OPERATOR(!=)