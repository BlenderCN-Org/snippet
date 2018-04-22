//
#include <cstdint>
#include <cstdio>
//
#include <vector>
#include <array>

// http://postd.cc/dynamic-programming-for-the-confused-rod-cutting-problem/
static const int32_t price[11] =
{
    -1, // dummy
    1 , //  1
    5,  // 2
    8, // 3
    9, // 4
    10, // 5
    17, // 6
    17, // 7
    20, // 8
    24, // 9
    30, //10
};
class Main0
{
public:
    static int32_t calcMaxPriceSub(int32_t lodLen, std::vector<int32_t>& memos)
    {
        if(lodLen == 1)
        {
            return price[1];
        }
        if(memos[lodLen] != -1)
        {
            return memos[lodLen];
        }
        //
        int32_t maxPrice = 0;
        for(int32_t i=1;i<lodLen+1;++i)
        {
            auto np = price[i] + calcMaxPriceSub(lodLen-i, memos);
            maxPrice = std::max( maxPrice, np  );
        }
        memos[lodLen] = maxPrice;
        return maxPrice;
    }
    
    // 総当たり
    static int32_t calcMaxPrice(int32_t lodLen)
    {
        std::vector<int32_t> memos(lodLen+1);
        std::fill(memos.begin(),memos.end(), -1);
        return calcMaxPriceSub(lodLen, memos);
    }
    
    // 動的計画法
    static int32_t calcMaxPriceDP(int32_t lodLen)
    {
        std::vector<int32_t> memos(lodLen+1, 0);
        for(int32_t l=1;l<=lodLen;++l)
        {
            for(int32_t i=0;i<l;++i)
            {
                memos[l] = std::max( memos[l], memos[i] + price[l-i] );
            }
        }
        return memos[lodLen];
    }
};

// http://dai1741.github.io/maximum-algo-2012/docs/dynamic-programming/
struct Item
{
    int32_t cost;
    int32_t weight;
};
const std::array<Item,6> items =
{
    15, 11,
    3, 2,
    1, 3,
    4, 4,
    2, 1,
    8, 5,
};
typedef std::array<std::array<int32_t,6>,16> Memo;

// idx以降のItemでMaxWeight以下の最大の価値を返す
int32_t napsac(int idx, int32_t maxWeight, Memo& memo)
{
    //
    if(memo[maxWeight][idx] != -1)
    {
        return memo[maxWeight][idx];
    }
    //
    auto& item = items[idx];
    int32_t ret = 0;
    // 末尾の場合は物がないので0
    if(idx == items.size())
    {
        ret = 0;
    }
    // 現在のアイテム単体で最大重量を超えているのでスキップ
    else if( maxWeight < item.weight  )
    {
        ret = napsac(idx+1, maxWeight, memo);
    }
    //
    else
    {
        // このアイテムを使った場合と使わなかった場合の最大の方を返す
        const int32_t v0 = napsac(idx+1, maxWeight - item.weight, memo) + item.cost;
        const int32_t v1 = napsac(idx+1, maxWeight, memo);
        ret = std::max(v0,v1);
    }
    //
    memo[maxWeight][idx] = ret;
    return ret;
}

//
int32_t main()
{
    printf("%d\n", Main0::calcMaxPrice(5) );
    printf("%d\n", Main0::calcMaxPriceDP(5) );
    
    Memo memo;
    std::array<int32_t,6> d;
    d.fill(-1);
    memo.fill(d);
    printf("%d\n", napsac(0,15, memo));
    
}
