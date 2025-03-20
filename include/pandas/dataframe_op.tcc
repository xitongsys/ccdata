#pragma once

// template<class IT, class DT>
// class Series {

#define DEFINE_DATAFRAME_OPERATOR(OP)                                              \
    template <class T2>                                                            \
    DataFrame operator OP(const T2& val)                                           \
    {                                                                              \
        DataFrame res = *this;                                                     \
        for (auto& sr : res.values) {                                              \
            sr = sr OP val;                                                        \
        }                                                                          \
        return res;                                                                \
    }                                                                              \
                                                                                   \
    template <class IT2, class DT2, class INT2, class DNT2>                        \
    DataFrame operator OP(const DataFrame<IT2, DT2, INT2, DNT2>& val)              \
    {                                                                              \
        DataFrame res = *this;                                                     \
        for (auto& sr : res.values) {                                              \
            DNT col = sr.get_name();                                               \
            int i = val.get_column_index(col);                                     \
            if (i < 0) {                                                           \
                throw std::format("columns {} not found", pandas::to_string(col)); \
            }                                                                      \
            sr = sr OP val.values[i];                                              \
        }                                                                          \
        return res;                                                                \
    }

DEFINE_DATAFRAME_OPERATOR(+)
DEFINE_DATAFRAME_OPERATOR(-)
DEFINE_DATAFRAME_OPERATOR(*)
DEFINE_DATAFRAME_OPERATOR(/)
DEFINE_DATAFRAME_OPERATOR(%)
DEFINE_DATAFRAME_OPERATOR(&)
DEFINE_DATAFRAME_OPERATOR(|)
DEFINE_DATAFRAME_OPERATOR(^)

#define DEFINE_DATAFRAME_OPERATOR(OP)                                              \
    template <class T2>                                                            \
    DataFrame& operator OP(const T2 & val)                                         \
    {                                                                              \
        for (auto& sr : values) {                                                  \
            sr OP val;                                                             \
        }                                                                          \
        return *this;                                                              \
    }                                                                              \
                                                                                   \
    template <class IT2, class DT2, class INT2, class DNT2>                        \
    DataFrame& operator OP(const DataFrame<IT2, DT2, INT2, DNT2>& val)             \
    {                                                                              \
        for (auto& sr : values) {                                                  \
            DNT col = sr.get_name();                                               \
            int i = val.get_column_index(col);                                     \
            if (i < 0) {                                                           \
                throw std::format("columns {} not found", pandas::to_string(col)); \
            }                                                                      \
            sr OP val.values[i];                                                   \
        }                                                                          \
        return *this;                                                              \
    }

DEFINE_DATAFRAME_OPERATOR(+=)
DEFINE_DATAFRAME_OPERATOR(-=)
DEFINE_DATAFRAME_OPERATOR(*=)
DEFINE_DATAFRAME_OPERATOR(/=)
DEFINE_DATAFRAME_OPERATOR(%=)
DEFINE_DATAFRAME_OPERATOR(&=)
DEFINE_DATAFRAME_OPERATOR(|=)
DEFINE_DATAFRAME_OPERATOR(^=)

#define DEFINE_DATAFRAME_OPERATOR(OP) \
    DataFrame operator OP() const     \
    {                                 \
        DataFrame res = *this;        \
        for (auto& sr : res.values) { \
            sr = OP sr;               \
        }                             \
        return res;                   \
    }

DEFINE_DATAFRAME_OPERATOR(~)

/////////// cmp operator /////////////////
#define DEFINE_DATAFRAME_OPERATOR(OP)                                                     \
    template <class T2>                                                                   \
    DataFrame<IT, bool, INT, DNT> operator OP(const T2 & val) const                       \
    {                                                                                     \
        std::vector<Series<IT, bool, INT, DNT>> srs;                                      \
        for (auto& sr : values) {                                                         \
            auto sr2 = sr OP val;                                                         \
            srs.emplace_back(sr2);                                                        \
        }                                                                                 \
        return DataFrame<IT, bool, INT, DNT>(srs);                                        \
    }                                                                                     \
                                                                                          \
    template <class IT2, class DT2, class INT2, class DNT2>                               \
    DataFrame<IT, bool, INT, DNT> operator OP(const DataFrame<IT2, DT2, INT2, DNT2>& val) \
    {                                                                                     \
        DataFrame<IT, bool, INT, DNT> res = *this;                                        \
        for (auto& sr : res.values) {                                                     \
            DNT col = sr.get_name();                                                      \
            int i = val.get_column_index(col);                                            \
            if (i < 0) {                                                                  \
                throw std::format("columns {} not found", pandas::to_string(col));        \
            }                                                                             \
            sr = sr OP val.values[i];                                                     \
        }                                                                                 \
        return res;                                                                       \
    }

DEFINE_DATAFRAME_OPERATOR(>)
DEFINE_DATAFRAME_OPERATOR(>=)
DEFINE_DATAFRAME_OPERATOR(<)
DEFINE_DATAFRAME_OPERATOR(<=)
DEFINE_DATAFRAME_OPERATOR(==)
DEFINE_DATAFRAME_OPERATOR(!=)

#define DEFINE_DATAFRAME_ROW_OPERATOR(FUN, OP)                               \
    template <class DT2, class DNT2>                                         \
    DataFrame FUN##_row(const Array<DT2, DNT2>& ar)                          \
    {                                                                        \
        if (ar.size() != size(1)) {                                          \
            throw std::format("size not mathch {}!={}", ar.size(), size(1)); \
        }                                                                    \
                                                                             \
        DataFrame res = *this;                                               \
        for (int i = 0; i < size(0); i++) {                                  \
            for (int j = 0; j < size(1); j++) {                              \
                res.iloc_ref(i, j) = res.iloc(i, j) OP ar.iloc(j);           \
            }                                                                \
        }                                                                    \
        return res;                                                          \
    }                                                                        \
                                                                             \
    template <class IT2, class DT2, class INT2, class DNT2>                  \
    DataFrame FUN##_row(const Series<IT2, DT2, INT2, DNT2>& sr)              \
    {                                                                        \
        if (sr.size() != size(1)) {                                          \
            throw std::format("size not mathch {}!={}", sr.size(), size(1)); \
        }                                                                    \
                                                                             \
        DataFrame res = *this;                                               \
        for (int j = 0; j < size(1); j++) {                                  \
            IT id = res.values[j].get_name();                                \
            DT2 val = sr.loc(id);                                            \
            values[j] = values[j] OP val;                                    \
        }                                                                    \
        return res;                                                          \
    }

DEFINE_DATAFRAME_ROW_OPERATOR(add, +)
DEFINE_DATAFRAME_ROW_OPERATOR(subtract, -)
DEFINE_DATAFRAME_ROW_OPERATOR(multiply, *)
DEFINE_DATAFRAME_ROW_OPERATOR(div, /)
DEFINE_DATAFRAME_ROW_OPERATOR(mod, %)