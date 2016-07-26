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
void simple_min_element(const vec_type& a, value_type& r)
{
    r = std::numeric_limits<value_type>::max();
    for (std::uint32_t i = 0; i < a.size(); ++i)
    {
        //r = std::min(r, a[i]);
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
    const auto n = a.size() / factor;  
    for (std::uint32_t i = 0; i < n; ++i)
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

} // namespace test
