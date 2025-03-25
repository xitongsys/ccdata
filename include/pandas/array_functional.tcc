#pragma once

// template<class T, class NT>
// class Array {
template <class T2>
Array<T2, NT> map(std::function<T2(const T&)> const& func) const
{
    Array<T2, NT> ar(get_name());
    for (int i = 0; i < size(); i++) {
        T val = iloc(i);
        T2 mval = func(val);
        ar._append(mval);
    }
    return ar;
}
template <class T2>
void _map(std::function<T2(const T&)> const& func)
{
    for (int i = 0; i < size(); i++) {
        T val = iloc(i);
        T2 mval = func(val);
        iloc_ref(i) = mval;
    }
}
