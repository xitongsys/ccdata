#pragma once

// template <class T, class NT = std::string>
// class Index : public Index<T, NT> {

// template <class T2, class NT2, class IIT>
// class IndexVisitor;

template <class IIT>
class IndexVisitor {
public:
    Index& si;
    IIT it;
    std::vector<int> iids;

    IndexVisitor(Index& si_, const IIT& it_)
        : si(si_)
        , it(it_)
    {
    }

    inline bool has_left()
    {
        return it.has_left();
    }

    inline T next()
    {
        if (!has_left()) {
            PANDAS_THROW(std::format("no left data"));
        }

        int i = it.next();
        it++;
        return si.iloc(i);
    }

    inline T& next_ref()
    {
        if (!has_left()) {
            PANDAS_THROW(std::format("no left data"));
        }

        int i = it.next();
        return si.iloc_ref(i);
    }

    void reset()
    {
        it.reset();
    }

    Index to_index()
    {
        Index si2;
        it.reset();
        while (it.has_left()) {
            si2._append(it.next());
        }
        si2._rename(si2.get_name());
        return si2;
    }

    Array<T, NT> to_array()
    {
        Array<T, NT> ar;
        it.reset();
        while (it.has_left()) {
            ar._append(it.next());
        }
        ar._rename(si.get_name());
        return ar;
    }

    template <class T2>
    void operator=(const T2& v)
    {
        it.reset();
        while (it.has_left()) {
            next_ref() = v;
        }
        si._update_index();
    }

    template <class DT2, class NT2>
    void operator=(const Array<DT2, NT2>& ar)
    {
        it.reset();
        for (int i = 0; i < ar.size() && it.has_left(); i++) {
            it.next_ref() = ar.iloc(i);
        }
        si._update_index();
    }

    template <class DT2, class NT2>
    void operator=(const Index<DT2, NT2>& ar)
    {
        (*this) = ar.values;
    }

    template <class T2, class NT2, class IIT2>
    void operator=(const typename Index<T2,NT2>::template IndexVisitor<IIT2>& ar)
    {
        (*this) = ar.values;
    }

    // functions that need iids

    void extract_iids()
    {
        if (iids.size() == 0) {
            it.reset();
            while (it.has_left()) {
                int i = it.next();
                iids.push_back(i);
            }
        }
    }

    size_t size()
    {
        extract_iids();
        return iids.size();
    }

    inline T iloc(int i)
    {
        extract_iids();
        if (i > iids.size()) {
            PANDAS_THROW(std::format("i = {} overflow size = {}", i, size()));
        }
        i = iids[i];
        return si.iloc(i);
    }

    inline T& iloc_ref(int i)
    {
        extract_iids();
        if (i > iids.size()) {
            PANDAS_THROW(std::format("i = {} overflow size = {}", i, size()));
        }
        i = iids[i];
        return si.iloc_ref(i);
    }
};
