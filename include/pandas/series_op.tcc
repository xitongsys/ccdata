#pragma once

// template<class IT, class DT>
// class Series {

#define DEFINE_SERIES_OPERATOR(OP)                                            \
    template <class T2>                                                       \
    Series operator OP(const T2& val)                                         \
    {                                                                         \
        Series res = *this;                                                   \
        res.values = res.values OP val;                                       \
        return res;                                                           \
    }                                                                         \
                                                                              \
    template <class DT2>                                                      \
    Series operator OP(const std::vector<DT2>& vals)                          \
    {                                                                         \
        if (size() != vals.size()) {                                          \
            throw std::format("size not match: {}!={}", size(), vals.size()); \
        }                                                                     \
        Series res = *this;                                                   \
        for (int i = 0; i < size(); i++) {                                    \
            res.iloc_ref(i) = res.iloc(i) OP vals[i];                         \
        }                                                                     \
        return res;                                                           \
    }                                                                         \
                                                                              \
    template <class DT2, class DNT2>                                          \
    Series operator OP(const Array<DT2, DNT2>& ar)                            \
    {                                                                         \
        return ((*this)OP ar.values);                                         \
    }                                                                         \
                                                                              \
    template <class IT2, class DT2, class INT2, class DNT2>                   \
    Series operator OP(const Series<IT2, DT2, INT2, DNT2>& sr)                \
    {                                                                         \
        Series res = *this;                                                   \
        for (int i = 0; i < res.size(); i++) {                                \
            IT id = res.pidx->iloc(i);                                        \
            DT val = res.iloc(i);                                             \
            if (sr.pidx->has(id)) {                                           \
                res.iloc_ref(i) = val OP sr.loc(id);                          \
            } else {                                                          \
                res.iloc_ref(i) = val OP pandas::nan<DT>();                   \
            }                                                                 \
            return res;                                                       \
        }                                                                     \
    }

DEFINE_SERIES_OPERATOR(+)
DEFINE_SERIES_OPERATOR(-)
DEFINE_SERIES_OPERATOR(*)
DEFINE_SERIES_OPERATOR(/)
DEFINE_SERIES_OPERATOR(%)
DEFINE_SERIES_OPERATOR(&)
DEFINE_SERIES_OPERATOR(|)
DEFINE_SERIES_OPERATOR(^)

#define DEFINE_SERIES_OPERATOR(OP)           \
    Series operator OP() const               \
    {                                        \
        Series<IT, DT, INT, DT> res = *this; \
        res.values = OP res.values;          \
        return res;                          \
    }

DEFINE_SERIES_OPERATOR(~)

////////////////////////////////////////////////////////////////////////////////

#define DEFINE_SERIES_OPERATOR(OP)                                            \
    template <class T>                                                        \
    Series& operator OP(T val)                                                \
    {                                                                         \
        values OP val;                                                        \
        return *this;                                                         \
    }                                                                         \
                                                                              \
    template <class DT2>                                                      \
    Series& operator OP(const std::vector<DT2>& vals)                         \
    {                                                                         \
        if (vals.size() != size()) {                                          \
            throw std::format("size not match: {}!={}", vals.size(), size()); \
        }                                                                     \
        for (int i = 0; i < size(); i++) {                                    \
            iloc(i) OP vals[i];                                               \
        }                                                                     \
        return *this;                                                         \
    }                                                                         \
                                                                              \
    template <class DT2, class DNT2>                                          \
    Series& operator OP(const Array<DT2, DNT2>& ar)                           \
    {                                                                         \
        return (*this)OP ar.values;                                           \
    }                                                                         \
                                                                              \
    template <class IT2, class DT2, class INT2, class DNT2>                   \
    Series& operator OP(const Series<IT2, DT2, INT2, DNT2>& sr)               \
    {                                                                         \
        for (int i = 0; i < size(); i++) {                                    \
            IT& id = pidx->iloc(i);                                           \
            DT& val = values.iloc(i);                                         \
            if (sr.pidx->has(id)) {                                           \
                val OP sr.loc(id);                                            \
            } else {                                                          \
                val OP pandas::nan<DT>();                                     \
            }                                                                 \
        }                                                                     \
        return *this;                                                         \
    }

DEFINE_SERIES_OPERATOR(+=)
DEFINE_SERIES_OPERATOR(-=)
DEFINE_SERIES_OPERATOR(*=)
DEFINE_SERIES_OPERATOR(/=)
DEFINE_SERIES_OPERATOR(%=)
DEFINE_SERIES_OPERATOR(&=)
DEFINE_SERIES_OPERATOR(|=)
DEFINE_SERIES_OPERATOR(^=)

///////////////////////////////// cmp operator ////////////////////////////////////////////
#define DEFINE_SERIES_OPERATOR(OP)                                                       \
    template <class T2>                                                                  \
    Series<IT, bool, INT, DNT> operator OP(const T2 & val) const                         \
    {                                                                                    \
        Array<bool, DNT> values = this->values OP val;                                   \
        auto res = Series<IT, bool, INT, DNT>(*pidx, values);                            \
        return res;                                                                      \
    }                                                                                    \
                                                                                         \
    template <class DT2>                                                                 \
    Series<IT, bool, INT, DNT> operator OP(const std::vector<DT2>& vals) const           \
    {                                                                                    \
        if (vals.size() != size()) {                                                     \
            throw std::format("size not match: {}!={}", vals.size(), size());            \
        }                                                                                \
        Array<bool, DNT> ar = values OP vals;                                            \
        auto res = Series<IT, bool, INT, DNT>(*pidx, ar);                                \
        return res;                                                                      \
    }                                                                                    \
                                                                                         \
    template <class DT2, class DNT2>                                                     \
    Series<IT, bool, INT, DNT> operator OP(const Array<DT2, DNT2>& ar) const             \
    {                                                                                    \
        return (*this)OP ar.values;                                                      \
    }                                                                                    \
                                                                                         \
    template <class IT2, class DT2, class INT2, class DNT2>                              \
    Series<IT, bool, INT, DNT> operator OP(const Series<IT2, DT2, INT2, DNT2>& sr) const \
    {                                                                                    \
        Series<IT, bool, INT, DNT> res(get_name());                                      \
        for (int i = 0; i < size(); i++) {                                               \
            IT id = pidx->iloc(i);                                                       \
            DT val = iloc(i);                                                            \
            if (sr.pidx->has(id)) {                                                      \
                res._append(id, val OP sr.loc(id));                                      \
            } else {                                                                     \
                throw std::format("key not found: {}", pandas::to_string(id));           \
            }                                                                            \
        }                                                                                \
        res.pidx->_rename(pidx->get_name());                                             \
        return res;                                                                      \
    }

DEFINE_SERIES_OPERATOR(>)
DEFINE_SERIES_OPERATOR(>=)
DEFINE_SERIES_OPERATOR(<)
DEFINE_SERIES_OPERATOR(<=)
DEFINE_SERIES_OPERATOR(==)
DEFINE_SERIES_OPERATOR(!=)