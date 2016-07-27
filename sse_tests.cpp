// g++ --std=c++14 -O3 -DNDEBUG sse.cpp -msse4.2 -o t

#include <iostream>
#include <vector>

#include "add.h"
#include "min_element.h"
#include "accumulate.h"

//------------------------------------------------------------------------------

int main(int c, char** v)
{    
    if (c != 4)
    {
        std::cerr << "you must set 3 arguments." << std::endl;
        return -1;
    }
    std::uint32_t n = std::stoi(v[1]) * test::factor;
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
        test::test_add(a, b, n, m);
        break;
    case 2:
        test::test_min(a, n, m);
        break;
    case 3:
        test::test_min_n(a, n, m);
    case 4:
        test::test_accumulate(a, m);
        break;
    case 5:
        test::test_accumulate_n(a, m);
        break;
    default:
        std::cout << "There is no test with id " << test << std::endl;
    }
    
    return 0;
}
