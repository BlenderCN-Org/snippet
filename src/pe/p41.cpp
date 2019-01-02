#include <cstdio>
#include <cstdint>
#include <algorithm>
#include <vector>
#include <set>
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
    int32_t maxPrime = 0;
    for (int32_t nDigits = 1; nDigits<10;++nDigits)
    {
        //
        std::vector<int32_t> digits(nDigits);
        std::iota(digits.begin(), digits.end(), 1);
        //
        do
        {
            //
            const size_t n = [&]()
            {
                int32_t t = 0;
                for (int32_t i = 0; i < nDigits; ++i)
                {
                    t *= 10;
                    t += digits[i];
                }
                return t;
            }();
            //
            const bool isPrime = [](int32_t v)
            {
                for (int32_t i = 2,n=sqrti(v) + 1; i < n; ++i)
                {
                    if (v % i == 0)
                    {
                        return false;
                    }
                }
                return true;
            }(n);
            //
            if (isPrime)
            {
                if (maxPrime < n)
                {
                    printf("%d\n", n);
                    maxPrime = n;
                }
            }
        } while (std::next_permutation(digits.begin(), digits.end()));
    }
    //
    printf("%d\n", maxPrime);
    return;
}
