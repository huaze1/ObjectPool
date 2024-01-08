
#ifndef INT_HPP
#define INT_HPP
#include<iostream>
#include<thread>
#include<string>
using namespace std;
class Int
{
private:
    std::string idname;
	int value;
public:
    Int(int x) :value(x) 
	{ 
		cout<<"Create Int() Object"<<this<<endl;
	}
	Int(const std::string &name,int x) 
		:idname(name),
		value(x) 
	{ 
		cout << "Create Int(int) Object " <<idname << " : "<<value<<endl;
	}
	~Int() 
	{ 
		cout << "Destroy Int Object " <<idname<<" : "<<value<< endl;
	}
	Int(const Int& it) :value(it.value) 
	{ 
		cout << this<<" Copy Create Int Object " <<&it<< endl; 
	}
	Int& operator=(const Int& it)
	{
		if (this != &it)
		{
			value = it.value;
		}
		cout <<this<< " operator(const Int &) " <<&it<< endl;
		return *this;
	}
	Int(Int&& it) :value(it.value)
	{
		cout << "Move Copy Create Int Object " <<idname<<" : "<<value<< endl;
	}
	Int& operator=(Int&& it)
	{
		value = it.value;
		cout << this<< " Move operator(Int &) " << &it<< endl;
		return *this;
	}
	void Print() const
	{
		std::this_thread::sleep_for(std::chrono::seconds(rand()%10));
		cout<<idname<<" : "<<value<<endl;
	}
	void SetValue(int x) { value = x; }
    void Clear() { value = -1;}
	int GetValue() const { return value; }
	Int operator+(const Int& it) const
	{
		return Int(this->value + it.value);
	}
	ostream& operator<<(ostream& out) const
	{
		out << value;
		return out;
	}
	istream& operator>>(istream& in)
	{
		in >> value;
		return in;
	}
};
ostream& operator<<(ostream& out, const Int& it)
{
	return it << out;
}
istream& operator>>(istream& in, Int& it)
{
	it >> in;
	return in;
}

#endif 