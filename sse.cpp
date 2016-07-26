// g++ --std=c++14 -O3 -DNDEBUG sse.cpp -msse4.2 -o t

#include <iostream>
#include <vector>
#include <array>
#include <chrono>
#include <algorithm>
#include <cstring>
#include <iomanip>
#include <functional>

#include <x86intrin.h>

void verify(bool b)
{
    if (not b) throw "error";
}

template<typename Func, typename... Args>
void print_time(auto name, Func&& fn, Args&&... args)
{
    auto t1 = std::chrono::steady_clock::now();
    fn(std::forward<Args>(args)...);
    auto t2 = std::chrono::steady_clock::now();
    using duration = std::chrono::duration<double, std::milli>;
    auto d = duration(t2-t1).count();
    std::cout << std::setw(10) << d  << " ms. - "<< name << std::endl;
}

using value_type = std::uint32_t;
using vec_type = std::vector<value_type>;

constexpr std::uint8_t factor = 4; 

void check_vec(const vec_type& a, const vec_type& b)
{
    verify(a.size() == b.size());
    verify(0 == std::memcmp(a.data(), b.data(), a.size() * sizeof(typename vec_type::value_type)));
}

//------------------------------------------------------------------------------
// add
//------------------------------------------------------------------------------

__attribute__((__noinline__))
void simple_add(const vec_type& a, const vec_type& b, vec_type& r)
{
    for (std::uint32_t i=0; i<a.size(); ++i)
    {
        r[i] = a[i] + b[i];
    }
}

__attribute__((__noinline__))
void std_add(const vec_type& a, const vec_type& b, vec_type& r)
{
    std::transform(a.begin(), a.end(), b.begin(), r.begin(), std::plus<value_type>());
}

__attribute__((__noinline__))
void sse_add(const vec_type& a, const vec_type& b, vec_type& r)
{
    const __m128i* aa = reinterpret_cast<const __m128i*>(a.data());
    const __m128i* bb = reinterpret_cast<const __m128i*>(b.data());
    __m128i* rr = reinterpret_cast<__m128i*>(r.data());
    for (std::uint32_t i = 0; i < a.size() / factor; ++i)
    {
        rr[i] = _mm_add_epi32(aa[i], bb[i]);
    } 
}

void test_add(const vec_type& a, const vec_type& b, std::uint32_t n, std::uint32_t m)
{
    std::vector<value_type> r1(n);
    std::vector<value_type> r2(n);
    std::vector<value_type> r3(n);
    
    while (m > 0)
    {
        print_time("simple add", simple_add, a, b, r1);
        print_time("std add", std_add, a, b, r2);
        print_time("sse add", sse_add, a, b, r3);
        --m;
        
        check_vec(r1, r2);
        check_vec(r1, r3);
    }
}

//------------------------------------------------------------------------------
// min
//------------------------------------------------------------------------------

__attribute__((__noinline__))
void simple_min_element(const vec_type& a, value_type& r)
{
    r = std::numeric_limits<value_type>::max();
    for (std::uint32_t i = 0; i < a.size(); ++i)
    {
        if (a[i] < r)
            r = a[i];
    }    
}

__attribute__((__noinline__))
void std_min_element(const vec_type& a, value_type& r)
{
    r = *std::min_element(a.begin(), a.end());
}

__attribute__((__noinline__))
void sse_min_element(const vec_type& a, value_type& r)
{
    constexpr int max_int = 0x7FFFFFFF;
    __m128i min = _mm_set_epi32(max_int, max_int, max_int, max_int);
    const __m128i* aa = reinterpret_cast<const __m128i*>(a.data());    
    for (std::uint32_t i = 0; i < a.size() / factor; ++i)
    {
        min = _mm_min_epi32(min, aa[i]);
    }
    const value_type* ui32 = reinterpret_cast<const value_type*>(&min);
    r = *std::min_element(ui32, ui32 + factor);
}

void test_min(const vec_type& a, std::uint32_t n, std::uint32_t m)
{
    std::uint32_t r1 = 0;
    std::uint32_t r2 = 0;
    std::uint32_t r3 = 0;
    while (m > 0)
    {
        print_time("simple_min_element", simple_min_element, a, r1);
        print_time("std_min_element", std_min_element, a, r2);
        print_time("sse_min_element", sse_min_element, a, r3);
        --m;
        
        verify(r1 == r2);
        verify(r1 == r3);
    }
}

//------------------------------------------------------------------------------

int main(int c, char** v)
{    
    if (c != 4)
    {
        std::cerr << "you must set 3 arguments." << std::endl;
        return -1;
    }
    std::uint32_t n = std::stoi(v[1]) * factor;
    std::uint32_t m = std::stoi(v[2]);
    std::uint32_t test = std::stoi(v[3]);
    
    std::vector<std::uint32_t> a(n);
    std::vector<std::uint32_t> b(n);
    
    std::srand(0);
    auto rand_gen = []() { return std::rand(); };
    //auto rand_gen = []() { static std::uint32_t x = 1; return x++; };
    std::generate_n(a.begin(), n, rand_gen);
    std::generate_n(b.begin(), n, rand_gen);
    
    switch (test)
    {
    case 1:
        test_add(a, b, n, m);
        break;
    case 2:
        test_min(a, n, m);
        break;
    default:
        std::cout << "There is no test with id " << test << std::endl;
    }
    
    return 0;
}
