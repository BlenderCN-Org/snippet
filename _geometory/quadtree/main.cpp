/*
Morton code
*/
#include <cstdio>
#include <cstdint>
#include <cassert>
#include <map>
#include <tuple>

namespace Morton
{
    // 入力は16bitまで
    uint32_t encodeXY(uint32_t x, uint32_t y)
    {
        // 16bit -> 32bit
        const auto part1By1 = [](uint32_t x) -> uint32_t
        {
            x &=                 0b00000000000000001111111111111111;
            x = (x ^ (x << 8)) & 0b00000000111111110000000011111111;
            x = (x ^ (x << 4)) & 0b00001111000011110000111100001111;
            x = (x ^ (x << 2)) & 0b00110011001100110011001100110011;
            x = (x ^ (x << 1)) & 0b01010101010101010101010101010101;
            return x;
        };
        return (part1By1(y) << 1) + part1By1(x);
    }
    //
    uint32_t encodeXY(float x, float y, int32_t depth)
    {
        const float size = float(1 << depth);
        return encodeXY(uint32_t(size*x), uint32_t(size*x));
    }
    // 入力は10bitまで
    uint32_t encodeXYZ(uint32_t x, uint32_t y, uint32_t z)
    {
        // 10bit -> 32bit
        const auto part1By2 = [](uint32_t x) -> uint32_t
        {
            x &=                  0b00000000000000000000001111111111;
            x = (x ^ (x << 16)) & 0b11111111000000000000000011111111;
            x = (x ^ (x << 8)) &  0b00000011000000001111000000001111;
            x = (x ^ (x << 4)) &  0b00000011000011000011000011000011;
            x = (x ^ (x << 2)) &  0b00001001001001001001001001001001;
            return x;
        };
        return (part1By2(z) << 2) + (part1By2(y) << 1) + part1By2(x);
    }
    // morton codeからXYを算出する
    std::pair<uint32_t, uint32_t> decodeXY(uint32_t code)
    {
        // 32bit -> 16bit
        const auto compact1by1 = [](uint32_t n) -> uint32_t
        {
            n = (n & 0b01010101010101010101010101010101);
            n = (((n >> 1) | n) & 0b00110011001100110011001100110011);
            n = (((n >> 2) | n) & 0b00001111000011110000111100001111);
            n = (((n >> 4) | n) & 0b00000000111111110000000011111111);
            n = (((n >> 8) | n) & 0b00000000000000001111111111111111);
            return n;
        };
        return { compact1by1(code), compact1by1(code>>1) };
    }
    std::tuple<uint32_t, uint32_t, uint32_t> decodeXYZ(uint32_t code)
    {
        // 32bit -> 16bit
        const auto compact1by1 = [](uint32_t x) -> uint32_t
        {
            x &=                  0b00001001001001001001001001001001;
            x = (x ^ (x >> 2)) &  0b00000011000011000011000011000011;
            x = (x ^ (x >> 4)) &  0b00000011000000001111000000001111;
            x = (x ^ (x >> 8)) &  0b11111111000000000000000011111111;
            x = (x ^ (x >> 16)) & 0b00000000000000000000001111111111;
            return x;
        };
        return { compact1by1(code>>0), compact1by1(code >> 1), compact1by1(code >> 2) };
    }
    // 親のモートンコードを算出する
    uint32_t parentXY(uint32_t code)
    {
        return code >> 2;
    }
};

//
void main()
{
    //
    const auto checkBackOriginal = [](uint32_t x, uint32_t y)
    {
        auto ret = Morton::decodeXY(Morton::encodeXY(x, y));
        assert(x == std::get<0>(ret));
        assert(y == std::get<1>(ret));
    };
    for (int32_t x = 0; x < std::numeric_limits<uint16_t>::max(); ++x)
    {
        checkBackOriginal(x, 0);
        checkBackOriginal(x, 1);
        checkBackOriginal(x, 15);
        checkBackOriginal(x, (1 << 15) + 125);
    }
    for (int32_t y = 0; y < std::numeric_limits<uint16_t>::max(); ++y)
    {
        checkBackOriginal(0, y);
        checkBackOriginal(1, y);
        checkBackOriginal(15, y);
        checkBackOriginal((1 << 15) + 125, y);
    }
    //
    for (int32_t z = 0; z < (1 << 10); z += 111)
    {
        for (int32_t y = 0; y < (1 << 10); ++y)
        {
            for (int32_t x = 0; x < (1 << 10); ++x)
            {
                auto ret = Morton::decodeXYZ(Morton::encodeXYZ(x, y, z));
                assert(x == std::get<0>(ret));
                assert(y == std::get<1>(ret));
                assert(z == std::get<2>(ret));
            }
        }
    }

    // TODO: 次元と深さが指定された上でのmorton codeの実装を作成する
}

