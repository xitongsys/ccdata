#pragma once

// template<class IT, class DT>
// class Series {
// class SeriesPicker {

#define DEFINE_SERIESPICKER_OPERATOR(OP)                                               \
    template <class T2>                                                                \
    Series<IT, DT, INT, DNT> operator OP(T2 val) const                                 \
    {                                                                                  \
        Series<IT, DT, INT, DNT> res = this->to_series();                              \
        res = res OP val;                                                              \
        return res;                                                                    \
    }                                                                                  \
                                                                                       \
    template <class DT2, class DNT2>                                                   \
    Series<IT, DT, INT, DNT> operator OP(const Array<DT2, DNT2>& ar) const             \
    {                                                                                  \
        Series<IT, DT, INT, DNT> res = this->to_series();                              \
        res = res OP ar;                                                               \
        return res;                                                                    \
    }                                                                                  \
                                                                                       \
    template <class IT2, class DT2, class INT2, class DNT2>                            \
    Series<IT, DT, INT, DNT> operator OP(const Series<IT2, DT2, INT2, DNT2>& sr) const \
    {                                                                                  \
        Series<IT, DT, INT, DNT> res = this->to_series();                              \
        res = res OP sr;                                                               \
        return res;                                                                    \
    }

DEFINE_SERIESPICKER_OPERATOR(+)
DEFINE_SERIESPICKER_OPERATOR(-)
DEFINE_SERIESPICKER_OPERATOR(*)
DEFINE_SERIESPICKER_OPERATOR(/)
DEFINE_SERIESPICKER_OPERATOR(%)
DEFINE_SERIESPICKER_OPERATOR(&)
DEFINE_SERIESPICKER_OPERATOR(|)
DEFINE_SERIESPICKER_OPERATOR(^)

#define DEFINE_SERIESPICKER_OPERATOR(OP)                  \
    Series<IT, DT, INT, DNT> operator OP() const          \
    {                                                     \
        Series<IT, DT, INT, DNT> res = this->to_series(); \
        res = OP res;                                     \
        return res;                                       \
    }

DEFINE_SERIESPICKER_OPERATOR(~)

#define DEFINE_SERIESPICKER_OPERATOR(OP)                                                             \
    template <class T>                                                                               \
    void operator OP(const T& val)                                                                   \
    {                                                                                                \
        pvis->reset();                                                                               \
        for (int i = pvis->next().value_or(-1); i >= 0; i = pvis->next().value_or(-1)) {             \
            sr.iloc(i) OP val;                                                                       \
        }                                                                                            \
    }                                                                                                \
                                                                                                     \
    template <class DT2, class DNT2>                                                                 \
    void operator OP(const Array<DT2, DNT2>& ar)                                                     \
    {                                                                                                \
        if (ar.size() != size()) {                                                                   \
            throw std::format("size not match: {}!={}", ar.size(), size());                          \
        }                                                                                            \
        pvis->reset();                                                                               \
        for (int i = pvis->next().value_or(-1), j = 0; i >= 0; i = pvis->next().value_or(-1), j++) { \
            const DT2& val = ar.iloc(j);                                                             \
            sr.iloc(i) OP val;                                                                       \
        }                                                                                            \
    }                                                                                                \
                                                                                                     \
    template <class IT2, class DT2, class INT2, class DNT2>                                          \
    void operator OP(const Series<IT2, DT2, INT2, DNT2>& sr)                                         \
    {                                                                                                \
        if (sr.size() != size()) {                                                                   \
            throw std::format("size not match: {}!={}", sr.size(), size());                          \
        }                                                                                            \
        for (int i = 0; i < sr.size(); i++) {                                                        \
            const IT2& id = sr.pidx->iloc(i);                                                        \
            const DT2& val = sr.iloc(i);                                                             \
            if (!sr.pidx->has(id)) {                                                                 \
                throw std::format("key {} not found", id.to_string());                               \
            }                                                                                        \
            sr.loc(id) OP val;                                                                       \
        }                                                                                            \
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
#define DEFINE_SERIESPICKER_OPERATOR(OP)                                                 \
    template <class T2>                                                                  \
    Series<IT, bool, INT, DNT> operator OP(const T2 & val) const                         \
    {                                                                                    \
        Series tmp = this->to_series();                                                  \
        return tmp OP val;                                                               \
    }                                                                                    \
                                                                                         \
    template <class DT2, class DNT2>                                                     \
    Series<IT, bool, INT, DNT> operator OP(const Array<DT2, DNT2>& ar) const             \
    {                                                                                    \
        Series tmp = this->to_series();                                                  \
        return tmp OP ar;                                                                \
    }                                                                                    \
                                                                                         \
    template <class IT2, class DT2, class INT2, class DNT2>                              \
    Series<IT, bool, INT, DNT> operator OP(const Series<IT2, DT2, INT2, DNT2>& sr) const \
    {                                                                                    \
        Series tmp = this->to_series();                                                  \
        return tmp OP sr;                                                                \
    }

DEFINE_SERIESPICKER_OPERATOR(>)
DEFINE_SERIESPICKER_OPERATOR(>=)
DEFINE_SERIESPICKER_OPERATOR(<)
DEFINE_SERIESPICKER_OPERATOR(<=)
DEFINE_SERIESPICKER_OPERATOR(==)
DEFINE_SERIESPICKER_OPERATOR(!=)