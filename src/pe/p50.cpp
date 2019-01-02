#include <cstdio>
#include <cstdint>
#include <algorithm>
#include <array>
#include <vector>
#include <set>
#include <unordered_set>
#include <numeric>
#include <optional>
#include <bitset>

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

bool isPrime(int32_t v)
{
    for (int32_t i = 2, n = sqrti(v); i <n +1; ++i)
    {
        if (v % i == 0)
        {
            return false;
        }
    }
    return true;
};

//
void main()
{
    //
    std::vector<int64_t> primes;
    for (int64_t i = 2; primes.size() < 600; ++i)
    {
        if (isPrime(i))
        {
            primes.push_back(i);
        }
    }
    // summed area table�̍쐬
    std::vector<int64_t> cdf;
    int64_t total = 0;
    cdf.push_back(0);
    for (int32_t p : primes)
    {
        total += p;
        cdf.push_back(total);
    }
    //
    int32_t maxLen = 0;
    int64_t maxPrim = 0;
    for (int32_t e = 1; e < primes.size(); ++e)
    {
        for (int32_t b = 0; b < e; ++b)
        {
            const int64_t ps = cdf[e] - cdf[b];
            const int32_t len = e - b;
            if (ps < 1000000)
            {
                if (maxLen < len)
                {
                    if (isPrime(ps))
                    {
                        maxPrim = ps;
                        maxLen = len;
                    }
                }
            }
        }
    }
    printf("%lld\n", maxPrim);
    return;
}
