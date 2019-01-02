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
    const auto diggitSort = [](int64_t v)
    {
        int32_t idx = 0;
        std::array<int32_t, 64> tmp;
        while (v != 0)
        {
            tmp[idx] = v % 10;
            v /= 10;
            ++idx;
        }
        // 0Ç™ç≈å„Ç…Ç»ÇÈÇÊÇ§Ç…ç~èáÇ…Ç∑ÇÈ
        std::sort(tmp.begin(), tmp.begin() + idx, std::greater<int64_t>());
        int64_t ret = 0;
        for (int32_t i = 0; i<idx; ++i)
        {
            ret *= 10;
            ret += tmp[i];
        }
        return ret;
    };
    //
    struct Prime
    {
    public:
        int64_t number;
        int64_t sorted;
    };
    std::vector<Prime> primes;
    std::unordered_set<int64_t> primeSet;
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
    for (int64_t i=1000;i<10000;++i)
    {
        if (isPrime(i))
        {
            Prime p;
            p.number = i;
            p.sorted = diggitSort(i);
            primes.push_back(p);
            primeSet.insert(i);
        }
    }

    for (int32_t i = 0; i < primes.size(); ++i)
    {
        for (int32_t j = i+1; j < primes.size(); ++j)
        {
            const auto p0 = primes[i];
            const auto p1 = primes[j];
            if (p0.sorted != p1.sorted)
            {
                continue;
            }
            const int64_t p2 = 2 * p1.number - p0.number;
            if (primeSet.find(p2) == primeSet.end())
            {
                continue;
            }
            const int64_t ps2 = diggitSort(p2);
            if (p1.sorted != ps2)
            {
                continue;
            }
            printf("%d,%d,%d\n", p0.number, p1.number, p2);
        }
    }
    return;
}
