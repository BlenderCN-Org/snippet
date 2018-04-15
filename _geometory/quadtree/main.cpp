/*
 Morton code
 */
#include <cstdio>
#include <cstdint>
#include <cassert>
#include <map>
#include <tuple>
#include <array>
#include <vector>
#include <cstdlib>
#include "../../thirdparty/vdb/vdb-win/vdb.h"

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
    // 親から見たらどこに属しているか
    uint32_t chNo(uint32_t code)
    {
        return code & 0x03;
    }
};

class MortonCode2D
{
public:
    MortonCode2D() = default;
    // モートンコード直接指定
    MortonCode2D(uint32_t code)
    :code_(code)
    {}
    // (X,Y)の指定
    MortonCode2D(uint32_t x, uint32_t y)
    {
        code_ = Morton::encodeXY(x,y);
    }
    // 座標と深さを指定
    MortonCode2D(float x, float y, int32_t depth)
    {
        const float size = float(1 << depth);
        code_ = Morton::encodeXY(uint32_t(size*x), uint32_t(size*x));
    }
    // モートンコードそのものを返す
    uint32_t code() const
    {
        return code_;
    }
    // 親を生成する
    MortonCode2D makeParent(int32_t depth) const
    {
        return MortonCode2D( code_ >> (2*depth));
    }
    // 子を作成する
    MortonCode2D makeChild(int32_t chNo) const
    {
        return MortonCode2D((code_ << 2) | (chNo & 0b11));
    }
    // 親から見た番号を返す
    uint32_t chNoParent() const
    {
        return code_ & 0x03;
    }
    // (X,Y)を得る
    std::pair<uint32_t, uint32_t> getXY() const
    {
        return Morton::decodeXY(code_);
    }
    //
private:
private:
    uint32_t code_ = 0;
};

