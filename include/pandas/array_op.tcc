#pragma once

#define DEFINE_ARRAY_OPERATOR(OP)                                                   \
    template <class T2>                                                             \
    Array operator OP(T2 val) const                                                 \
    {                                                                               \
        Array res(*this);                                                           \
        for (auto& v : res.values) {                                                \
            v = v OP val;                                                           \
        }                                                                           \
        return res;                                                                 \
    }                                                                               \
                                                                                    \
    template <class T2, class NT2>                                                  \
    Array operator OP(const Array<T2, NT2>& ar) const                               \
    {                                                                               \
        if (size() != ar.size()) {                                                  \
            PANDAS_THROW(std::format("size not match: {}!={}", size(), ar.size())); \
        }                                                                           \
        Array res = *this;                                                          \
        for (int i = 0; i < size(); i++) {                                          \
            res.values[i] = res.values[i] OP ar.values[i];                          \
        }                                                                           \
        return res;                                                                 \
    }

DEFINE_ARRAY_OPERATOR(+)
DEFINE_ARRAY_OPERATOR(-)
DEFINE_ARRAY_OPERATOR(*)
DEFINE_ARRAY_OPERATOR(/)
DEFINE_ARRAY_OPERATOR(%)
DEFINE_ARRAY_OPERATOR(&)
DEFINE_ARRAY_OPERATOR(|)
DEFINE_ARRAY_OPERATOR(^)

#define DEFINE_ARRAY_OPERATOR(OP)                                                   \
    template <class T2>                                                             \
    Array& operator OP(T2 val)                                                      \
    {                                                                               \
        for (auto& v : values) {                                                    \
            v = v OP val;                                                           \
        }                                                                           \
        return *this;                                                               \
    }                                                                               \
                                                                                    \
    template <class T2, class NT2>                                                  \
    Array& operator OP(const Array<T2, NT2>& ar)                                    \
    {                                                                               \
        if (size() != ar.size()) {                                                  \
            PANDAS_THROW(std::format("size not match: {}!={}", size(), ar.size())); \
        }                                                                           \
        for (int i = 0; i < size(); i++) {                                          \
            values[i] OP ar.values[i];                                              \
        }                                                                           \
        return *this;                                                               \
    }

DEFINE_ARRAY_OPERATOR(+=)
DEFINE_ARRAY_OPERATOR(-=)
DEFINE_ARRAY_OPERATOR(*=)
DEFINE_ARRAY_OPERATOR(/=)
DEFINE_ARRAY_OPERATOR(%=)
DEFINE_ARRAY_OPERATOR(&=)
DEFINE_ARRAY_OPERATOR(|=)
DEFINE_ARRAY_OPERATOR(^=)

#define DEFINE_ARRAY_OPERATOR(OP)    \
    Array operator OP() const        \
    {                                \
        Array res = *this;           \
        for (auto& v : res.values) { \
            v = OP v;                \
        }                            \
        return res;                  \
    }

DEFINE_ARRAY_OPERATOR(~)

////////////////CMP operator///////////////////////

#define DEFINE_ARRAY_OPERATOR(OP)                                                   \
    template <class T2>                                                             \
    Array<char, NT> operator OP(const T2 & val) const                               \
    {                                                                               \
        Array<char, NT> res(get_name());                                            \
        for (auto& v : values) {                                                    \
            res._append(v OP val);                                                  \
        }                                                                           \
        return res;                                                                 \
    }                                                                               \
                                                                                    \
    template <class T2, class NT2>                                                  \
    Array<char, NT> operator OP(const Array<T2, NT2>& ar) const                     \
    {                                                                               \
        if (size() != ar.size()) {                                                  \
            PANDAS_THROW(std::format("size not match: {}!={}", size(), ar.size())); \
        }                                                                           \
        Array<char, NT> res(get_name());                                            \
        for (int i = 0; i < size(); i++) {                                          \
            res._append(values[i] OP ar.values[i]);                                 \
        }                                                                           \
        return res;                                                                 \
    }

DEFINE_ARRAY_OPERATOR(>)
DEFINE_ARRAY_OPERATOR(>=)
DEFINE_ARRAY_OPERATOR(<)
DEFINE_ARRAY_OPERATOR(<=)
DEFINE_ARRAY_OPERATOR(==)
DEFINE_ARRAY_OPERATOR(!=)
DEFINE_ARRAY_OPERATOR(&&)
DEFINE_ARRAY_OPERATOR(||)

#define DEFINE_ARRAY_OPERATOR(OP)        \
    Array<char, NT> operator OP() const  \
    {                                    \
        Array<char, NT> res(get_name()); \
        for (auto v : values) {          \
            res._append(OP v);           \
        }                                \
        return res;                      \
    }

DEFINE_ARRAY_OPERATOR(!)