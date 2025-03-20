#pragma once

// template<class IT, class DT>
// class Series {

#define DEFINE_DATAFRAME_OPERATOR(OP)    \
    template <class T2>                  \
    DataFrame operator OP(const T2& val) \
    {                                    \
        DataFrame res = *this;           \
        for (auto& sr : res.values) {    \
            sr = sr OP val;              \
        }                                \
        return res;                      \
    }

DEFINE_DATAFRAME_OPERATOR(+)
DEFINE_DATAFRAME_OPERATOR(-)
DEFINE_DATAFRAME_OPERATOR(*)
DEFINE_DATAFRAME_OPERATOR(/)
DEFINE_DATAFRAME_OPERATOR(%)
DEFINE_DATAFRAME_OPERATOR(&)
DEFINE_DATAFRAME_OPERATOR(|)
DEFINE_DATAFRAME_OPERATOR(^)

#define DEFINE_DATAFRAME_OPERATOR(OP)      \
    template <class T2>                    \
    DataFrame& operator OP(const T2 & val) \
    {                                      \
        for (auto& sr : values) {          \
            sr OP val;                     \
        }                                  \
        return *this;                      \
    }

DEFINE_DATAFRAME_OPERATOR(+=)
DEFINE_DATAFRAME_OPERATOR(-=)
DEFINE_DATAFRAME_OPERATOR(*=)
DEFINE_DATAFRAME_OPERATOR(/=)
DEFINE_DATAFRAME_OPERATOR(%=)
DEFINE_DATAFRAME_OPERATOR(&=)
DEFINE_DATAFRAME_OPERATOR(|=)
DEFINE_DATAFRAME_OPERATOR(^=)

#define DEFINE_DATAFRAME_OPERATOR(OP) \
    DataFrame operator OP() const     \
    {                                 \
        DataFrame res = *this;        \
        for (auto& sr : res.values) { \
            sr = OP sr;               \
        }                             \
        return res;                   \
    }

DEFINE_DATAFRAME_OPERATOR(~)

/////////// cmp operator /////////////////
#define DEFINE_DATAFRAME_OPERATOR(OP)                               \
    template <class T2>                                             \
    DataFrame<IT, bool, INT, DNT> operator OP(const T2 & val) const \
    {                                                               \
        std::vector<Series<IT, bool, INT, DNT>> srs;                \
        for (auto& sr : values) {                                   \
            auto sr2 = sr OP val;                                   \
            srs.emplace_back(sr2);                                  \
        }                                                           \
        return DataFrame<IT, bool, INT, DNT>(srs);                  \
    }

DEFINE_DATAFRAME_OPERATOR(>)
DEFINE_DATAFRAME_OPERATOR(>=)
DEFINE_DATAFRAME_OPERATOR(<)
DEFINE_DATAFRAME_OPERATOR(<=)
DEFINE_DATAFRAME_OPERATOR(==)
DEFINE_DATAFRAME_OPERATOR(!=)