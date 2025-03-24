#pragma once

// template<class IT, class DT, class INT, class DNT>
// class DataFrame {
#define DEFINE_DATAFRAME_OPERATOR(OP)                                               \
    template <class T2>                                                             \
    DataFrame operator OP(const T2& val)                                            \
    {                                                                               \
        DataFrame res = *this;                                                      \
        for (auto& sr : res.values) {                                               \
            sr = sr OP val;                                                         \
        }                                                                           \
        return res;                                                                 \
    }                                                                               \
    template <int axis = 0, class T2>                                               \
    DataFrame operator OP(const std::vector<T2>& vals)                              \
    {                                                                               \
        DataFrame res = *this;                                                      \
        if constexpr (axis == 0) {                                                  \
            for (auto& sr : res.values) {                                           \
                sr = sr OP vals;                                                    \
            }                                                                       \
            return res;                                                             \
                                                                                    \
        } else if constexpr (axis == 1) {                                           \
            if (vals.size() != size<1>()) {                                         \
                throw std::format("size not match: {}:{}", vals.size(), size<1>()); \
            }                                                                       \
                                                                                    \
            for (int j = 0; j < size<1>(); j++) {                                   \
                res.values[j] = res.values[j] OP vals[j];                           \
            }                                                                       \
            return res;                                                             \
                                                                                    \
        } else {                                                                    \
            throw std::format("axis not supported: {}", axis);                      \
        }                                                                           \
    }                                                                               \
    template <int axis = 0, class T2, class NT2>                                    \
    DataFrame operator OP(const Array<T2, NT2>& vals)                               \
    {                                                                               \
        return operator OP<axis, T2>(vals.values);                                  \
    }                                                                               \
                                                                                    \
    template <int axis = 0, class IT2, class DT2, class INT2, class DNT2>           \
    DataFrame operator OP(const Series<IT2, DT2, INT2, DNT2>& sr2)                  \
    {                                                                               \
        DataFrame res = *this;                                                      \
        if constexpr (axis == 0) {                                                  \
            for (auto& sr : res.values) {                                           \
                sr = sr OP sr2;                                                     \
            }                                                                       \
            return res;                                                             \
                                                                                    \
        } else if constexpr (axis == 1) {                                           \
            for (auto& sr : res.values) {                                           \
                DNT& col_name = sr.get_name();                                      \
                if (sr2.pidx->has(col_name)) {                                      \
                    sr = sr OP sr2.loc(col_name);                                   \
                } else {                                                            \
                    sr = sr OP pandas::nan<DT>();                                   \
                }                                                                   \
            }                                                                       \
            return res;                                                             \
                                                                                    \
        } else {                                                                    \
            throw std::format("axis not support: {}", axis);                        \
        }                                                                           \
    }

DEFINE_DATAFRAME_OPERATOR(+)
DEFINE_DATAFRAME_OPERATOR(-)
DEFINE_DATAFRAME_OPERATOR(*)
DEFINE_DATAFRAME_OPERATOR(/)
DEFINE_DATAFRAME_OPERATOR(%)
DEFINE_DATAFRAME_OPERATOR(&)
DEFINE_DATAFRAME_OPERATOR(|)
DEFINE_DATAFRAME_OPERATOR(^)

DataFrame operator~()
{
    DataFrame res = *this;
    for (int j = 0; j < res.size<1>(); j++) {
        res.values[j] = ~res.values[j];
    }
    return res;
}

/////////////////////////////////////////////////////////////////////////////////////////////

#define DEFINE_DATAFRAME_OPERATOR(OP)                                               \
    template <class T2>                                                             \
    DataFrame& operator OP(const T2 & val)                                          \
    {                                                                               \
        for (auto& sr : values) {                                                   \
            sr OP val;                                                              \
        }                                                                           \
        return *this;                                                               \
    }                                                                               \
    template <int axis = 0, class T2>                                               \
    DataFrame& operator OP(const std::vector<T2>& vals)                             \
    {                                                                               \
        if constexpr (axis == 0) {                                                  \
            for (auto& sr : values) {                                               \
                sr OP vals;                                                         \
            }                                                                       \
            return *this;                                                           \
                                                                                    \
        } else if constexpr (axis == 1) {                                           \
            if (vals.size() != size<1>()) {                                         \
                throw std::format("size not match: {}:{}", vals.size(), size<1>()); \
            }                                                                       \
                                                                                    \
            for (int j = 0; j < size<1>(); j++) {                                   \
                values[j] OP vals[j];                                               \
            }                                                                       \
            return *this;                                                           \
                                                                                    \
        } else {                                                                    \
            throw std::format("axis not supported: {}", axis);                      \
            return *this;                                                           \
        }                                                                           \
    }                                                                               \
    template <int axis = 0, class T2, class NT2>                                    \
    DataFrame& operator OP(const Array<T2, NT2>& vals)                              \
    {                                                                               \
        return operator OP<axis, T2>(vals.values);                                  \
    }                                                                               \
                                                                                    \
    template <int axis = 0, class IT2, class DT2, class INT2, class DNT2>           \
    DataFrame& operator OP(const Series<IT2, DT2, INT2, DNT2>& sr2)                 \
    {                                                                               \
        if constexpr (axis == 0) {                                                  \
            for (auto& sr : values) {                                               \
                sr OP sr2;                                                          \
            }                                                                       \
            return *this;                                                           \
                                                                                    \
        } else if constexpr (axis == 1) {                                           \
            for (auto& sr : values) {                                               \
                DNT& col_name = sr.get_name();                                      \
                if (sr2.pidx->has(col_name)) {                                      \
                    sr OP sr2.loc(col_name);                                        \
                } else {                                                            \
                    sr OP pandas::nan<DT>();                                        \
                }                                                                   \
            }                                                                       \
            return *this;                                                           \
                                                                                    \
        } else {                                                                    \
            throw std::format("axis not support: {}", axis);                        \
            return *this;                                                           \
        }                                                                           \
    }

