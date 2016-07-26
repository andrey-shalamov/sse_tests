#pragma once

#include <iostream>
#include <chrono>
#include <iomanip>
#include <vector>
#include <cstring>

namespace test
{

using value_type = std::uint32_t;
using vec_type = std::vector<value_type>;

constexpr std::uint8_t factor = 4; 

void verify(bool b)
{
    if (not b) throw "error";
}

void check_vec(const vec_type& a, const vec_type& b)
{
    verify(a.size() == b.size());
    verify(0 == std::memcmp(a.data(), b.data(), a.size() * sizeof(typename vec_type::value_type)));
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

} // namespace test
