#include <cstdio>
#include <cstdint>
#include <algorithm>
#include <array>
#include <set>
#include <numeric>
//
void main()
{
    std::set<int32_t> okays;
    std::array<int32_t, 9> digits = { 1,2,3,4,5,6,7,8,9 };
    do
    {
        for (int32_t sep0 = 1; sep0 < 8; ++sep0)
        {
            for (int32_t sep1 = sep0 + 1; sep1 < 9; ++sep1)
            {
                const auto makeNum = [&](int32_t b, int32_t e)
                {
                    int32_t v = 0;
                    for (int32_t i = b; i < e; ++i)
                    {
                        v *= 10;
                        v += digits[i];
                    }
                    return v;
                };
                //
                const int32_t v0 = makeNum(0, sep0);
                const int32_t v1 = makeNum(sep0, sep1);
                const int32_t v2 = makeNum(sep1, 9);
                if (v0*v1 == v2)
                {
                    printf("%dx%d=%d\n", v0, v1, v2);
                    okays.insert(v2);
                }
            }
        }
    } while (std::next_permutation(digits.begin(), digits.end()));
    //
    printf("%lld\n", std::accumulate(okays.begin(), okays.end(), 0));
    return;
}
