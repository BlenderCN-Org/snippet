/*
óÕãZÇ©Ç¬èoóÕÇ©ÇÁñ⁄éãÇ≈íTÇ≥Ç»Ç¢Ç∆Ç¢ÇØÇ»Ç¢...
*/
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
    for (int32_t i = 2, n = sqrti(v)+1; i <n +1; ++i)
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
#if 0
    for (int32_t i=0;i<10;++i)
    {
        for (int32_t j = i+1; j < 10; ++j)
        {
            for (int32_t k = j + 1; k < 10; ++k)
            {
                std::array<int32_t, 5> digits = { i,j,k, 100, 200 };
                do
                {
                    if (digits[0] != 0)
                    {
                        int32_t primeCount = 0;
                        int32_t firstNum = 0;
                        for (int32_t l = 0; l <= 9; ++l)
                        {
                            int32_t v = 0;
                            for (int32_t m = 0; m < 5; ++m)
                            {
                                v *= 10;
                                //
                                switch (digits[m])
                                {
                                case 100:
                                case 200:
                                    v += l;
                                    break;
                                default:
                                    v += digits[m];
                                    break;
                                }
                            }
                            if (isPrime(v))
                            {
                                ++primeCount;
                                //
                                if (firstNum != -1)
                                {
                                    firstNum = v;
                                }
                            }
                        }
                        //
                        if (primeCount >= 7)
                        {
                            printf("%d(%d)\n", firstNum, primeCount);
                        }
                    }
                } while (std::next_permutation(digits.begin(), digits.end()));
            }
        }
    }
#else
    for (int32_t i0 = 0; i0<10; ++i0)
    {
        for (int32_t i1 = i0; i1 < 10; ++i1)
        {
            for (int32_t i2 = i1; i2 < 10; ++i2)
            {
                std::array<int32_t, 6> digits = { i0, i1, i2, 100, 200, 300 };
                do
                {
                    if (digits[0] != 0)
                    {
                        int32_t primeCount = 0;
                        int32_t firstNum = -1;
                        for (int32_t l = 0; l <= 9; ++l)
                        {
                            int32_t v = 0;
                            for (int32_t m = 0; m < 6; ++m)
                            {
                                v *= 10;
                                //
                                switch (digits[m])
                                {
                                case 100:
                                case 200:
                                case 300:
                                    v += l;
                                    break;
                                default:
                                    v += digits[m];
                                    break;
                                }
                            }
                            if (isPrime(v))
                            {
                                ++primeCount;
                                //
                                if (firstNum == -1)
                                {
                                    firstNum = v;
                                }
                            }
                        }
                        //
                        if (primeCount >= 8)
                        {
                            printf("%d(%d)\n", firstNum, primeCount);
                        }
                    }
                } while (std::next_permutation(digits.begin(), digits.end()));
            }

        }
    }
#endif
    return;
}
