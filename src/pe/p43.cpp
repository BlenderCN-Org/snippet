#include <cstdio>
#include <cstdint>
#include <algorithm>
#include <array>
#include <set>
#include <numeric>

//
void main()
{
    size_t total = 0;
    //
    std::array<size_t, 10> digits = { 0,1,2,3,4,5,6,7,8,9 };
    //
    do
    {
        if (digits[0] != 0)
        {
            const auto makeNum = [&](int32_t b, int32_t l)
            {
                size_t t = 0;
                for (size_t i = b; i <= l; ++i)
                {
                    t *= 10;
                    t += digits[i];
                }
                return t;
            };
            //
            if (!(makeNum(1, 3) % 2) &&
                !(makeNum(2, 4) % 3) &&
                !(makeNum(3, 5) % 5) &&
                !(makeNum(4, 6) % 7) &&
                !(makeNum(5, 7) % 11) &&
                !(makeNum(6, 8) % 13) &&
                !(makeNum(7, 9) % 17))
            {
                size_t nv = makeNum(0, 9);
                total += nv;
                printf("%lld,%lld\n", nv, total);
            }
        }
    } while (std::next_permutation(digits.begin(), digits.end()));
    //
    printf("%lld\n", total);
    return;
}
