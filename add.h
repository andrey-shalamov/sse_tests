#pragma once

#include <x86intrin.h>

#include <algorithm>

#include "details/common.h"

namespace sse
{

} // namespace sse

namespace test
{

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
    const auto n = a.size() / factor;
    for (std::uint32_t i = 0; i < n; ++i)
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

} // namespace test
