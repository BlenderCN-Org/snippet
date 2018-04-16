#include <cstdint>
#include <iostream>
#include <array>
#include <functional>
#include <set>
//
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
    typedef std::array<int32_t, 9> Buffer;
    Buffer buf;
    std::set<int32_t> hs;
    dfs<9>(0, 1, 9, buf, [&](const Buffer& a)
           {
               const auto cl = [&](int32_t i0, int32_t i1, int32_t i2)
               {
                   return (a[i0] == a[i1]) || (a[i1] == a[i2]) || (a[i0] == a[i2]);
               };
               if (cl(0, 5, 8) || cl(1, 6, 4) || cl(2, 3, 7) ||
                   cl(0, 6, 3) || cl(5, 4, 7) || cl(8, 1, 2) ||
                   cl(5, 6, 2) || cl(8, 4, 3) || cl(0, 1, 7) )
               {
                   return;
               }
               const auto ch = [&](int32_t i0, int32_t i1, int32_t i2, int32_t i3, int32_t i4, int32_t i5)
               { return (a[i0]* a[i2] * a[i4]) != (a[i1] * a[i3] * a[i5]); };
               //
               if (ch(0, 1, 2, 3, 4, 5) || ch(5, 6, 3, 7, 1, 8) || ch(8, 4, 7, 2, 6, 0) ||
                   ch(6, 2, 8, 0, 7, 4) || ch(4, 3, 0, 5, 2, 1) || ch(1, 7, 5, 8, 3, 6) ||
                   ch(2, 5, 4, 1, 0, 3) || ch(3, 8, 1, 6, 5, 7) || ch(7, 0, 6, 4, 8, 2))
               {
                   return;
               }
               //
               const auto b2h = [](const Buffer& a)
               {
                   int32_t r = 0;
                   for(auto& b : a) { r ^= int32_t((int64_t(b)*2654435761) & 0xFFFFFFFF);; }
                   return r;
               };
               if( hs.find(b2h(a)) != hs.end() )
               { return; }
               hs.insert(b2h(a));
               //
               for(auto& c : a) { std::cout << c << " "; }
               std::cout << std::endl;
           });
    return 0;
}
