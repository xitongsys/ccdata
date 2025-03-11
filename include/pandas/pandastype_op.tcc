#pragma once

#define DEFINE_PTYPE_OPERATOR(OP)                         \
    template <class T2>                                   \
    PandasType operator OP(const T2& v) const             \
    {                                                     \
        if (is_nan) {                                     \
            return PandasType {};                         \
        }                                                 \
        return PandasType<T>(value OP v);                 \
    }                                                     \
                                                          \
    template <class T2>                                   \
    PandasType operator OP(const PandasType<T2>& v) const \
    {                                                     \
        if (is_nan || v.is_nan) {                         \
            return PandasType<T>();                       \
        }                                                 \
        return PandasType<T>(value OP v.value);           \
    }                                                     \
                                                          \
    PandasType operator OP(const PandasType& v) const     \
    {                                                     \
        if (is_nan || v.is_nan) {                         \
            return PandasType<T>();                       \
        }                                                 \
        return PandasType<T>(value OP v.value);           \
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
        if (is_nan) {                               \
            return *this;                           \
        }                                           \
        value OP v;                                 \
        return *this;                               \
    }                                               \
                                                    \
    template <class T2>                             \
    PandasType operator OP(const PandasType<T2>& v) \
    {                                               \
        if (is_nan || v.is_nan) {                   \
            *this = PandasType {};                  \
        }                                           \
        value OP v.value;                           \
        return *this;                               \
    }                                               \
                                                    \
    PandasType operator OP(const PandasType& v)     \
    {                                               \
        if (is_nan || v.is_nan) {                   \
            *this = PandasType {};                  \
        }                                           \
        value OP v.value;                           \
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
        if (is_nan) {                   \
            return PandasType<T>();     \
        }                               \
        return PandasType<T>(OP value); \
    }

DEFINE_PTYPE_OPERATOR(~)

#define DEFINE_PTYPE_OPERATOR(OP)                   \
    template <class T2>                             \
    bool operator OP(const T2& v) const             \
    {                                               \
        if (is_nan) {                               \
            return false;                           \
        }                                           \
        return value OP v;                          \
    }                                               \
                                                    \
    template <class T2>                             \
    bool operator OP(const PandasType<T2>& v) const \
    {                                               \
        if (is_nan || v.is_nan) {                   \
            return false;                           \
        }                                           \
        return value OP v.value;                    \
    }                                               \
    bool operator OP(const PandasType& v) const     \
    {                                               \
        if (is_nan || v.is_nan) {                   \
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
