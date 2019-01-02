#include <cstdio>
#include <vector>
#include <unordered_map>
#include <set>
#include <cstdint>
#include <array>
#include <functional>

//
void main()
{
    typedef std::array<std::set<int32_t>, 100> NumSets;
    //
    const auto genSets = [](const std::function<int32_t(int32_t)>& gen)
    {
        NumSets ns;
        for (int32_t n = 0;; ++n)
        {
            const int32_t tmp = gen(n);
            const int32_t l2 = tmp / 100;
            const int32_t r2 = tmp % 100;
            if (l2 < 9)
            {
                continue;
            }
            if (l2 > 99)
            {
                break;
            }
            ns[l2].insert(r2);
        }
        return ns;
    };
    const std::array<NumSets, 6> setsList =
    {
        genSets([](int32_t n){ return n*(n + 1) / 2;}),
        genSets([](int32_t n){ return n*n; }), 
        genSets([](int32_t n){ return n*(3 * n - 1) / 2; }),
        genSets([](int32_t n){ return n*(2 * n - 1);}),
        genSets([](int32_t n){ return n*(5 * n - 3) / 2; }),
        genSets([](int32_t n){ return n*(3 * n - 2); })
    };
    std::array<int32_t, 6> arr = { 0, 1, 2, 3, 4, 5 };
    do
    {
        const auto& a0 = setsList[arr[0]];
        const auto& a1 = setsList[arr[1]];
        const auto& a2 = setsList[arr[2]];
        const auto& a3 = setsList[arr[3]];
        const auto& a4 = setsList[arr[4]];
        const auto& a5 = setsList[arr[5]];
        for (int32_t i0 = 10; i0 <= 99; ++i0)
        {
            for (auto i1 : a0[i0])
            {
                for (auto i2 : a1[i1])
                {
                    for (auto i3 : a2[i2])
                    {
                        for (auto i4 : a3[i3])
                        {
                            for (auto i5 : a4[i4])
                            {
                                for (auto i6 : a5[i5])
                                {
                                    if (i6 == i0)
                                    {
                                        printf("%d,%d,%d,%d,%d,%d,%d\n", i0, i1, i2, i3, i4, i5, i6);
                                        const int32_t total =
                                            (i0 * 100 + i1) +
                                            (i1 * 100 + i2) +
                                            (i2 * 100 + i3) +
                                            (i3 * 100 + i4) +
                                            (i4 * 100 + i5) +
                                            (i5 * 100 + i6);
                                        printf("%d\n", total);
                                        return;
}
                                }
                            }
                        }
}
                }
            }
        }
    } while (std::next_permutation(arr.begin(), arr.end()));
    //
    return;
}
