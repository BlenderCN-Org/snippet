/*
 UnionFind
 https://www.slideshare.net/chokudai/union-find-49066733
 */
#include <cstdio>
#include <numeric>
#include <array>

// 親の番号
template<int32_t N>
class UnionFind
{
public:
    std::array<int32_t,N> par;
    //
    UnionFind()
    {
        // 初期化。全てが根
        std::iota(par.begin(),par.end(),0);
    }
    //
    int32_t root(int32_t x)
    {
        // すでに根の場合
        if(par[x] == x)
        {
            return x;
        }
        else
        {
            // 経路圧縮
            return par[x] = root(par[x]);
        }
    }
    
    //
    bool same(int32_t x, int32_t y)
    {
        return root(x) == root(y);
    }
    
    //
    void unite(int32_t x, int32_t y)
    {
        const int32_t rx = root(x);
        const int32_t ry = root(y);
        // すでに根が同じ場合
        if(rx == ry)
        {
            return;
        }
        // 違う場合は根につけていく
        else
        {
            par[x] = y;
        }
    }
};

//
int32_t main()
{
    UnionFind<6> uf;
    uf.unite(0, 2);
    uf.unite(1, 3);
    uf.unite(3, 5);
    
    printf("%d\n",uf.same(0, 5));
    
    //
    
    //
    return 0;
}
