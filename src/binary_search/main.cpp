//#define _ITERATOR_DEBUG_LEVEL 0

#include <vector>
#include <numeric>
#include <algorithm>
#include <random>
#include <chrono>
#include <cassert>
#include <iostream>
#include <queue>

//
template<typename BinarySerachFun>
void doTest(const BinarySerachFun& fun, int32_t dataSize, std::vector<int32_t>& targets)
{
    //
    int32_t total = 0;
    const auto start = std::chrono::high_resolution_clock::now();
    for (auto& t : targets)
    {
        total += fun(t);
    }
    const auto end = std::chrono::high_resolution_clock::now();
    const auto diff = end - start;
    const auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(diff).count();
    printf("%dms\n", ms, total);
}

// 幅優先で二分探索
class  BS_BreadthFirst
{
public:
    // data.size()は2の累乗-1しか受け付けない
    void init(const std::vector<int32_t>& data)
    {
        //
        struct Context
        {
            int32_t low;
            int32_t high;
        };
        std::queue<Context> q;
        q.push({ 0, int32_t(data.size()) - 1 });
        //
        while (!q.empty())
        {
            //
            auto ctx = q.front();
            const int32_t mid = (ctx.low + ctx.high) / 2;
            data_.push_back(data[mid]);
            q.pop();
            //
            if (ctx.low != ctx.high)
            {
                const int32_t low0 = ctx.low;
                const int32_t high0 = mid - 1;
                const int32_t low1 = mid + 1;
                const int32_t high1 = ctx.high;
                if (low0 <= high0)
                {
                    q.push({ low0, high0 });
                }
                if (low1 <= high1)
                {
                    q.push({ low1, high1 });
                }
            }
        }
    }
    //
    bool serach(int32_t target) const
    {
        uint32_t idx = 1;
        for(;;)
        {
            if (data_.size() < idx)
            {
                return false;
            }
            const int32_t idxi = idx - 1;
            const int32_t d = data_[idxi];
            if (d == target)
            {
                return true;
            }
            else if (d < target)
            {
                idx <<= 1;
                idx += 1;
            }
            else if (target < d)
            {
                idx <<= 1;
                idx += 0;
                
            }
        }
    }
private:
    std::vector<int32_t> data_;
};

// 深さ優先
class BS_DepthFirst
{
private:
    int32_t msb_count(uint32_t a) const
    {
        uint32_t bit = 0x80000000;
        for (long i = 31; i >= 0; i--) {
            if (a & bit)
            {
                return i;
            }
            bit >>= 1;
        }
        return 0;
    }
public:
    void init(const std::vector<int32_t>& data)
    {
        construct(data, 0, int32_t(data.size())-1);
        depth_ = msb_count(data.size()+1);
    }
    //
    void construct(const std::vector<int32_t>& data, int32_t low, int32_t high)
    {
        if (high < low)
        {
            return;
        }
        const int32_t mid = (low + high) / 2;
        data_.push_back(data[mid]);
        construct(data, low, mid - 1);
        construct(data, mid+1, high);
    }
    bool serach(int32_t target) const
    {
        return serachSub(target, 0, depth_ - 1);
    }
    bool serachSub(int32_t target, uint64_t index, int32_t remainDepth) const
    {
        //
        if (data_.size() <= index)
        {
            return false;
        }
        //
        const int32_t d = data_[index];
        if (target == d)
        {
            return true;
        }
        if (remainDepth == 0)
        {
            return false;
        }
        // 左は一つとなり
        if (target < d)
        {
            index += 1;
            serachSub(target, index, remainDepth - 1);
        }
        // 右は
        else if(d < target)
        {
            index += (1 << remainDepth);
            serachSub(target, index, remainDepth - 1);
        }
    }
private:
    std::vector<int32_t> data_;
    int32_t depth_ = 0;
};

//
void test()
{
    // データの作成
    const int32_t dataSize = (1<<20)-1;
    std::vector<int32_t> data(dataSize);
    std::iota(data.begin(), data.end(), 0);

    //
    BS_DepthFirst bsdf2;
    bsdf2.init(data);
    bsdf2.serach(2);
    for (int32_t i = 0; i < dataSize; ++i)
    {
        assert(bsdf2.serach(i));
    }
    assert(!bsdf2.serach(-1));
    assert(!bsdf2.serach(dataSize+1));

    // 特殊な検索専用構造体を作るもの
    BS_BreadthFirst bsbfn;
    bsbfn.init(data);
    for (int32_t i=0;i<7;++i)
    {
        const bool find = bsbfn.serach(i);
        assert(find);
    }
    assert(!bsbfn.serach(-1));
    assert(!bsbfn.serach(8));

}


//
void main()
{
#if 0
    test();
#else
    //
    printf("gen serach data\n");
    const int32_t testNum = 1024 * 1024 * 4;
    const int32_t dataSize = (1 << 28) - 1;
    std::vector<int32_t> data(dataSize);
    std::iota(data.begin(), data.end(), 0);

    // 検索対象のデータを作成する
    printf("gen targets\n");
    std::vector<int32_t> targets(testNum);
    std::mt19937 rng(0x123);
    std::uniform_int_distribution<int32_t> dist(0, dataSize - 1);
    for (auto& t : targets)
    {
        t = dist(rng);
    }
    printf("done\n");

#if 1
    // 二分探索
    doTest([&](int32_t target)
    {
        return std::binary_search(data.begin(), data.end(), target);
    }, data.size(), targets);

    // 幅優先
    BS_BreadthFirst bsbfn;
    bsbfn.init(data);
    doTest([&](int32_t target)
    {
        return bsbfn.serach(target);
    }, data.size(), targets);

    // 幅優先
    /*BS_BreadthFirst bsbfn2;
    bsbfn2.init(data);
    doTest([&](int32_t target)
    {
        return bsbfn2.serach(target);
    }, data.size(), targets);*/

    // 深さ優先
    BS_DepthFirst bsdf;
    bsdf.init(data);
    doTest([&](int32_t target)
    {
        return bsdf.serach(target);
    }, data.size(), targets);

    return;
#endif    

#endif
}