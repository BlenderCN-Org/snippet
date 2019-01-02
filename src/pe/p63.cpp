#include <cstdio>
#include <cstdint>
#include <vector>
#include <algorithm>

//
void main()
{
    //
    const auto digit = [](size_t v)
    {
        int32_t d = 0;
        while (v != 0)
        {
            v /= 10;
            ++d;
        }
        return d;
    };
    const auto power = [](size_t v, size_t p)
    {
        size_t r = 1;
        for (size_t i = 0; i < p; ++i)
        {
            r *= v;
        }
        return r;
    };
    size_t count = 0;
    for (size_t baseDigit = 1; baseDigit < 20;++baseDigit)
    {
        for (size_t i = 1;; ++i)
        {
            const size_t target = power(i, baseDigit);
            const size_t d = digit(target);
            if (d == baseDigit)
            {
                printf("%lld, %lld^%lld\n", target,i, (baseDigit));
                ++count;
            }
            else if (d > baseDigit)
            {
                break;
            }
        }
    }
    // 49‚ç‚µ‚¢‚ª47‚Ü‚Å‚µ‚©ƒJƒEƒ“ƒg‚Å‚«‚Ä‚¢‚È‚¢...
    printf("%d\n", count);
    //
    return;
}
