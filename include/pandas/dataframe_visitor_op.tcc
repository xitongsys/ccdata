#pragma once

// template<class IT, class DT, class INT, class DNT>
// class DataFrame {
// class DataFrameVisitor {

#define DEFINE_DATAFRAMEVISITOR_OPERATOR(OP)                \
    template <class T2>                                     \
    DataFrame<IT, DT, INT, DNT> operator OP(T2 val) const   \
    {                                                       \
        DataFrame<IT, DT, INT, DNT> res = this->to_frame(); \
        res = res OP val;                                   \
        return res;                                         \
    }

DEFINE_DATAFRAMEVISITOR_OPERATOR(+)
DEFINE_DATAFRAMEVISITOR_OPERATOR(-)
DEFINE_DATAFRAMEVISITOR_OPERATOR(*)
DEFINE_DATAFRAMEVISITOR_OPERATOR(/)
DEFINE_DATAFRAMEVISITOR_OPERATOR(%)
DEFINE_DATAFRAMEVISITOR_OPERATOR(&)
DEFINE_DATAFRAMEVISITOR_OPERATOR(|)
DEFINE_DATAFRAMEVISITOR_OPERATOR(^)

#define DEFINE_DATAFRAMEVISITOR_OPERATOR(OP)                \
    DataFrame<IT, DT, INT, DNT> operator OP() const         \
    {                                                       \
        DataFrame<IT, DT, INT, DNT> res = this->to_frame(); \
        res = OP res;                                       \
        return res;                                         \
    }

DEFINE_DATAFRAMEVISITOR_OPERATOR(~)

/////////////////////////////////////////////////////////////////////////////

#define DEFINE_DATAFRAMEVISITOR_OPERATOR(OP) \
    template <class T>                       \
    DataFrame& operator OP(const T & val)    \
    {                                        \
        it_row.reset();                      \
        it_col.reset();                      \
        while (it_col.has_left()) {          \
            int j = it_col.next();           \
            it_row.reset();                  \
            while (it_row.has_left()) {      \
                int i = it_row.next();       \
                iloc_ref(i, j) OP val;       \
            }                                \
        }                                    \
        return *this;                        \
    }

DEFINE_DATAFRAMEVISITOR_OPERATOR(+=)
DEFINE_DATAFRAMEVISITOR_OPERATOR(-=)
DEFINE_DATAFRAMEVISITOR_OPERATOR(*=)
DEFINE_DATAFRAMEVISITOR_OPERATOR(/=)
DEFINE_DATAFRAMEVISITOR_OPERATOR(%=)
DEFINE_DATAFRAMEVISITOR_OPERATOR(&=)
DEFINE_DATAFRAMEVISITOR_OPERATOR(|=)
DEFINE_DATAFRAMEVISITOR_OPERATOR(^=)

////////////////////////////////////// cmp operator ////////////////////////////////////////////

#define DEFINE_DATAFRAMEVISITOR_OPERATOR(OP)                        \
    template <class T2>                                             \
    DataFrame<IT, bool, INT, DNT> operator OP(const T2 & val) const \
    {                                                               \
        DataFrame<IT, bool, INT, DNT> tmp = this->to_frame();       \
        return tmp OP val;                                          \
    }

DEFINE_DATAFRAMEVISITOR_OPERATOR(>)
DEFINE_DATAFRAMEVISITOR_OPERATOR(>=)
DEFINE_DATAFRAMEVISITOR_OPERATOR(<)
DEFINE_DATAFRAMEVISITOR_OPERATOR(<=)
DEFINE_DATAFRAMEVISITOR_OPERATOR(==)
DEFINE_DATAFRAMEVISITOR_OPERATOR(!=)