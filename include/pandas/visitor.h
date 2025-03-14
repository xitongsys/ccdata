namespace pandas {

template <class T>
class Visitor {
public:
    virtual T& iloc(int i) = 0;
    virtual T iloc(int i) const = 0;
    virtual size_t size() const = 0;
    virtual int count() const = 0;

    T sum() const
    {
        T s {};
        for (int i = 0; i < size(); i++) {
            s += iloc(i);
        }
        return s;
    }
};
}