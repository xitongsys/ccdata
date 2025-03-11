#pragma once

// template<class IT, class DT>
// class Series {
// class SeriesPicker {

#define DEFINE_SERIESPICKER_OPERATOR(OP)                         \
    template <class T2>                                          \
    Series<IT, DT> operator OP(T2 val) const                     \
    {                                                            \
        Series<IT, DT> res = this->to_series();                  \
        res = res OP val;                                        \
        return res;                                              \
    }                                                            \
                                                                 \
    template <class DT2>                                         \
    Series<IT, DT> operator OP(const Array<DT2>& ar) const       \
    {                                                            \
        Series<IT, DT> res = this->to_series();                  \
        res = res OP ar;                                         \
        return res;                                              \
    }                                                            \
                                                                 \
    template <class IT2, class DT2>                              \
    Series<IT, DT> operator OP(const Series<IT2, DT2>& sr) const \
    {                                                            \
        Series<IT, DT> res = this->to_series();                  \
        res = res OP sr;                                         \
        return res;                                              \
    }

DEFINE_SERIESPICKER_OPERATOR(+)
DEFINE_SERIESPICKER_OPERATOR(-)
DEFINE_SERIESPICKER_OPERATOR(*)
DEFINE_SERIESPICKER_OPERATOR(/)
DEFINE_SERIESPICKER_OPERATOR(%)
DEFINE_SERIESPICKER_OPERATOR(&)
DEFINE_SERIESPICKER_OPERATOR(|)
DEFINE_SERIESPICKER_OPERATOR(^)

#define DEFINE_SERIESPICKER_OPERATOR(OP)        \
    Series<IT, DT> operator OP() const          \
    {                                           \
        Series<IT, DT> res = this->to_series(); \
        res = OP res;                           \
        return res;                             \
    }

DEFINE_SERIESPICKER_OPERATOR(~)

#define DEFINE_SERIESPICKER_OPERATOR(OP)                                    \
    template <class T>                                                      \
    void operator OP(const T& val)                                          \
    {                                                                       \
        for (int i : iids) {                                                \
            sr.iloc(i) OP val;                                              \
        }                                                                   \
    }                                                                       \
                                                                            \
    template <class DT2>                                                    \
    void operator OP(const Array<DT2>& ar)                                  \
    {                                                                       \
        if (ar.size() != size()) {                                          \
            throw std::format("size not match: {}!={}", ar.size(), size()); \
        }                                                                   \
        for (int i = 0; i < ar.size(); i++) {                               \
            const DT2& val = ar.iloc(i);                                    \
            sr.iloc(iids[i]) OP val;                                        \
        }                                                                   \
    }                                                                       \
                                                                            \
    template <class IT2, class DT2>                                         \
    void operator OP(const Series<IT2, DT2>& sr)                            \
    {                                                                       \
        if (sr.size() != size()) {                                          \
            throw std::format("size not match: {}!={}", sr.size(), size()); \
        }                                                                   \
        for (int i = 0; i < sr.size(); i++) {                               \
            const IT2& id = sr.pidx->iloc(i);                               \
            const DT2& val = sr.iloc(i);                                    \
            if (!sr.pidx->has(id)) {                                        \
                throw std::format("key {} not found", id.to_string());      \
            }                                                               \
            sr.loc(id) OP val;                                              \
        }                                                                   \
    }

DEFINE_SERIESPICKER_OPERATOR(+=)
DEFINE_SERIESPICKER_OPERATOR(-=)
DEFINE_SERIESPICKER_OPERATOR(*=)
DEFINE_SERIESPICKER_OPERATOR(/=)
DEFINE_SERIESPICKER_OPERATOR(%=)
DEFINE_SERIESPICKER_OPERATOR(&=)
DEFINE_SERIESPICKER_OPERATOR(|=)
DEFINE_SERIESPICKER_OPERATOR(^=)

/////////// cmp operator /////////////////
#define DEFINE_SERIESPICKER_OPERATOR(OP)                           \
    template <class T2>                                            \
    Series<IT, Bool> operator OP(const T2 & val) const             \
    {                                                              \
        Series tmp = this->to_series();                            \
        return tmp OP val;                                         \
    }                                                              \
                                                                   \
    template <class DT2>                                           \
    Series<IT, Bool> operator OP(const Array<DT2>& ar) const       \
    {                                                              \
        Series tmp = this->to_series();                            \
        return tmp OP ar;                                          \
    }                                                              \
                                                                   \
    template <class IT2, class DT2>                                \
    Series<IT, Bool> operator OP(const Series<IT2, DT2>& sr) const \
    {                                                              \
        Series tmp = this->to_series();                            \
        return tmp OP sr;                                          \
    }

DEFINE_SERIESPICKER_OPERATOR(>)
DEFINE_SERIESPICKER_OPERATOR(>=)
DEFINE_SERIESPICKER_OPERATOR(<)
DEFINE_SERIESPICKER_OPERATOR(<=)
DEFINE_SERIESPICKER_OPERATOR(==)
DEFINE_SERIESPICKER_OPERATOR(!=)