#include <cstdio>
#include <cstdint>
#include <algorithm>
#include <array>
#include <vector>
#include <set>
#include <unordered_set>
#include <numeric>

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
    const auto isPrime = [](int32_t v)
    {
        for (int32_t i = 2,n=sqrti(v) + 1; i < n; ++i)
        {
            if (v % i == 0)
            {
                return false;
            }
        }
        return true;
    };
    const auto powi = [](int32_t b, int32_t v)
    {
        int64_t ret = 1;
        while (v != 0)
        {
            ret *= b;
            --v;
        }
        return ret;
    };

    //
    for (int64_t i=1;i<10000000;i+=2)
    {
        if (isPrime(i))
        {
            continue;
        }
        // 
        const auto isOkay = [&](int64_t i)
        {
            for (int64_t j = 0; j < 63; ++j)
            {
                const int64_t p0 = powi(j,2) * 2;
                const int64_t diff = i - p0;
                if (diff < 0)
                {
                    return false;
                }
                if (isPrime(diff))
                {
                    //printf("%d = %d + 2x%d^2\n", i, diff, j);
                    return true;
                }
            }
            return false;
        };
        if (!isOkay(i))
        {
            printf("NG: %d\n", i);
            return;
        }
    }

    return;
}
