/*
やさしいMCMC入門 例 7.2 p62
*/

#include <cstdint>
#include <cstdio>
#include <cassert>
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
                printf("%s", get({ x, y }) ? "●" : "○");
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
template<int32_t SIZE>
class BitArray
{
public:
    //
    BitArray()
    {
        clear();
    }
    //
    void clear()
    {
        std::fill(bits.begin(), bits.end(), 0);
    }
    //
    void next()
    {
        for (auto& bit : bits)
        {
            if (bit == false)
            {
                bit = true;
                return;
            }
            else
            {
                bit = false;
            }
        }
    }
    //
    bool get(int32_t idx) const
    {
        return bits[idx];
    }
    //
    int32_t totalPattern() const
    {
        return (1 << SIZE);
    }
    //
    void print() const
    {
        for (auto& bit : bits)
        {
            printf("%s", bit ? "●" : "○");
        }
        printf("\n");
    }
private:
    std::array<bool, SIZE> bits;
};

//
int32_t main()
{
    /*
    真値
    グリッド 1x1: E = 0.500000(2/2)
    グリッド 2x2: E = 1.142857(7/16)
    グリッド 3x3: E = 2.412699(63/512)
    グリッド 4x4: E = 4.064830(1234/65536)
    グリッド 5x5: E = 6.208433(55447/33554432)
    */
    //
    const int32_t GRID_SIZE = 4;
    const int32_t GRID_SIZE2 = GRID_SIZE * GRID_SIZE;
    Grid<GRID_SIZE> grid;
    BitArray<GRID_SIZE2> bitarray;
#if 1 // NOTE: グリッドサイズが増えてくると計算時間が劇的に増加するので5以上はコメントアウトした方が良い
    // 愚直に全てのパターンを数え上げ
    {
        float total = 0.0f;
        int32_t numCount = 0;
        const int32_t tp = bitarray.totalPattern();
        for (int32_t pattern = 0; pattern < tp; ++pattern)
        {
            grid.clear();
            for(int32_t i=0;i<GRID_SIZE2;++i)
            {
                grid.set(i, bitarray.get(i));
            }
            bitarray.next();
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
        printf("ValidPattern: %d/%d\n", numCount, tp);
        printf("E = %f\n", float(total) / float(numCount));
    }
#endif

    // MCMCを使って数え上げ
    // TODO: 硬化の表裏の話は全く考慮していないがなぜか一致するいいのか？
    // TODO: 初期状態が常に全てクリアしたところからスタートしているがいいのか？
    {
        grid.clear();
        std::mt19937 rng(std::random_device{}());
        float total = 0.0f;
        const int32_t totalIte = 10000;
        for (int32_t i = 0; i < totalIte; ++i)
        {
            // 特定のビット位置を書き換え
            const int32_t idx = rng2size(rng, GRID_SIZE2);
            const bool isExistAdjacent = grid.isExistAdjacent(idx);
            const bool isExist = grid.get(idx);
            // NGでなければ書き込む
            if (isExistAdjacent || isExist)
            {
                grid.set(idx, false);
            }
            else
            {
                grid.set(idx, true);
            }
            //grid.print();
            //
            total += grid.numParticle();
        }
        printf("E = %f\n", float(total) / float(totalIte));
    }

    return 0;
}
