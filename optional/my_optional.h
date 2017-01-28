#ifndef MY_OPTIONAL
#define MY_OPTIONAL

#include <bits/stdc++.h>
#include <type_traits>

using namespace std;

static const struct nullopt_t
{
};
constexpr nullopt_t nt;

template <class T>
struct optional
{
    optional() : ok(false)
    {}

    optional(optional const& a) : ok(a.ok)
    {
        if (ok)
        {
            new (&stor) T (*a);
        }
    }

    optional(T const& a) : ok(true)
    {
        new (&stor) T(a);
    }

    optional(T &&a) : ok(true)
    {
        new (&stor) T(move(a));
    }

    optional(optional &&a) : ok(a.ok)
    {
        if (a)
        {
            new (&stor) T(move(*a));
        }
    }

    optional(nullopt_t) : ok(false)
    {
    }

    ~optional()
    {
        if (ok)
            (**this).T::~T();
    }

    optional& operator=(optional rhs)
    {
        swap(rhs);
        return *this;
    }

    void swap(optional& a)
    {
        if (!ok)
        {
            if (a.ok)
            {
                new (&stor) T(*a);
                a.data_ptr()->T::~T();
                a.ok = false, ok = true;
            }
        }
        else
        {
            if (!a.ok)
            {
                new (&a.stor) T(**this);
                data_ptr()->T::~T();
                a.ok = true, ok = false;
            }
            else
            {
                std::swap(**this, *a);
            }
        }
    }

    template <typename ... Args>
    void emplace(Args&& ...args)
    {
        if (ok)
        {
            data_ptr()->T::~T();
            ok = false;
        }
        new (&stor) T(forward<Args>(args)...);
        ok = true;
    }

    explicit operator bool() const
    {
        return ok;
    }

    T const& operator*() const
    {
        assert(ok);
        return *data_ptr();
    }

    T* operator->()
    {
        assert(ok);
        return data_ptr();
    }

    T& operator*()
    {
        assert(ok);
        return *data_ptr();
    }

    T const* operator->() const
    {
        assert(ok);
        return data_ptr();
    }

    T* data_ptr()
    {
        return reinterpret_cast<T*>(&stor);
    }

    T const* data_ptr() const
    {
        return reinterpret_cast<T const*>(&stor);
    }

private:
    typename aligned_stor<sizeof(T), alignof(T)>::type stor;
    bool ok;

    template <typename U, typename ... Args>
    friend optional<U> make_optional(Args&& ...args);

    friend bool operator==(optional const& a, optional const& b)
    {
        if (a.ok != b.ok)
            return false;

        if (!a.ok)
            return true;

        return *a == *b;
    }

    friend bool operator!=(optional const& a, optional const& b)
    {
        return !(a == b);
    }

    friend bool operator<(optional const& a, optional const& b)
    {
        if (a)
        {
            if (b)
                return *a < *b;
            else
                return false;
        }
        else
        {
            if (b)
                return true;
            else
                return false;
        }
    }

    friend bool operator>(optional const& a, optional const& b)
    {
        return b < a;
    }

    friend bool operator<=(optional const& a, optional const& b)
    {
        return a < b || a == b;
    }

    friend bool operator>=(optional const& a, optional const& b)
    {
        return a > b || a == b;
    }
};

template <typename T, typename ... Args>
optional<T> make_optional(Args&& ...args)
{
    optional<T> result;
    result.emplace(forward<Args>(args)...);
    return result;
}

#endif // MY_OPTIONAL