DEFINE_DATAFRAME_OPERATOR(+=)
DEFINE_DATAFRAME_OPERATOR(-=)
DEFINE_DATAFRAME_OPERATOR(*=)
DEFINE_DATAFRAME_OPERATOR(/=)
DEFINE_DATAFRAME_OPERATOR(%=)
DEFINE_DATAFRAME_OPERATOR(&=)
DEFINE_DATAFRAME_OPERATOR(|=)
DEFINE_DATAFRAME_OPERATOR(^=)

//////////////////////// cmp operator /////////////////////////////////////////////////////////////
#define DEFINE_DATAFRAME_OPERATOR(OP)                                                       \
    template <class T2>                                                                     \
    DataFrame<IT, bool, INT, DNT> operator OP(const T2 & val) const                         \
    {                                                                                       \
        std::vector<Series<IT, bool, INT, DNT>> srs;                                        \
        for (auto& sr : values) {                                                           \
            auto sr2 = sr OP val;                                                           \
            srs.emplace_back(sr2);                                                          \
        }                                                                                   \
        return DataFrame<IT, bool, INT, DNT>(srs);                                          \
    }                                                                                       \
                                                                                            \
    template <int axis = 0, class T2>                                                       \
    DataFrame<IT, bool, INT, DNT> operator OP(const std::vector<T2>& vals)                  \
    {                                                                                       \
        if constexpr (axis == 0) {                                                          \
            std::vector<Series<IT, bool, INT, DNT>> srs;                                    \
            for (auto& sr : values) {                                                       \
                srs.push_back(sr OP vals);                                                  \
            }                                                                               \
            DataFrame<IT, bool, INT, DNT> res(srs);                                         \
            return res;                                                                     \
                                                                                            \
        } else if constexpr (axis == 1) {                                                   \
            if (vals.size() != size<1>()) {                                                 \
                throw std::format("size not match: {}:{}", vals.size(), size<1>());         \
            }                                                                               \
            std::vector<Series<IT, bool, INT, DNT>> srs;                                    \
            for (int j = 0; j < size<1>(); j++) {                                           \
                srs.push_back(values[j] OP vals[j]);                                        \
            }                                                                               \
            DataFrame<IT, bool, INT, DNT> res(srs);                                         \
            return res;                                                                     \
                                                                                            \
        } else {                                                                            \
            throw std::format("axis not supported: {}", axis);                              \
        }                                                                                   \
    }                                                                                       \
    template <int axis = 0, class T2, class NT2>                                            \
    DataFrame<IT, bool, INT, DNT> operator OP(const Array<T2, NT2>& vals)                   \
    {                                                                                       \
        return operator OP <axis, T2>(vals.values);                                          \
    }                                                                                       \
                                                                                            \
    template <int axis = 0, class IT2, class DT2, class INT2, class DNT2>                   \
    DataFrame<IT, bool, INT, DNT> operator OP(const Series<IT2, DT2, INT2, DNT2>& sr2)      \
    {                                                                                       \
        if constexpr (axis == 0) {                                                          \
            std::vector<Series<IT, bool, INT, DNT>> srs;                                    \
            for (auto& sr : values) {                                                       \
                srs.push_back(sr OP sr2);                                                   \
            }                                                                               \
            DataFrame<IT, bool, INT, DNT> res(srs);                                         \
            return res;                                                                     \
                                                                                            \
        } else if constexpr (axis == 1) {                                                   \
            std::vector<Series<IT, bool, INT, DNT>> srs;                                    \
            for (auto& sr : values) {                                                       \
                DNT& col_name = sr.get_name();                                              \
                if (sr2.pidx->has(col_name)) {                                              \
                    srs.push_back(sr OP sr2.loc(col_name));                                 \
                } else {                                                                    \
                    throw std::format("column not found: {}", pandas::to_string(col_name)); \
                }                                                                           \
            }                                                                               \
            DataFrame<IT, bool, INT, DNT> res(srs);                                         \
            return res;                                                                     \
                                                                                            \
        } else {                                                                            \
            throw std::format("axis not support: {}", axis);                                \
        }                                                                                   \
    }                                                                                       \
                                                                                            \
    template <class IT2, class DT2, class INT2, class DNT2>                                 \
    DataFrame<IT, bool, INT, DNT> operator OP(const DataFrame<IT2, DT2, INT2, DNT2>& val)   \
    {                                                                                       \
        std::vector<Series<IT, bool, INT, DNT>> srs;                                        \
        for (auto& sr : values) {                                                       \
            DNT col = sr.get_name();                                                        \
            int i = val.get_column_index(col);                                              \
            if (i < 0) {                                                                    \
                throw std::format("columns {} not found", pandas::to_string(col));          \
            }                                                                               \
            srs.push_back(sr OP val.values[i]);                                             \
        }                                                                                   \
        DataFrame<IT, bool, INT, DNT> res(srs);                                             \
        return res;                                                                         \
    }

DEFINE_DATAFRAME_OPERATOR(>)
DEFINE_DATAFRAME_OPERATOR(>=)
DEFINE_DATAFRAME_OPERATOR(<)
DEFINE_DATAFRAME_OPERATOR(<=)
DEFINE_DATAFRAME_OPERATOR(==)
DEFINE_DATAFRAME_OPERATOR(!=)
