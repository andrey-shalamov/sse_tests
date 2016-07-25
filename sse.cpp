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

using vec_type = std::vector<std::uint32_t>;

__attribute__((__noinline__))
void simple_add(const vec_type& a, const vec_type& b, vec_type& r)
{
    for (std::uint32_t i=0; i<a.size(); ++i)
    {
        r[i] = a[i] + b[i];
    }
}

__attribute__((__noinline__))
void sse_add(const vec_type& a, const vec_type& b, vec_type& r)
{
    const __m128i* aa = reinterpret_cast<const __m128i*>(a.data());
    const __m128i* bb = reinterpret_cast<const __m128i*>(b.data());
    __m128i* rr = reinterpret_cast<__m128i*>(r.data());
    for (std::uint32_t i = 0; i < a.size() / sizeof(__m128i); ++i)
    {
        rr[i] = _mm_add_epi32(aa[i], bb[i]);
    } 
}

template<typename... Args>
__attribute__((__noinline__))
void print_time(auto name, auto fn, Args... args)
{
    auto t1 = std::chrono::steady_clock::now();
    fn(args...);//(std::forward<Args>(args)...);
    auto t2 = std::chrono::steady_clock::now();
    using duration = std::chrono::duration<double, std::milli>;
    auto d = duration(t2-t1).count();
    std::cout << std::setw(10) << d  << " ms. - "<< name << std::endl;
}

void check(const auto& a, const auto& b)
{
    verify(a.size() == b.size());
    verify(0 == std::memcmp(a.data(), b.data()));
}

int main(int c, char** v)
{
    std::uint32_t n = std::stoi(v[1]);
    std::uint32_t m = std::stoi(v[2]);
    
    std::vector<std::uint32_t> a(n);
    std::vector<std::uint32_t> b(n);
    
    std::srand(0);
    auto rand_gen = []() { return std::rand(); };
    std::generate_n(a.begin(), n, rand_gen);
    std::generate_n(b.begin(), n, rand_gen);
    
    std::vector<std::uint32_t> r1(n);
    std::vector<std::uint32_t> r2(n);
    
    while (m > 0)
    {
        print_time("simple add", simple_add, a, b, r1);
        print_time("sse add", sse_add, a, b, r2);
        --m;
    }
    
    return 0;
}
