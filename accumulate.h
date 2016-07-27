#pragma once

#include <x86intrin.h>

//#include <algorithm>
#include <numeric>

#include "details/common.h"

namespace sse
{

template<typename T>
struct plus;

template<>
struct plus<std::uint32_t>
{
    using std_analog = std::plus<std::uint32_t>;
    
    __m128i operator()(const __m128i& lhs, const __m128i& rhs) const
    {
        return _mm_add_epi32(lhs, rhs);
    }
};

template<typename T, typename BinOp>
T accumulate(const T* data, std::size_t size, BinOp op)
{
    __m128i acc{0,0};
    const __m128i* m128_data = reinterpret_cast<const __m128i*>(data);
    auto n = size / 4;
    while (n > 0)
    {
        acc = op(acc, m128_data[--n]);
    }
    const T* t_data = reinterpret_cast<const T*>(&acc);
    return std::accumulate(t_data, t_data + 4, T{0}, typename BinOp::std_analog());
}

}

namespace test
{

template<typename T, typename BinOp>
__attribute__((__noinline__))
T simple_accumulate(const T* data, std::size_t size, BinOp op)
{
    T acc{0};
    for (std::uint32_t i = 0; i < size; ++i)
    {
        acc = op(acc, data[i]);
    }
    return acc;    
}

template<typename T, typename BinOp>
__attribute__((__noinline__))
T std_accumulate(const T* data, std::size_t size, BinOp op)
{
    return std::accumulate(data, data + size, T{0}, op);
}

template<typename Fn, typename T, typename BinOp>
void wrapper(const vec_type& a, T& r, Fn&& fn, BinOp op)
{
    r = fn(a.data(), a.size(), op);
}

void test_accumulate(vec_type& a, std::uint32_t m)
{
    std::uint32_t r1 = 0;
    std::uint32_t r2 = 0;
    std::uint32_t r3 = 0;
    
    using v = value_type;
    using p = std::plus<v>;
    
    auto&& f1 = simple_accumulate<v, p>;
    auto&& w1 = wrapper<decltype(f1), v, p>;
    
    auto&& f2 = std_accumulate<v, p>;
    auto&& w2 = wrapper<decltype(f2), v, p>;
    
    auto&& f3 = sse::accumulate<v, sse::plus<v>>;
    auto&& w3 = wrapper<decltype(f3), v, sse::plus<v>>;
    
    while (m > 0)
    {
        print_time("simple_accumulate", w1, a, r1, f1, p());
        print_time("std_accumulate", w2, a, r2, f2, p());
        print_time("sse_accumulate", w3, a, r3, f3, sse::plus<v>());
        --m;
        a[0]++;
        
        verify(r1 == r2);
        verify(r1 == r3);
    }
}

void test_accumulate_n(vec_type& a, std::uint32_t m)
{
    auto wrapper_n = [&](std::uint32_t m, auto&& fn, auto op)
    {
        value_type r;
        while (m > 0)
        {
            r = fn(a.data(), a.size(), op);
            --m;
            a[0]++;
        }
        volatile auto v = r;
        static_cast<void>(v);
    };
    
    using v = value_type;
    using p = std::plus<v>;
    
    auto f1 = simple_accumulate<v, p>;
    auto f2 = std_accumulate<v, p>;    
    auto f3 = sse::accumulate<v, sse::plus<v>>;
    
    print_time("simple_accumulate", wrapper_n, m, f1, p());
    print_time("std_accumulate", wrapper_n, m, f2, p());
    print_time("sse_accumulate", wrapper_n, m, f3, sse::plus<v>());
}

} // namespace test
