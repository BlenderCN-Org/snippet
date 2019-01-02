#include <cstdio>
#include <cstdint>
#include <algorithm>
#include <array>
#include <vector>
#include <set>
#include <unordered_set>
#include <numeric>

//
void main()
{
    //
    std::unordered_set<int64_t> pensSet;
    const auto pen = [](int64_t v)
    {
        return v*(3 * v - 1) / 2;
    };
    for (int64_t i = 1; i < 100000; ++i)
    {
        const int64_t v = pen(i);
        pensSet.insert(v);
    }
    int64_t minD = std::numeric_limits<int64_t>::max();
    for (int64_t j = 2; j < 10000; ++j)
    {
        for (int64_t i = 1; i < j; ++i)
        {
            const int64_t pi = pen(i);
            const int64_t pj = pen(j);
            const int64_t sum = pj + pi;
            const int64_t dif = pj - pi;
            const bool ok0 = (pensSet.find(sum) != pensSet.end());
            const bool ok1 = (pensSet.find(dif) != pensSet.end());
            if (ok0 && ok1 )
            {
                const int64_t d = std::abs(pi - pj);
                minD = std::min(minD, d);
                printf("(%d,%d) D:%d\n", pi, pj, d);
            }
        }
    }
    printf("%d\n", minD);
    return;
}
