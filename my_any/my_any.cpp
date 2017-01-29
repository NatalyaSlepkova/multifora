#include <typeinfo.h>
#include <algorithm>
#include <string>
#include <memory.h>
#include <fstream>
#include <iostream>

class my_any
{
public:
	my_any() : held_(0)
	{
	}

	template<typename T>
	my_any(const T& t) : held_(new holder<T>(t))
	{}

	~my_any()
	{
	    delete held_;
    }

    my_any(const my_any & a) : held_(a.held_ ? a.held_->same() : 0)
    {
    }

    my_any& swap(my_any& a)
    {
        std::swap(held_, a.held_);
        return *this;
    }

    bool empty() const
    {
        return !(held_ != nullptr);
    }

    void clear()
    {
        my_any().swap(*this);
    }

	template <typename T>
	my_any operator=(T&& t)
	{
		my_any(std::move(t).swap(*this));
		return *this;
	}

	my_any operator=(my_any&& a)
	{
		a.swap(*this);
		return *this;
	}

	template<typename U>
	U cast() const
	{
		if(typeid(U) != held_->type_info())
        {
			throw std::runtime_error("Bad any cast");
        }
		return static_cast<holder<U>* >(held_)->t_;
	}

	template<typename T> friend T* my_any_cast(my_any*);
    template<typename T> friend const T* my_any_cast(const my_any*);
    template<typename T> friend T my_any_cast(my_any& a);
    template<typename T> friend T my_any_cast(const my_any& a);
    template<typename T> friend T my_any_cast(my_any&& a);

private:
	struct base_holder
	{
		virtual ~base_holder(){}
		virtual const std::type_info& type_info() const = 0;
        virtual base_holder* same() const = 0;
	};

public:
	template<typename T> struct holder : base_holder
	{
		holder(const T& t) : t_(t){}

		holder(T&& t) : t_(static_cast<T&&>(t)) {}

		const std::type_info& type_info() const
		{
			return typeid(t_);
		}

		base_holder* same() const
		{
		    return new holder(t_);
		}
		T t_;
	};
private:
	base_holder* held_;
};

template <typename T>
T* any_cast(my_any* a) 
{
    return oper && a->type_info() == typeid(T) ? &static_cast<my_any::holder<typename std::remove_cv<T>::type>*>(a->held_.get())->held_ : 0;
}

template <typename T>
const T* my_any_cast(const my_any* a) 
{
    return my_any_cast<T>(const_cast<my_any*>(a));
}

template <typename T>
T my_any_cast(my_any& a) 
{
    typedef typename std::remove_reference<T>::type t;
    t* ans = my_any_cast<t>(&a);
    return *ans;
}

template <typename T>
T my_any_cast(const my_any& a) 
{
    typedef typename std::remove_reference<T>::type t;
    return my_any_cast<const t&>(const_cast<my_any&>(a));
}

template <typename T>
T my_any_cast(my_any&& a)
{
    return my_any_cast<T>(a);
}

int main()
{
	//std::ofstream cout("answer.out");
	my_any a(3);
	my_any b(std::string("abc"));
	//a.swap(b);
	//a = b;
	//a.swap(b);

	/*if (a.empty())
	{
		std::cout << "yes"; 
	}*/
	try
	{
		std::cout << a.cast<int>() << std::endl;
	}
	catch(const std::exception& e)
	{
		std::cout << e.what() << "\n";
	}
	int d;
	std::cin >> d;
	
	/*my_any b(std::string("abcd"));
	try
	{
		std::cout << b.cast<double>() << std::endl;
	}
	catch(const std::exception& e)
	{
		std::cout << e.what() << "\n";
	}*/
	return 0;
}
