#pragma once

#define DEFINE_PTYPE_OPERATOR(OP)                         \
    template <class T2>                                   \
    PandasType operator OP(const PandasType<T2>& v) const \
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
    PandasType operator OP(const PandasType<T2>& v) \
    {                                               \
        if (is_nan || v.is_nan) {                   \
            value = PandasType {};                  \
        }                                           \
        value OP v.value;                           \
        return value;                               \
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

#define DEFINE_PTYPE_OPERATOR(OP)                              \
    template <class T2>                                        \
    PandasType<int> operator OP(const PandasType<T2>& v) const \
    {                                                          \
        if (is_nan || v.is_nan) {                              \
            return PandasType<int>();                          \
        }                                                      \
        if (value OP v.value) {                                \
            return PandasType<int>(1);                         \
        } else {                                               \
            return PandasType<int>(0);                         \
        }                                                      \
    }

DEFINE_PTYPE_OPERATOR(>)
DEFINE_PTYPE_OPERATOR(>=)
DEFINE_PTYPE_OPERATOR(<)
DEFINE_PTYPE_OPERATOR(<=)
DEFINE_PTYPE_OPERATOR(==)
DEFINE_PTYPE_OPERATOR(!=)

/////////////// simple type //////////////

#define DEFINE_PTYPE_OPERATOR(OP)             \
    template <class T2>                       \
    PandasType operator OP(const T2& v) const \
    {                                         \
        return PandasType<T>(value OP v);     \
    }

DEFINE_PTYPE_OPERATOR(+)
DEFINE_PTYPE_OPERATOR(-)
DEFINE_PTYPE_OPERATOR(*)
DEFINE_PTYPE_OPERATOR(/)
DEFINE_PTYPE_OPERATOR(%)
DEFINE_PTYPE_OPERATOR(&)
DEFINE_PTYPE_OPERATOR(|)
DEFINE_PTYPE_OPERATOR(^)

#define DEFINE_PTYPE_OPERATOR(OP)       \
    template <class T2>                 \
    PandasType operator OP(const T2& v) \
    {                                   \
        value OP v;                     \
        return value;                   \
    }

DEFINE_PTYPE_OPERATOR(+=)
DEFINE_PTYPE_OPERATOR(-=)
DEFINE_PTYPE_OPERATOR(*=)
DEFINE_PTYPE_OPERATOR(/=)
DEFINE_PTYPE_OPERATOR(%=)
DEFINE_PTYPE_OPERATOR(&=)
DEFINE_PTYPE_OPERATOR(|=)
DEFINE_PTYPE_OPERATOR(^=)

#define DEFINE_PTYPE_OPERATOR(OP)                   \
    template <class T2>                             \
    PandasType<int> operator OP(const T2 & v) const \
    {                                               \
        return value OP v;                          \
    }

DEFINE_PTYPE_OPERATOR(>)
DEFINE_PTYPE_OPERATOR(>=)
DEFINE_PTYPE_OPERATOR(<)
DEFINE_PTYPE_OPERATOR(<=)
DEFINE_PTYPE_OPERATOR(==)
DEFINE_PTYPE_OPERATOR(!=)
