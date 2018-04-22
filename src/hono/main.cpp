#include <cstdint>
#include <cstdio>
#include <iostream>
#include <vector>
#include <array>
#include <functional>

// brute force
template<int32_t SIZE>
void dfs(int32_t i, int32_t f, int32_t l,
    std::array<int32_t, SIZE>& buf,
    const std::function<void(const std::array<int32_t, SIZE>& buf)>& fun)
{
    if (i == SIZE) { fun(buf); }
    else
    {
        for (int32_t j = f; j <= l; ++j)
        {
            buf[i] = j;
            dfs<SIZE>(i + 1, f, l, buf, fun);
        }
    }
}
//
int main()
{
    std::array<int32_t, 9> buf;
    int32_t cnt = 0;
    dfs<9>(0, 1, 9, buf, [&](const std::array<int32_t, 9>& a)
    {
        const auto checkLine = [&](int32_t i0, int32_t i1, int32_t i2)
        {
            return
                (a[i0] == a[i1]) ||
                (a[i1] == a[i2]) ||
                (a[i0] == a[i2]);
        };
        if ( // horizontal
            checkLine(0, 5, 8) ||
            checkLine(1, 6, 4) ||
            checkLine(2, 3, 7) ||
            // right downward
            checkLine(0, 6, 3) ||
            checkLine(5, 4, 7) ||
            checkLine(8, 1, 2) ||
            // left downward
            checkLine(5, 6, 2) ||
            checkLine(8, 4, 3) ||
            checkLine(0, 1, 7) )
        {
            return;
        }
        const auto checkHex = [&](int32_t i0, int32_t i1, int32_t i2, int32_t i3, int32_t i4, int32_t i5)
        {
            return (a[i0]* a[i2] * a[i4]) != (a[i1] * a[i3] * a[i5]);
        };
        //
        if (checkHex(0, 1, 2, 3, 4, 5)) { return; }
        if (checkHex(5, 6, 3, 7, 1, 8)) { return; }
        if (checkHex(8, 4, 7, 2, 6, 0)) { return; }
        //
        if (checkHex(6, 2, 8, 0, 7, 4)) { return; }
        if (checkHex(4, 3, 0, 5, 2, 1)) { return; }
        if (checkHex(1, 7, 5, 8, 3, 6)) { return; }
        //
        if (checkHex(2, 5, 4, 1, 0, 3)) { return; }
        if (checkHex(3, 8, 1, 6, 5, 7)) { return; }
        if (checkHex(7, 0, 6, 4, 8, 2)) { return; }
        //
        /*for(auto& c : a) { std::cout << c << " "; }
        std::cout << std::endl;*/
        ++cnt;
        //exit(0);
    });
    // 162ŒÂ‚ÅA‰ñ“]‚ÆˆÚ“®‚Å/6 /9‚Æ‚È‚Á‚Ä3’Ê‚è‚É‚È‚éB
    printf("%d\n", cnt);
    return 0;
}
