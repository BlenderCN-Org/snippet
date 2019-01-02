#include <cstdio>
#include <cstdint>
#include <algorithm>
#include <array>
#include <vector>
#include <set>
#include <unordered_set>
#include <numeric>
#include <optional>

//
int32_t sqrti(int32_t v)
{
    if (v < 0)
    {
        return -1;
    }
    else if (v < 2)
    {
        return v;
    }
    else
    {
        const int32_t i0 = sqrti(v >> 2) << 1;
        const int32_t i1 = i0 + 1;
        if (i0 * i1 > v)
        {
            return i0;
        }
        else
        {
            return i1;
        }
    }
}

//
void main()
{
    //
    std::vector<int64_t> primes;
    const auto isPrime = [](int32_t v)
    {
        for (int32_t i = 2, n=sqrti(v) + 1; i < n; ++i)
        {
            if (v % i == 0)
            {
                return false;
            }
        }
        return true;
    };
    for (int64_t i=2;i<10000000;++i)
    {
        if (isPrime(i))
        {
            primes.push_back(i);
        }
    }
    //
    const auto factoring4 = [&](int64_t v) -> std::optional<std::array<int32_t, 4>>
    {
        const int32_t SIZE = 4;
        std::array<int32_t, SIZE> fs;
        int32_t fidx = 0;
        for (int32_t i = 0; 1 < v; i++)
        {
            const int64_t p = primes[i];
            if (v % p != 0)
            {
                continue;
            }
            while ((v % p) == 0)
            {
                v /= p;
            }
            if (fidx == SIZE)
            {
                return std::nullopt;
            }
            fs[fidx] = p;
            ++fidx;
        }
        if (fidx == SIZE)
        {
            return fs;
        }
        else
        {
            return std::nullopt;
        }
    };
    //
    for (int64_t i=1;i<10000000;++i)
    {
        const auto f4_0 = factoring4(i + 0);
        const auto f4_1 = factoring4(i + 1);
        const auto f4_2 = factoring4(i + 2);
        const auto f4_3 = factoring4(i + 3);
        if (!f4_0 || !f4_1 || !f4_2 || !f4_3)
        {
            continue;
        }
        const auto v0 = f4_0.value();
        const auto v1 = f4_1.value();
        const auto v2 = f4_2.value();
        const auto v3 = f4_3.value();
        if ((v0 != v1) &&
            (v0 != v2) &&
            (v0 != v3) &&
            (v1 != v2) && 
            (v1 != v3) &&
            (v2 != v3))
        {
            printf("%d = %d x %d x %d x %d\n", i + 0, v0[0], v0[1], v0[2], v0[3]);
            printf("%d = %d x %d x %d x %d\n", i + 1, v1[0], v1[1], v1[2], v1[3]);
            printf("%d = %d x %d x %d x %d\n", i + 2, v2[0], v2[1], v2[2], v2[3]);
            printf("%d = %d x %d x %d x %d\n", i + 3, v3[0], v3[1], v3[2], v3[3]);
            return;
        }
    }
    return;
}
