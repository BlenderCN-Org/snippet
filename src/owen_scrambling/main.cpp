/*
Owen's scramblingを学ぶ

TODOs
- 点列を出力してエクセルで見れるようにする
- radicalinverseを作成する
*/

#include <random>
#include <iostream>
#include <array>
#include <algorithm>
#include <numeric>

//
float radicalInverse(uint32_t base, uint32_t index)
{
    int32_t n = index;
    float val = 0.0f;
    const float invBase = 1.0f / float(base);
    float invBi = invBase;
    while (n > 0)
    {
        int32_t d_i = (n % base);
        val += d_i * invBi;
        n = (int32_t)((float)n * invBase);
        invBi *= invBase;
    }
    return val;
}

float radicalInverseBase2(uint32_t n, uint32_t scramble)
{
    n = (n << 16) | (n >> 16);
    n = ((n & 0x00ff00ff) << 8) | ((n & 0xff00ff00) >> 8);
    n = ((n & 0x0f0f0f0f) << 4) | ((n & 0xf0f0f0f0) >> 4);
    n = ((n & 0x33333333) << 2) | ((n & 0xcccccccc) >> 2);
    n = ((n & 0x55555555) << 1) | ((n & 0xaaaaaaaa) >> 1);
    //
    n = (n >> (32 - 24)) ^ (scramble & ~- (1 << 24));

    return (float)n / (float)(1U << 24);
}

//
void main()
{
    // パーミュテーションテーブルの作成
    std::array<uint32_t,32> perm;
    std::iota(perm.begin(), perm.end(), 0);
    std::shuffle(perm.begin(), perm.end(), std::mt19937(0x123));
    puts("");

    //
    const uint32_t scramble = 0b10101010;
    uint32_t n = 0b0000000111111;
    n = (n >> (32 - 24)) ^ (scramble & ~- (1 << 24));
    printf("%b\n", n);

    // TODO: 
    for (int32_t i=0;i<64;++i)
    {
        printf("%f\n", radicalInverse(2, i));
    }
}