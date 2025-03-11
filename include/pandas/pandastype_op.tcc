#pragma once

#define DEFINE_PTYPE_OPERATOR(OP)                         \
    template <class T2>                                   \
    PandasType operator OP(const T2& v) const             \
    {                                                     \
        if (isnan()) {                                    \
            return PandasType {};                         \
        }                                                 \
        auto vv = value OP v;                             \
        if (std::isnan(vv)) {                             \
            return PandasType {};                         \
        }                                                 \
        return PandasType<T>(vv);                         \
    }                                                     \
                                                          \
    template <class T2>                                   \
    PandasType operator OP(const PandasType<T2>& v) const \
    {                                                     \
        if (isnan() || v.isnan()) {                       \
            return PandasType<T>();                       \
        }                                                 \
        auto vv = value OP v.value;                       \
        if (std::isnan(vv)) {                             \
            return PandasType<T>();                       \
        }                                                 \
        return PandasType<T>(vv);                         \
    }                                                     \
                                                          \
    PandasType operator OP(const PandasType& v) const     \
    {                                                     \
        if (isnan() || v.isnan()) {                       \
            return PandasType<T>();                       \
        }                                                 \
        auto vv = value OP v.value;                       \
        if (std::isnan(vv)) {                             \
            return PandasType<T>();                       \
        }                                                 \
        return PandasType<T>(vv);                         \
    }

DEFINE_PTYPE_OPERATOR(+)
DEFINE_PTYPE_OPERATOR(-)
DEFINE_PTYPE_OPERATOR(*)
DEFINE_PTYPE_OPERATOR(/)
DEFINE_PTYPE_OPERATOR(%)
DEFINE_PTYPE_OPERATOR(&)
DEFINE_PTYPE_OPERATOR(|)
DEFINE_PTYPE_OPERATOR(^)

#define DEFINE_PTYPE_OPERATOR(OP)                   \
    template <class T2>                             \
    PandasType operator OP(const T2& v)             \
    {                                               \
        if (isnan()) {                              \
            return *this;                           \
        }                                           \
        value OP v;                                 \
        if (std::isnan(value)) {                    \
            is_nan = true;                          \
        }                                           \
        return *this;                               \
    }                                               \
                                                    \
    template <class T2>                             \
    PandasType operator OP(const PandasType<T2>& v) \
    {                                               \
        if (isnan() || v.isnan()) {                 \
            *this = PandasType {};                  \
        }                                           \
        value OP v.value;                           \
        if (std::isnan(value)) {                    \
            is_nan = true;                          \
        }                                           \
        return *this;                               \
    }                                               \
                                                    \
    PandasType operator OP(const PandasType& v)     \
    {                                               \
        if (isnan() || v.isnan()) {                 \
            *this = PandasType {};                  \
        }                                           \
        value OP v.value;                           \
        if (std::isnan(value)) {                    \
            is_nan = true;                          \
        }                                           \
        return *this;                               \
    }

DEFINE_PTYPE_OPERATOR(+=)
DEFINE_PTYPE_OPERATOR(-=)
DEFINE_PTYPE_OPERATOR(*=)
DEFINE_PTYPE_OPERATOR(/=)
DEFINE_PTYPE_OPERATOR(%=)
DEFINE_PTYPE_OPERATOR(&=)
DEFINE_PTYPE_OPERATOR(|=)
DEFINE_PTYPE_OPERATOR(^=)

#define DEFINE_PTYPE_OPERATOR(OP)       \
    PandasType operator OP() const      \
    {                                   \
        if (isnan()) {                  \
            return PandasType<T>();     \
        }                               \
        return PandasType<T>(OP value); \
    }

DEFINE_PTYPE_OPERATOR(~)

#define DEFINE_PTYPE_OPERATOR(OP)                   \
    template <class T2>                             \
    bool operator OP(const T2& v) const             \
    {                                               \
        if (isnan()) {                              \
            return false;                           \
        }                                           \
        return value OP v;                          \
    }                                               \
                                                    \
    template <class T2>                             \
    bool operator OP(const PandasType<T2>& v) const \
    {                                               \
        if (isnan() || v.isnan()) {                 \
            return false;                           \
        }                                           \
        return value OP v.value;                    \
    }                                               \
    bool operator OP(const PandasType& v) const     \
    {                                               \
        if (isnan() || v.isnan()) {                 \
            return false;                           \
        }                                           \
        return value OP v.value;                    \
    }

DEFINE_PTYPE_OPERATOR(>)
DEFINE_PTYPE_OPERATOR(>=)
DEFINE_PTYPE_OPERATOR(<)
DEFINE_PTYPE_OPERATOR(<=)
DEFINE_PTYPE_OPERATOR(==)
DEFINE_PTYPE_OPERATOR(!=)
