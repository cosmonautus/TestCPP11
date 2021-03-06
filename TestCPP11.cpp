#include "pch.h"
#include <iostream>
#include <functional>
#include <stack>
#include <thread>
#include <chrono>
#include <control.h>
#include "threadpool.hpp"
#include <future>
#include <numeric>
#include <sstream>
#include <algorithm>

ThreadPool pool(2);

using fnType = std::function<void()>;

class FunctionObjectType
{
public:
    void operator() ()
    {
        std::cout << "[o] Hi!\n";
    }
};

void testFunc(char c)
{
    std::cout << "[f] Hello, " << c << "!\n";
}

void test1()
{
    std::stack<fnType> q;
    //
    std::cout << "[-] thread id=" << std::this_thread::get_id() << std::endl;
    // add functional object
    FunctionObjectType fo;
    q.push(fo);
    // add function
    q.push(std::bind(testFunc, 'a'));
    q.push(std::bind(testFunc, 'b'));
    // add lambda
    for (int i = 0; i < 10; i++)
    {
        q.push(
            [i]() 
            {
                std::cout << "[-] lambda" << i << std::endl;
            }
        );
    }
    // exec
    while (!q.empty())
    {
        fnType f = q.top();
        q.pop();
        f();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

int test2(int no, size_t count)
{
    std::cout << "[" << no << "] start " << count << " elements\n";
    std::vector<int> v(count);
    for (size_t i = 0; i < v.size(); i++)
        v[i] = rand();
    std::sort(v.begin(), v.end());
    std::cout << "[" << no << "] ";
    for (size_t i = 0; i < 20; i++)
        std::cout << v[i] << " ";
    std::cout << "\n[" << no << "] stop\n";
    return 42;
}

class First
{
public:
    int a;
    int b;
};
using pFirst = std::shared_ptr<First>;

class Second : public First
{
public:
    int c;
};
using pSecond = std::shared_ptr<Second>;

#include "slist.hpp"

template<typename T>
class X
{
    T value;
public:
    X() : X{0}
    {
        std::cout << "X[" << value << "]->default constructor\n";
    }
    X(T a) : value(a)
    {
        std::cout << "X[" << value << "]->constructor\n";
    }
    ~X()
    {
        std::cout << "X[" << value << "]->destructor\n";
    }
    X(const X& a) : value(a.value)
    {
        std::cout << "X[" << value << "]->copy constructor\n";
    }
    X<T>& operator=(const X<T>& a) = delete;
/*    {
        if (this != &a)
        {
            std::cout << "Operator = X\n";
            value = a.value;
        }
        else
            std::cout << "Operator self = X\n";
        return *this;
    }*/
    void test()
    {
        std::cout << "X[" << value << "]->test()\n";
    }
};


constexpr int sum(std::initializer_list<int> list)
{
    int res = 0;
    for (auto v : list)
        res += v;
    return res;
};

using IX = X<int>;
using pIX = std::shared_ptr<IX>;

template<typename T, typename... ARGS>
std::shared_ptr<T> makeSharedPtr(ARGS... args)
{
    return std::make_shared<T>(args...);
}

void testSharedPtr()
{
    std::shared_ptr<int> pI = makeSharedPtr<int>(55);
    std::shared_ptr<int> pI2 = pI;
    if (*pI2 == 55)
        std::cout << "Ok.\n";
    pIX pix = makeSharedPtr<IX>();
    pix->test();
}

// литерал для std::string
std::string operator""_s(const char* p, size_t n)
{
    return std::string(p, n); // требуется динамическое выделение памяти
}

std::atomic_flag atomF = ATOMIC_FLAG_INIT;

#include <iomanip>

int rangeCheck(int v)
{
    if (v < 0) return 0;
    if (v > 255) return 255;
    return v;
}

class Slava
{
public:
    Slava() { std::cout << "{*}"; }
    virtual ~Slava() { std::cout << "[=]"; }
};


#include <set>


int main()
{    
    std::vector<std::shared_ptr<Slava>> vslava;
    for (int i : {0, 1, 2, 3, 4})
        vslava.push_back(std::make_shared<Slava>());

    std::set<int> vint = { 10, 6, 3, 2, 7, 9, 1 };
    vint.insert(20);
    vint.insert(0);
    if (*(vint.begin()) == 1)
        std::cout << "set ok\n";
    std::stringstream shex;
    shex << std::uppercase << std::setfill('0');
    shex << std::setw(2) << std::hex << min(255, max(0, 10));
    shex << std::setw(2) << std::hex << 11;
    shex << std::setw(2) << std::hex << 12;

    std::string _s = shex.str();

    atomF.test_and_set();
    auto retAsync = std::async(test2, 66, 123456);
    std::cout << "std::async --> " << retAsync.get() << std::endl;
    std::vector<int> v1{ 54, 6, 37, 98, 9, 0 };
    auto acc = std::accumulate(v1.begin(), v1.end(), 0);
    std::vector<int> v2{ 1, 2, 53, 14, 5, 16 };
    std::sort(v1.begin(), v1.end());
    std::sort(v2.begin(), v2.end());
    std::swap(v1, v2);

    auto ss = "Super puper std::string"_s;

    testSharedPtr();
    constexpr int s = sum({ 435, 324, 34, 45, 5476, 657 });
    X<int> a = s;
    X<int> b(8);
    X<int> c = b;

    auto p = std::minmax_element(v1.begin(), v1.end());

//    a = a;
    //a = c;
    Slist<int> slst1;
    Slist<int> slst2;
    slst1.append(5);
    slst1.append(6);
    slst1.append(7);
    if (slst1.get() == 5)
        if (slst1.get() == 6)
            if (slst1.get() == 7)
                std::cout << "Hi!\n";

    /*	pool.runAsync([]() {
            for (int i = 0; i < 20; i++)
            {
                std::cout << "Thread id " << std::this_thread::get_id() << " out " << i << std::endl;
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
        }
        );*/
        //pool.runAsync(test1);
    auto res = pool.runAsync<int>(test2, 1, 100000);
    pool.runAsync(test1);
    pool.runAsync(test2, 2, 100000);
    while (!res->ready)
        ;
    std::cout << "runAsync(test2) --> " << res->data << std::endl;
}
