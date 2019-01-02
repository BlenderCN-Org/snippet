#include <cstdio>
#include <cstdint>
#include <vector>
#include <algorithm>

//
void main()
{
    //
    const auto cube = [](size_t i)
    {
        return size_t(i*i*i);
    };
    const std::vector<std::vector<size_t>> sorted = [&]()
    {
        std::vector<std::vector<size_t>> ret;
        std::vector<size_t> tmpv;
        for (size_t i = 0; i < 10000; ++i)
        {
            tmpv.clear();
            size_t tmp = cube(i);
            while (tmp != 0)
            {
                tmpv.push_back(tmp % 10);
                tmp /= 10;
            }
            std::sort(tmpv.begin(), tmpv.end());
            ret.push_back(tmpv);
        }
        return ret;
    }();
    //
    for (size_t i0 = 1; i0 < sorted.size(); ++i0)
    {
        for (size_t i1 = i0 + 1; (i1 < sorted.size()) && (sorted[i0].size() == sorted[i1].size()); ++i1)
        {
            if (sorted[i0] != sorted[i1]) { continue;  }
            for (size_t i2 = i1 + 1; (i2 < sorted.size()) && (sorted[i1].size() == sorted[i2].size()); ++i2)
            {
                if (sorted[i0] != sorted[i2]) { continue; }
                for (size_t i3 = i2 + 1; (i3 < sorted.size()) && (sorted[i2].size() == sorted[i3].size()); ++i3)
                {
                    if (sorted[i0] != sorted[i3]) { continue; }
                    for (size_t i4 = i3 + 1; (i4 < sorted.size()) && (sorted[i3].size() == sorted[i4].size()); ++i4)
                    {
                        if (sorted[i0] != sorted[i4]) { continue; }
                        printf("%lld(%d),%lld(%d),%lld(%d),%lld(%d),%lld(%d)\n", cube(i0), i0, cube(i1), i1, cube(i2), i2, cube(i3), i3, cube(i4), i4);
                        return;
                    }
                }
            }
        }
    }
    //
    return;
}
