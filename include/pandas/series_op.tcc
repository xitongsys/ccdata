#pragma once

namespace pandas {

///////////////////////// single variable operator ///////////////////////

// 2
#define DEFINE_OPERATOR_2(_OP)           \
    template <class T>                   \
    Series operator_OP(T val) const      \
    {                                    \
        Series<T> res = *this;           \
        res.values = res.values _OP val; \
        return res;                      \
    }

DEFINE_OPERATOR_2(+)
DEFINE_OPERATOR_2(-)
DEFINE_OPERATOR_2(*)
DEFINE_OPERATOR_2(/)
DEFINE_OPERATOR_2(%)
DEFINE_OPERATOR_2(&)
DEFINE_OPERATOR_2(|)
DEFINE_OPERATOR_2(^)

// 2 inplace
#define DEFINE_OPERATOR_2_INPLACE(_OP) \
    template <class T>                 \
    Series operator_OP(T val)          \
    {                                  \
        values = values _OP val;       \
        return *this;                  \
    }

DEFINE_OPERATOR_2_INPLACE(+=)
DEFINE_OPERATOR_2_INPLACE(-=)
DEFINE_OPERATOR_2_INPLACE(*=)
DEFINE_OPERATOR_2_INPLACE(/=)
DEFINE_OPERATOR_2_INPLACE(%=)
DEFINE_OPERATOR_2_INPLACE(&=)
DEFINE_OPERATOR_2_INPLACE(|=)
DEFINE_OPERATOR_2_INPLACE(^=)

// 1
#define DEFINE_OPERATOR_1(_OP)       \
    template <class T>               \
    Series operator_OP() const       \
    {                                \
        Series<T> res = *this;       \
        res.values = _OP res.values; \
        return res;                  \
    }

DEFINE_OPERATOR_1(~)

// 1 inplace
#define DEFINE_OPERATOR_1_INPLACE(_OP) \
    template <class T>                 \
    Series operator_OP()               \
    {                                  \
        values = _OP values;           \
        return *this;                  \
    }

DEFINE_OPERATOR_1_INPLACE(~)

// bool
#define DEFINE_OPERATOR_BOOL_2(_OP)          \
    template <class T>                       \
    Series<IT, Int> operator_OP(T val) const \
    {                                        \
        Series<IT, T> res = *this;           \
        res.values = res.values _OP val;     \
        return res;                          \
    }

DEFINE_OPERATOR_BOOL_2(>)
DEFINE_OPERATOR_BOOL_2(>=)
DEFINE_OPERATOR_BOOL_2(<)
DEFINE_OPERATOR_BOOL_2(<=)
DEFINE_OPERATOR_BOOL_2(==)
DEFINE_OPERATOR_BOOL_2(!=)

/////////////////////// series operators /////////////////////////////////

// 2
#define DEFINE_OPERATOR_SERIES_2(_OP)                    \
    template <class IT2, class DT2>                      \
    Series operator_OP(const Series<IT2, DT2>& sr) const \
    {                                                    \
        Index<IT> index;                                 \
        for (int i = 0; i < pidx->size(); i++) {         \
            index.append(pidx->iloc(i));                 \
        }                                                \
        for (int i = 0; i < sr->size(); i++) {           \
            index.append(sr->pidx->iloc(i));             \
        }                                                \
        index.append(sr.index());                        \
        sr1 = this->reindex(index);                      \
        sr2 = sr->reindex(index);                        \
        Array<DT> vals = sr1.values _OP sr2.values;      \
        return Series(index, vals);                      \
    }

DEFINE_OPERATOR_SERIES_2(+)
DEFINE_OPERATOR_SERIES_2(-)
DEFINE_OPERATOR_SERIES_2(*)
DEFINE_OPERATOR_SERIES_2(/)
DEFINE_OPERATOR_SERIES_2(%)
DEFINE_OPERATOR_SERIES_2(&)
DEFINE_OPERATOR_SERIES_2(|)
DEFINE_OPERATOR_SERIES_2(^)

// 2 inplace
#define DEFINE_OPERATOR_SERIES_2_INPLACE(_OP)      \
    template <class IT2, class DT2>                \
    Series operator_OP(const Series<IT2, DT2>& sr) \
    {                                              \
        for (int i = 0; i < size(); i++) {         \
            IT& id = pidx->iloc(i);                \
            DT& val = values.iloc(i);              \
            if (sr.pidx->has(id)) {                \
                val _OP sr.loc(id);                \
            }                                      \
        }                                          \
        return *this;                              \
    }

DEFINE_OPERATOR_2_INPLACE(+=)
DEFINE_OPERATOR_2_INPLACE(-=)
DEFINE_OPERATOR_2_INPLACE(*=)
DEFINE_OPERATOR_2_INPLACE(/=)
DEFINE_OPERATOR_2_INPLACE(%=)
DEFINE_OPERATOR_2_INPLACE(&=)
DEFINE_OPERATOR_2_INPLACE(|=)
DEFINE_OPERATOR_2_INPLACE(^=)

////////////////BOOL operator///////////////////////

// bool
#define DEFINE_OPERATOR_BOOL_2(_OP)                               \
    template <class IT2, class DT2>                               \
    Series<IT, Int> operator_OP(const Series<IT2, DT2>& sr) const \
    {                                                             \
        Index<IT> index;                                          \
        for (int i = 0; i < pidx->size(); i++) {                  \
            index.append(pidx->iloc(i));                          \
        }                                                         \
        for (int i = 0; i < sr->size(); i++) {                    \
            index.append(sr->pidx->iloc(i));                      \
        }                                                         \
        index.append(sr.index());                                 \
        sr1 = this->reindex(index);                               \
        sr2 = sr->reindex(index);                                 \
        Array<DT> vals = sr1.values _OP sr2.values;               \
        return Series(index, vals);                               \
    }

DEFINE_OPERATOR_BOOL_2(>)
DEFINE_OPERATOR_BOOL_2(>=)
DEFINE_OPERATOR_BOOL_2(<)
DEFINE_OPERATOR_BOOL_2(<=)
DEFINE_OPERATOR_BOOL_2(==)
DEFINE_OPERATOR_BOOL_2(!=)

};