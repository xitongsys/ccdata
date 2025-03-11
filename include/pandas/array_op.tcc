#pragma once

#define DEFINE_ARRAY_OPERATOR(OP)                                           \
    template <class T2>                                                     \
    Array operator OP(T2 val) const                                         \
    {                                                                       \
        Array<T> res(*this);                                                \
        for (auto& v : res.values) {                                        \
            v = v OP val;                                                   \
        }                                                                   \
        return res;                                                         \
    }                                                                       \
                                                                            \
    template <class T2>                                                     \
    Array operator OP(const Array<T2>& ar) const                            \
    {                                                                       \
        if (size() != ar.size()) {                                          \
            throw std::format("size not match: {}!={}", size(), ar.size()); \
        }                                                                   \
        Array<T> res = *this;                                               \
        for (int i = 0; i < size(); i++) {                                  \
            res.values[i] = res.values[i] OP ar.values[i];                  \
        }                                                                   \
        return res;                                                         \
    }

DEFINE_ARRAY_OPERATOR(+)
DEFINE_ARRAY_OPERATOR(-)
DEFINE_ARRAY_OPERATOR(*)
DEFINE_ARRAY_OPERATOR(/)
DEFINE_ARRAY_OPERATOR(%)
DEFINE_ARRAY_OPERATOR(&)
DEFINE_ARRAY_OPERATOR(|)
DEFINE_ARRAY_OPERATOR(^)

#define DEFINE_ARRAY_OPERATOR(OP)                                           \
    template <class T2>                                                     \
    Array& operator OP(T2 val)                                              \
    {                                                                       \
        for (auto& v : values) {                                            \
            v = v OP val;                                                   \
        }                                                                   \
        return *this;                                                       \
    }                                                                       \
                                                                            \
    template <class T2>                                                     \
    Array& operator OP(const Array<T2>& ar)                                 \
    {                                                                       \
        if (size() != ar.size()) {                                          \
            throw std::format("size not match: {}!={}", size(), ar.size()); \
        }                                                                   \
        for (int i = 0; i < size(); i++) {                                  \
            values[i] OP ar.values[i];                                      \
        }                                                                   \
        return *this;                                                       \
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
        Array<T> res = *this;        \
        for (auto& v : res.values) { \
            v = OP v;                \
        }                            \
        return res;                  \
    }

DEFINE_ARRAY_OPERATOR(~)

////////////////CMP operator///////////////////////

#define DEFINE_ARRAY_OPERATOR(OP)                                           \
    template <class T2>                                                     \
    Array<Int> operator OP(const T2 & val) const                            \
    {                                                                       \
        Array<Int> res;                                                     \
        for (auto& v : values) {                                            \
            if (v OP val) {                                                 \
                res.append(Int(1));                                         \
            } else {                                                        \
                res.append(Int(0));                                         \
            }                                                               \
        }                                                                   \
        return res;                                                         \
    }                                                                       \
                                                                            \
    template <class T2>                                                     \
    Array<Int> operator OP(const Array<T2>& ar) const                       \
    {                                                                       \
        if (size() != ar.size()) {                                          \
            throw std::format("size not match: {}!={}", size(), ar.size()); \
        }                                                                   \
        Array<Int> res;                                                     \
        for (int i = 0; i < size(); i++) {                                  \
            if (values[i] OP ar.values[i]) {                                \
                res.append(Int(1));                                         \
            } else {                                                        \
                res.append(Int(0));                                         \
            }                                                               \
        }                                                                   \
        return res;                                                         \
    }

DEFINE_ARRAY_OPERATOR(>)
DEFINE_ARRAY_OPERATOR(>=)
DEFINE_ARRAY_OPERATOR(<)
DEFINE_ARRAY_OPERATOR(<=)
DEFINE_ARRAY_OPERATOR(==)
DEFINE_ARRAY_OPERATOR(!=)
