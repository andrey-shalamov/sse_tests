#pragma once

#include <x86intrin.h>

#include <algorithm>
#include <array>

#include "details/common.h"

namespace sse
{

std::uint32_t min_element(const std::uint32_t* data, std::size_t size)
{
    constexpr std::int32_t MAX_INT32 = std::numeric_limits<std::int32_t>::max();
    
    __m128i min_128 = _mm_set_epi32(MAX_INT32, MAX_INT32, MAX_INT32, MAX_INT32);
    const __m128i* data_128 = reinterpret_cast<const __m128i*>(data);  
    auto n = size / 4;
    while (n > 0)
    {
        min_128 = _mm_min_epu32(min_128, data_128[--n]);
    }
    const std::uint32_t* ui32 = reinterpret_cast<const std::uint32_t*>(&min_128);
    return *std::min_element(ui32, ui32 + 4);
}

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
    
void test_min(const vec_type& a, std::uint32_t n, std::uint32_t m)
{
    std::uint32_t r1 = 0;
    std::uint32_t r2 = 0;
    std::uint32_t r3 = 0;
    
    auto sse_min_element_wrapper = [](const vec_type& v, value_type& r)
    {
        r = sse::min_element(v.data(), v.size());
    };
    while (m > 0)
    {
        print_time("simple_min_element", simple_min_element, a, r1);
        print_time("std_min_element", std_min_element, a, r2);
        print_time("sse_min_element", sse_min_element_wrapper, a, r3);
        --m;
        
        verify(r1 == r2);
        verify(r1 == r3);
    }
}

} // namespace test
