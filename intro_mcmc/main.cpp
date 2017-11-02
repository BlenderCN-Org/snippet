/*
やさしいMCMC入門 例 7.2 p62
*/

#include <cstdint>
#include <cstdio>
#include <random>
#include <array>

// 格子表現
template<int32_t SIZE>
class Grid
{
public:
    class Index
    {
    public:
        Index(int32_t index) :index_(index) {}
        Index(int32_t x, int32_t y) :index_(x + y * SIZE) {}
        int32_t index() const { return index_; }
        int32_t x() const { return index_ % SIZE; }
        int32_t y() const { return index_ / SIZE; }
    private:
        int32_t index_ = 0;
    };
public:
    //
    Grid()
    {
        clear();
    }
    //
    int32_t size() const
    {
        return SIZE;
    }
    // 取得
    bool get(Index index) const
    {
        return bits[index.index()];
    }
    // 設定
    void set(Index index, bool v)
    {
        bits[index.index()] = v;
    }
    // クリア
    void clear()
    {
        std::fill(bits.begin(), bits.end(), false);
    }
    //// 特定の位置を反転する
    //void not(int32_t idx)
    //{
    //    bits[idx] = !bits[idx];
    //}
    // 指定した場所の隣接に粒子が存在するか
    bool isExistAdjacent(Index index) const
    {
        const int32_t x = index.x();
        const int32_t y = index.y();
        return
            ((x > 0) ? get({ x - 1, y }) : false) ||
            ((y > 0) ? get({ x, y - 1 }) : false) ||
            ((x < SIZE - 1) ? get({ x + 1, y }) : false) ||
            ((y < SIZE - 1) ? get({ x, y + 1 }) : false);
    }
    // 全ての粒子が、隣に粒子がいない状態を維持しているか
    bool valid() const
    {
        //
        for (int32_t y = 0; y < SIZE; ++y)
        {
            for (int32_t x = 0; x < SIZE; ++x)
            {
                if (!get({ x, y }))
                {
                    continue;
                }
                // 粒子の隣接に別の粒子が存在しているためNG
                if (isExistAdjacent({ x, y }))
                {
                    return false;
                }
            }
        }
        return true;
    }
    //
    int32_t numParticle() const
    {
        int32_t n = 0;
        for (bool v : bits)
        {
            n += v ? 1 : 0;
        }
        return n;
    }
    //
    void print()
    {
        for (int32_t y = 0; y < SIZE; ++y)
        {
            for (int32_t x = 0; x < SIZE; ++x)
            {
                printf("%s", get(x, y) ? "●" :  "○");
            }
            printf("\n");
        }
        printf("\n");
    }
private:
    std::array<bool, SIZE*SIZE> bits;
};

//
uint32_t makeBitmask(uint32_t n)
{
    uint32_t bitmask = n;
    bitmask |= bitmask >> 1;
    bitmask |= bitmask >> 2;
    bitmask |= bitmask >> 4;
    bitmask |= bitmask >> 8;
    bitmask |= bitmask >> 16;
    return bitmask;
}

//
uint32_t rng2size(std::mt19937& rng, int32_t size)
{
    if (size == 0)
    {
        return 0;
    }
    const uint32_t mask = makeBitmask(size);
    //
    uint32_t result;
    while ((result = (rng() & mask)) >= static_cast<uint32_t>(size)) {}
    return result;
}

//
int32_t main()
{
    //
    Grid<4> grid;

    // 愚直に全てのパターンを数え上げ
    {
        float total = 0.0f;
        int32_t numCount = 0;
        for (uint16_t pattern = 0; pattern < std::numeric_limits<uint16_t>::max(); ++pattern)
        {
            grid.clear();
            for (int32_t i = 0; i < 16; ++i)
            {
                grid.set(i, ((pattern >> i) & 0x01));
            }
            // 
            if (!grid.valid())
            {
                continue;
            }
            //
            total += grid.numParticle();
            ++numCount;
            //
            //grid.print();
        }
        printf("ValidPattern: %d/%d\n", numCount, std::numeric_limits<uint16_t>::max());
        printf("E = %f\n", float(total) / float(numCount));
    }

    // MCMCを使って数え上げ
    {
        grid.clear();
        std::mt19937 rng(std::random_device{}());
        float total = 0.0f;
        int32_t numCount = 0;
        const int32_t totalIte = 1000;
        for (int32_t i = 0; i < totalIte; ++i)
        {
            // 特定のビット位置を書き換え
            const int32_t idx = rng2size(rng, 16);
            const bool isExistAdjacent = grid.isExistAdjacent(idx);
            // NGでなければ書き込む
            if (isExistAdjacent)
            {
                grid.set(idx, true);
            }
            else
            {
                grid.set(idx, false);
            }
            
            total += grid.numParticle();
            ++numCount;

            /*if ((i + 1) % 100 == 0)
            {
                printf("E = %f (Hit:%d/%d)\n", float(total) / float(numCount), numCount, i);
            }*/
        }
        printf("E = %f (Hit:%d/%d)\n", float(total) / float(numCount), numCount, totalIte);
    }
    return 0;
}
