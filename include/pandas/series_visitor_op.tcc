#pragma once

// template<class IT, class DT, class INT, class DNT>
// class Series {
// class SeriesVisitor {

#define DEFINE_SERIESVISITOR_OPERATOR(OP)                 \
    template <class T2>                                   \
    Series<IT, DT, INT, DNT> operator OP(T2 val) const    \
    {                                                     \
        Series<IT, DT, INT, DNT> res = this->to_series(); \
        res = res OP val;                                 \
        return res;                                       \
    }

DEFINE_SERIESVISITOR_OPERATOR(+)
DEFINE_SERIESVISITOR_OPERATOR(-)
DEFINE_SERIESVISITOR_OPERATOR(*)
DEFINE_SERIESVISITOR_OPERATOR(/)
DEFINE_SERIESVISITOR_OPERATOR(%)
DEFINE_SERIESVISITOR_OPERATOR(&)
DEFINE_SERIESVISITOR_OPERATOR(|)
DEFINE_SERIESVISITOR_OPERATOR(^)

#define DEFINE_SERIESVISITOR_OPERATOR(OP)                 \
    Series<IT, DT, INT, DNT> operator OP() const          \
    {                                                     \
        Series<IT, DT, INT, DNT> res = this->to_series(); \
        res = OP res;                                     \
        return res;                                       \
    }

DEFINE_SERIESVISITOR_OPERATOR(~)

////////////////////////////////////////////////////////////////////////////////

#define DEFINE_SERIESVISITOR_OPERATOR(OP)                     \
    template <class T>                                        \
    void operator OP(const T& val)                            \
    {                                                         \
        it.reset();                                           \
        while (it.has_left()) {                               \
            int i = it.next();                                \
            sr.iloc_ref(i) OP val;                            \
        }                                                     \
    }                                                         \
                                                              \
    template <class DT2>                                      \
    void operator OP(const std::vector<DT2>& vs)              \
    {                                                         \
        it.reset();                                           \
        int i = 0;                                            \
        for (i = 0; i < vs.size() && it.has_left(); i++) {    \
            sr.iloc_ref(it.next()) OP vs[i];                  \
        }                                                     \
        if (it.has_left() || (i + 1) < vs.size()) {           \
            throw std::format("size not match");              \
        }                                                     \
    }                                                         \
                                                              \
    template <class DT2, class DNT2>                          \
    void operator OP(const Array<DT2, DNT2>& ar)              \
    {                                                         \
        return (*this)OP(ar.values);                          \
    }                                                         \
                                                              \
    template <class IT2, class DT2, class INT2, class DNT2>   \
    void operator OP(const Series<IT2, DT2, INT2, DNT2>& sr2) \
    {                                                         \
        it.reset();                                           \
        while (it.has_left()) {                               \
            int i = it.next();                                \
            IT id = sr.pidx->iloc(i);                         \
            sr.iloc_ref(i) OP sr2.loc(id);                    \
        }                                                     \
    }

DEFINE_SERIESVISITOR_OPERATOR(+=)
DEFINE_SERIESVISITOR_OPERATOR(-=)
DEFINE_SERIESVISITOR_OPERATOR(*=)
DEFINE_SERIESVISITOR_OPERATOR(/=)
DEFINE_SERIESVISITOR_OPERATOR(%=)
DEFINE_SERIESVISITOR_OPERATOR(&=)
DEFINE_SERIESVISITOR_OPERATOR(|=)
DEFINE_SERIESVISITOR_OPERATOR(^=)

///////////////////////////////// cmp operator ///////////////////////////////////////////////
#define DEFINE_SERIESVISITOR_OPERATOR(OP)                        \
    template <class T2>                                          \
    Series<IT, bool, INT, DNT> operator OP(const T2 & val) const \
    {                                                            \
        Series tmp = this->to_series();                          \
        return tmp OP val;                                       \
    }

DEFINE_SERIESVISITOR_OPERATOR(>)
DEFINE_SERIESVISITOR_OPERATOR(>=)
DEFINE_SERIESVISITOR_OPERATOR(<)
DEFINE_SERIESVISITOR_OPERATOR(<=)
DEFINE_SERIESVISITOR_OPERATOR(==)
DEFINE_SERIESVISITOR_OPERATOR(!=)
DEFINE_SERIESVISITOR_OPERATOR(&&)
DEFINE_SERIESVISITOR_OPERATOR(||)

#define DEFINE_SERIESVISITOR_OPERATOR(OP)                   \
    Series<IT, bool, INT, DNT> operator OP() const          \
    {                                                       \
        Series<IT, bool, INT, DNT> res = this->to_series(); \
        res = OP res;                                       \
        return res;                                         \
    }

DEFINE_SERIESVISITOR_OPERATOR(!)
