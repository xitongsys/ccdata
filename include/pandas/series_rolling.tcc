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
        Series<IT, int, INT, DNT> res(sr.get_name());
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
            res._append(id, cnt);
        }
        return res;
    }

    Series sum()
    {
        Series res(sr);
        int n = sr.size();
        DT s = 0;
        int cnt = 0;
        for (int i = 0; i < n; i++) {
            DT v = sr.iloc(i);
            int j = i - window;
            if (!pandas::isnan(v)) {
                s += v;
                cnt++;
            }

            if (j >= 0 && !pandas::isnan(sr.iloc(j))) {
                cnt--;
                s -= sr.iloc(j);
            }

            if (cnt >= min_periods) {
                res.iloc_ref(i) = s;
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

        Series<IT, double, INT, DNT> res(sr.get_name());
        for (int i = 0; i < sr.size(); i++) {
            IT id = sr.pidx->iloc(i);
            double c = cs.iloc(i), s = ss.iloc(i);
            res._append(id, s / c);
        }
        return res;
    }

    Series<IT, double, INT, DNT> var()
    {
        Series mns = mean();
        Series<IT, double, INT, DNT> res(sr.get_name());

        for (int i = 0; i < sr.size(); i++) {
            IT id = sr.pidx->iloc(i);
            DT v = sr.iloc(i);
            double mn = mns.iloc(i);
            double va = (v - mn) * (v - mn);
            res._append(id, va);
        }

        return res.rolling(window, min_periods).mean();
    }

    Series<IT, double, INT, DNT> std()
    {
        return var().pow(0.5);
    }

    ////////// common agg /////////////////////

    template <class DT2>
    Series<IT, DT2, INT, DNT> agg(std::function<DT2(SeriesVisitor<Range<int>>&)> const& func)
    {
        Series<IT, DT2, INT, DNT> res(sr);

        for (int i = 0; i < sr.size(); i++) {
            int b = std::max(0, i - window + 1), e = i;
            SeriesVisitor<Range<int>> sv(sr, Range<int>(b, e + 1));
            if (min_periods > 0 && sv.count() < min_periods) {
                res.iloc_ref(i) = pandas::nan<DT2>();
            } else {
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