class QuadTree
{
public:
    struct Node
    {
    public:
        //
        std::array<int32_t,4> child = {-1,-1,-1,-1};
        //
        MortonCode2D code_;
    public:
        bool isLeaf() const
        {
            return (child[0] == -1);
        }
    };
public:
    QuadTree()
    {
        // ルートだけ作成する
        nodes_.resize(1);
        nodes_[0].code_ = MortonCode2D(0);
    }
    // 指定したUVにあるNodeを得る
    Node& queryNode(float x, float y)
    {
        return nodes_[queryNodeIndex(x,y)];
    }
    // 指定したUVにあるNodeのIndexを得る
    size_t queryNodeIndex(float x, float y) const
    {
        //
        class Local
        {
        public:
            static size_t queryNodeIndexSub(float x, float y, size_t nodeIdx, const std::vector<Node>& nodes)
            {
                auto& node = nodes[nodeIdx];
                if( node.isLeaf() )
                {
                    return nodeIdx;
                }
                else
                {
                    const bool L = (x <= 0.5f);
                    const bool U = (y <= 0.5f);
                    if(L)
                    {
                        x *= 2.0f;
                    }
                    else
                    {
                        x -= 0.5f;
                        x *= 2.0f;
                    }
                    if(U)
                    {
                        y *= 2.0f;
                    }
                    else
                    {
                        y -= 0.5f;
                        y *= 2.0f;
                    }
                    //
                    if(L)
                    {
                        if(U)
                        {
                            return queryNodeIndexSub(x, y, node.child[0], nodes );
                        }
                        else
                        {
                            return queryNodeIndexSub(x, y, node.child[2], nodes );
                        }
                    }
                    else
                    {
                        if(U)
                        {
                            return queryNodeIndexSub(x, y, node.child[1], nodes );
                        }
                        else
                        {
                            return queryNodeIndexSub(x, y, node.child[3], nodes );
                        }
                    }
                }
            }
        };
        return Local::queryNodeIndexSub(x, y, 0, nodes_);
    }
    // 指定した深さの指定したモートン番号のノードを分割する
    void subdiv(uint32_t x, uint32_t y, uint32_t depth)
    {
        MortonCode2D baseCode(x,y);
        //printf("%d\n", baseCode.code());
        int32_t targetNodeIndex = 0;
        //node
        for(int32_t di=0;di<depth;++di)
        {
            auto& node = nodes_[targetNodeIndex];
            MortonCode2D mc = baseCode.makeParent(depth-di - 1);
            const uint32_t ch = mc.chNoParent();
            const uint32_t nextNodeIdx = node.child[ch];
            if( nextNodeIdx == -1 )
            {
                printf("Wrrong\n");
                return;
            }
            targetNodeIndex = nextNodeIdx;
        }
        auto& node = nodes_[targetNodeIndex];
        // 葉でなければ失敗
        if( !node.isLeaf())
        {
            return;
        }
        // 子を作成
        const size_t nn = nodes_.size();
        node.child[0] = nn + 0;
        node.child[1] = nn + 1;
        node.child[2] = nn + 2;
        node.child[3] = nn + 3;
        const MortonCode2D pcode = node.code_;
        //
        nodes_.resize(nodes_.size()+4);
        nodes_[nn + 0].code_ = pcode.makeChild(0);
        nodes_[nn + 1].code_ = pcode.makeChild(1);
        nodes_[nn + 2].code_ = pcode.makeChild(2);
        nodes_[nn + 3].code_ = pcode.makeChild(3);
    }
    // 内容を出力する
    void print()
    {
        //
        printf("%ld nodes\n", nodes_.size());
        //
        class Local
        {
        public:
            static void printSub(int32_t nodeIdx, int32_t depth, const std::vector<Node>& nodes)
            {
                const auto drawQuad = [](float x, float y, float s, int32_t depth)
                {
                    // 枠をつける
                    const float lz = 0.01f;
                    vdb_color(1.0f, 1.0f, 1.0f);
                    vdb_line(x, y, lz, x+s, y, lz);
                    vdb_line(x, y, lz, x, y+s, lz);
                    vdb_line(x+s, y, lz, x+s, y+s, lz);
                    vdb_line(x, y+s, lz, x+s, y+s, lz);
                    // 矩形の描画。適当に深さで色付け
                    vdb_color(1.0f, depth*0.2f, 0.0f);
                    vdb_triangle(x, y, 0.0f, x+s, y+0, 0.0f, x+0.0f, y+s, 0.0f);
                    vdb_triangle(x+s, y, 0.0f, x, y+s, 0.0f, x+s, y+s, 0.0f);
                };
                //
                auto& node = nodes[nodeIdx];
                if( node.isLeaf() )
                {
                    auto XY = Morton::decodeXY(node.code_.code());
                    const float s = 1.0f/float(1 << depth);
                    const float x = std::get<0>(XY) * s;
                    const float y = std::get<1>(XY) * s;
                    drawQuad(x,y,s, depth);
                }
                else
                {
                    for(int32_t ch : node.child)
                    {
                        printSub(ch, depth + 1, nodes);
                    }
                }
            }
        };
        Local::printSub(0, 0, nodes_);
    }
    
public:
    std::vector<Node> nodes_;
    
};


//
int main()
{
    vdb_frame();
    QuadTree qt;
    qt.subdiv(0, 0, 0);
    qt.subdiv(0, 0, 1);
    qt.subdiv(0, 0, 2);
    qt.subdiv(0, 0, 3);
    qt.subdiv(0, 0, 4);
    //qt.subdiv(1, 0, 1);
    //qt.subdiv(2, 1, 2);
    for(int32_t yi=0;yi<=10;++yi)
    {
        for(int32_t xi=0;xi<10;++xi)
        {
            const float x = float(xi)/10.0f;
            const float y = float(yi)/10.0f;
            printf("%02ld ", qt.queryNodeIndex(x, y));
        }
        printf("\n");
    }
    //
    qt.print();
    return 0;
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
    
    // TODO: 親から見た何番目の子かを返す
    
    
    // TODO: 次元と深さが指定された上でのmorton codeの実装を作成する
    
    return 0;
}

