#pragma once

// template<class IT, class DT, class INT, class DNT>
// class Series {

class SeriesRolling {
public:
    Series& sr;
    int window, min_periods;

    SeriesRolling(const SeriesRolling& sr2)
        : sr(sr2.sr)
        , window(sr2.window)
        , min_periods(sr2.min_periods)
    {
    }

    SeriesRolling(SeriesRolling&& sr2)
        : sr(sr2.sr)
        , window(sr2.window)
        , min_periods(sr2.min_periods)
    {
    }

    SeriesRolling(Series& sr_, int window_, int min_periods_)
        : sr(sr_)
        , window(window_)
        , min_periods(min_periods_)
    {
    }

    /// rolling functions

    Series<IT, int, INT, DNT> count()
    {
        Array<IT, INT> ar_idx(sr.pidx->get_name());
        Array<int, DNT> ar_val(sr.get_name());
        int n = sr.size();
        int cnt = 0;
        for (int i = 0; i < n; i++) {
            IT id = sr.pidx->iloc(i);
            DT v = sr.iloc(i);
            int j = i - window;
            if (!pandas::isnan(v)) {
                cnt++;
            }

            if (j >= 0 && !pandas::isnan(sr.iloc(j))) {
                cnt--;
            }

            ar_idx._append(id);
            ar_val._append(cnt);
        }
        return Series<IT, int, INT, DNT>(
            std::move(Index<IT, INT>(std::move(ar_idx))),
            std::move(ar_val));
    }

    Series sum()
    {
        Series res(sr);
        int n = sr.size();
        DT s = 0;
        int cnt = 0;
        for (int i = 0; i < n; i++) {
            DT v = sr.iloc(i);
            if (!pandas::isnan(v)) {
                s += v;
                cnt++;
            }

            int j = i - window;
            if (j >= 0 && !pandas::isnan(sr.iloc(j))) {
                cnt--;
                s -= sr.iloc(j);
            }

            if (cnt >= min_periods) {
                res.iloc_ref(i) = s;
            } else {
                res.iloc_ref(i) = pandas::nan<DT>();
            }
        }
        return res;
    }

    Series max()
    {
        std::multiset<DT> st;
        Series res(sr);
        int n = sr.size();
        if (n == 0) {
            return res;
        }
        int cnt = 0;
        for (int i = 0; i < n; i++) {
            DT v = sr.iloc(i);
            int j = i - window;
            if (!pandas::isnan(v)) {
                st.insert(v);
                cnt++;
            }

            if (j >= 0 && !pandas::isnan(sr.iloc(j))) {
                DT vj = sr.iloc(j);
                st.erase(st.find(vj));
                cnt--;
            }

            if (cnt >= min_periods) {
                res.iloc_ref(i) = *st.rbegin();
            } else {
                res.iloc_ref(i) = pandas::nan<DT>();
            }
        }
        return res;
    }

    Series min()
    {
        std::multiset<DT> st;
        Series res(sr);
        int n = sr.size();
        if (n == 0) {
            return res;
        }
        int cnt = 0;
        for (int i = 0; i < n; i++) {
            DT v = sr.iloc(i);
            int j = i - window;
            if (!pandas::isnan(v)) {
                st.insert(v);
                cnt++;
            }

            if (j >= 0 && !pandas::isnan(sr.iloc(j))) {
                DT vj = sr.iloc(j);
                st.erase(st.find(vj));
                cnt--;
            }

            if (cnt >= min_periods) {
                res.iloc_ref(i) = *st.begin();
            } else {
                res.iloc_ref(i) = pandas::nan<DT>();
            }
        }
        return res;
    }

    Series<IT, double, INT, DNT> mean()
    {
        Series<IT, int, INT, DNT> cs = count();
        Series ss = sum();

        Array<IT, INT> ar_idx(sr.pidx->get_name());
        Array<DT, DNT> ar_val(sr.get_name());

        for (int i = 0; i < sr.size(); i++) {
            IT id = sr.pidx->iloc(i);
            double c = cs.iloc(i), s = ss.iloc(i);
            ar_idx._append(id);
            ar_val._append(s / c);
        }

        return Series<IT, double, INT, DNT>(
            std::move(Index<IT, INT>(std::move(ar_idx))),
            std::move(ar_val));
    }

    Series<IT, double, INT, DNT> var()
    {
        Series mns = mean();
        Array<IT, INT> ar_idx(sr.pidx->get_name());
        Array<DT, DNT> ar_val(sr.get_name());

        for (int i = 0; i < sr.size(); i++) {
            IT id = sr.pidx->iloc(i);
            DT v = sr.iloc(i);
            double mn = mns.iloc(i);
            double va = (v - mn) * (v - mn);
            ar_idx._append(id);
            ar_val._append(va);
        }

        return Series<IT, double, INT, DNT>(
            std::move(Index<IT, INT>(std::move(ar_idx))),
            std::move(ar_val));
    }

    Series<IT, double, INT, DNT> std()
    {
        return var().pow(0.5);
    }

    ////////// general agg func /////////////////////

    template <class DT2>
    Series<IT, DT2, INT, DNT> agg(std::function<DT2(SeriesVisitor<Range<int>>&)> const& func)
    {
        Series<IT, DT2, INT, DNT> res(sr.get_name(), *(sr.pidx), DT2 {});

        for (int i = 0; i < sr.size(); i++) {
            int b = std::max(0, i - window + 1), e = i;
            SeriesVisitor<Range<int>> sv(sr, Range<int>(b, e + 1));
            if (min_periods > 0 && sv.count() < min_periods) {
                res.iloc_ref(i) = pandas::nan<DT2>();
            } else {
                sv.reset();
                res.iloc_ref(i) = func(sv);
            }
        }
        return res;
    }

#define DEFINE_SERIESROLLING_AGG_FUNC(TYPE, FUN)                                                                                  \
    Series<IT, TYPE, INT, DNT> FUN()                                                                                              \
    {                                                                                                                             \
        return agg<TYPE>([](SeriesVisitor<Range<int>>& sv) -> TYPE { return pandas::FUN<TYPE, SeriesVisitor<Range<int>>>(sv); }); \
    }
    DEFINE_SERIESROLLING_AGG_FUNC(double, median)
};

SeriesRolling rolling(int window, int min_periods)
{
    return SeriesRolling(*this, window, min_periods);
}