#pragma once

namespace pandas {

// 2 element ops
#define DEFINE_OPERATOR_2(_OP)         \
    template <class T2>                \
    Array operator_OP(T2 val) const    \
    {                                  \
        Array res = to_array() _OP T2; \
        return res;                    \
    }

DEFINE_OPERATOR_2(+)
DEFINE_OPERATOR_2(-)
DEFINE_OPERATOR_2(*)
DEFINE_OPERATOR_2(/)
DEFINE_OPERATOR_2(%)
DEFINE_OPERATOR_2(&)
DEFINE_OPERATOR_2(|)
DEFINE_OPERATOR_2(^)

// 2 element ops inplacement
#define DEFINE_OPERATOR_2_INPLACE(_OP) \
    template <class T2>                \
    void operator_OP(T2 val)           \
    {                                  \
        for (int id : ids) {           \
            ar.iloc(id) _OP val;       \
        }                              \
    }

DEFINE_OPERATOR_2_INPLACE(+=)
DEFINE_OPERATOR_2_INPLACE(-=)
DEFINE_OPERATOR_2_INPLACE(*=)
DEFINE_OPERATOR_2_INPLACE(/=)
DEFINE_OPERATOR_2_INPLACE(%=)
DEFINE_OPERATOR_2_INPLACE(&=)
DEFINE_OPERATOR_2_INPLACE(|=)
DEFINE_OPERATOR_2_INPLACE(^=)

// 1 element operators
#define DEFINE_OPERATOR_1(_OP)   \
    template <class T2>          \
    void operator+=(T2 val)      \
    {                            \
        Array res = ~to_array(); \
    }

DEFINE_OPERATOR_1(~)

// 1 element operators inplacement
#define DEFINE_OPERATOR_1_INPLACE(_OP)  \
    template <class T2>                 \
    void operator+=(T2 val)             \
    {                                   \
        for (int id : ids) {            \
            ar.iloc(id) = ~ar.iloc(id); \
        }                               \
    }

DEFINE_OPERATOR_1_INPLACE(~)

// 2 array elements operators
#define DEFINE_OPERATOR_ARRAY_2(_OP)                                        \
    template <class T>                                                      \
    Array operator_OP(const ARRAY<T>& ar) const                             \
    {                                                                       \
        if (size() != ar.size()) {                                          \
            throw std::format("size not match: {}!={}", size(), ar.size()); \
        }                                                                   \
        Array res = *this;                                                  \
        for (int i = 0; i < size(); i++) {                                  \
            res.values[i] = res.values[i] _OP ar.values[i];                 \
        }                                                                   \
        return res                                                          \
    }

DEFINE_OPERATOR_ARRAY_2(+)
DEFINE_OPERATOR_ARRAY_2(-)
DEFINE_OPERATOR_ARRAY_2(*)
DEFINE_OPERATOR_ARRAY_2(/)
DEFINE_OPERATOR_ARRAY_2(%)
DEFINE_OPERATOR_ARRAY_2(&)
DEFINE_OPERATOR_ARRAY_2(|)
DEFINE_OPERATOR_ARRAY_2(^)

// 2 array elements operators inplacement
#define DEFINE_OPERATOR_ARRAY_2_INPLACE(_OP)                                \
    Array operator_OP(const Array& ar)                                      \
    {                                                                       \
        if (size() != ar.size()) {                                          \
            throw std::format("size not match: {}!={}", size(), ar.size()); \
        }                                                                   \
        for (int i = 0; i < size(); i++) {                                  \
            values[i] _OP ar.values[i];                                     \
        }                                                                   \
        return *this;                                                       \
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
#define DEFINE_OPERATOR_BOOL_2(_OP)     \
    template <class T>                  \
    Array<Int> operator_OP(T val) const \
    {                                   \
        Array<Int> res;                 \
        for (auto& v : values) {        \
            if (v _OP val) {            \
                res.append(Int(1));     \
            } else {                    \
                res.append(Int(0));     \
            }                           \
        }                               \
        return res;                     \
    }

DEFINE_OPERATOR_BOOL_2(>)
DEFINE_OPERATOR_BOOL_2(>=)
DEFINE_OPERATOR_BOOL_2(<)
DEFINE_OPERATOR_BOOL_2(<=)
DEFINE_OPERATOR_BOOL_2(==)
DEFINE_OPERATOR_BOOL_2(!=)

// bool
#define DEFINE_OPERATOR_BOOL_1(_OP) \
    template <class T>              \
    Array<Int> operator_OP() const  \
    {                               \
        Array<Int> res;             \
        for (auto& v : values) {    \
            if (_OP val) {          \
                res.append(Int(1)); \
            } else {                \
                res.append(Int(0)); \
            }                       \
        }                           \
        return res;                 \
    }

DEFINE_OPERATOR_BOOL_1(!)

// bool array 2
#define DEFINE_OPERATOR_BOOL_ARRAY_2(_OP)                                   \
    Array<Int> operator_OP(Array ar) const                                  \
    {                                                                       \
        if (size() != ar.size()) {                                          \
            throw std::format("size not match: {}!={}", size(), ar.size()); \
        }                                                                   \
        Array<Int> res;                                                     \
        for (int i = 0; i < size(); i++) {                                  \
            if (values[i] _OP ar.values[i]) {                               \
                res.append(Int(1));                                         \
            } else {                                                        \
                res.append(Int(0));                                         \
            }                                                               \
        }                                                                   \
        return res;                                                         \
    }

DEFINE_OPERATOR_BOOL_ARRAY_2(>)
DEFINE_OPERATOR_BOOL_ARRAY_2(>=)
DEFINE_OPERATOR_BOOL_ARRAY_2(<)
DEFINE_OPERATOR_BOOL_ARRAY_2(<=)
DEFINE_OPERATOR_BOOL_ARRAY_2(==)
DEFINE_OPERATOR_BOOL_ARRAY_2(!=)

}