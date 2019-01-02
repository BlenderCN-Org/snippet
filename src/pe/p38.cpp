#include <cstdio>
#include <cstdint>
#include <algorithm>

//
void main()
{
    //
    const auto digit = [](size_t v)
    {
        size_t d = 0;
        while (v != 0)
        {
            v /= 10;
            ++d;
        }
        return d;
    };
    //
    const auto pow10 = [](size_t d)
    {
        size_t t = 1;
        while (d != 0)
        {
            t *= 10;
            --d;
        }
        return t;
    };
    //
    const auto isPanDigit = [](size_t v)
    {
        //
        uint16_t bits = 0;
        for (int32_t i = 0; i < 9;++i)
        {
            if (v == 0)
            {
                return false;
            }
            const size_t shift = v % 10;
            if (shift == 0)
            {
                return false;
            }
            bits |= (1 << shift);
            v /= 10;
        }
        if (v != 0)
        {
            return false;
        }
        return (bits == 0b1111111110);
    };
    //
    const auto gen = [&](size_t b, size_t c)
    {
        size_t td = 0;
        size_t ret = 0;
        for (size_t i = 1; i<c; ++i)
        {
            const size_t t = b * i;
            const size_t d = digit(t);
            const size_t s = pow10(d);
            ret = t + ret * s;
            td += d;
        }
        if (td < 9)
        {
            return size_t(-1);
        }
        else if (td > 9)
        {
            return size_t(-2);
        }
        return ret;
    };
    //
    size_t maxPanDigit = 0;
    for (int32_t i=1;i<10000;++i)
    {
        for (int32_t c=3;;++c)
        {
            //
            const size_t s = gen(i, c);
            if (s == -1)
            {
                continue;
            }
            if (s == -2)
            {
                break;
            }
            //
            if (isPanDigit(s))
            {
                if (maxPanDigit < s)
                {
                    maxPanDigit = s;
                    printf("%d,(%d) = %d\n", i, c, s);
                }
            }
        }
    }
    printf("%lld\n", maxPanDigit);
    return;
}
