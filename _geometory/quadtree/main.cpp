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
#include <algorithm>
#include <random>
#include "../../thirdparty/vdb/vdb-win/vdb.h"

struct Vec3
{
public:
    float x;
    float y;
    float z;
};

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

template<typename NodeData>
class QuadTree
{
public:
    struct Node :public NodeData
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
    std::vector<Node>& nodes()
    {
        return nodes_;
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
        subdiv(node);
    }
    // 指定したノードを再分割する
    void subdiv(Node& node)
    {
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
    
public:
    std::vector<Node> nodes_;
    
};

struct NodeData
{
public:
    float energy = 0.0f;
};

/*
 - 複数回分割する必要があるときに対応する
 - マージに対応する
 - ノードのクリアみたいな処理はいらないの？
 */

// 内容を出力する
void printQaudTree(const QuadTree<NodeData>& qt)
{
    //
    printf("%ld nodes\n", qt.nodes_.size());
    //
    class Local
    {
    public:
        static void printSub(int32_t nodeIdx, int32_t depth, const std::vector<QuadTree<NodeData>::Node>& nodes)
        {
            const auto drawQuad = [](float x, float y, float s, float energy)
            {
                const auto heatmap = [](float x) -> Vec3
                {
                    Vec3 r;
                    r.x = x; // r
                    r.y = 0.0f; // g
                    r.z = 1.0f - x; // b
                    return r;
                };
#if 1
                // 枠をつける
                const float lz = 0.01f;
                vdb_color(1.0f, 1.0f, 1.0f);
                vdb_line(x, y, lz, x+s, y, lz);
                vdb_line(x, y, lz, x, y+s, lz);
                vdb_line(x+s, y, lz, x+s, y+s, lz);
                vdb_line(x, y+s, lz, x+s, y+s, lz);
#endif
                // エネルギー密度で色つけ
                const float e = energy/(s*s);
#if 0
                const float strength = e/(100.0f+e);
#else
                const float strength = std::logf(e+1.0f) / 10.0f;
#endif
                const auto col = heatmap(strength);
                vdb_color(col.x, col.y, col.z);
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
                drawQuad(x,y,s, node.energy);
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
    Local::printSub(0, 0, qt.nodes_);
}

//
int main()
{
    vdb_frame();
    QuadTree<NodeData> qt;
    //
    const auto trySubdiv = [&]()
    {
        // 総エネルギーを得る
        float totalEnergy = 0.0f;
        for(auto& n : qt.nodes())
        {
            if( n.isLeaf() )
            {
                totalEnergy += n.energy;
            }
        }
        // 再分割する
        const float energyThres = totalEnergy / 100.0f;
        for(auto& n : qt.nodes())
        {
            if( n.isLeaf() && n.energy > energyThres)
            {
                const float e = n.energy / 4.0f;
                n.energy = 0.0f;
                qt.subdiv(n);
                //
                auto& ns = qt.nodes();
                ns[ns.size()-4].energy = e;
                ns[ns.size()-3].energy = e;
                ns[ns.size()-2].energy = e;
                ns[ns.size()-1].energy = e;
                break;
            }
        }
    };
    // ガウシアン分布
    std::random_device rd{};
    std::mt19937 gen{rd()};
    std::normal_distribution<> distX0{0.6f,0.10f};
    std::normal_distribution<> distY0{0.7f,0.05f};
    std::normal_distribution<> distX1{0.3f,0.05f};
    std::normal_distribution<> distY1{0.3f,0.10f};
    for(int32_t i=0;i<1024 * 16;++i)
    {
        const auto push = [&](float x, float y)
        {
            if((x< 0.0f) || (1.0f <= x))
            {
                return;
            }
            if((y< 0.0f) || (1.0f <= y))
            {
                return;
            }
            qt.queryNode(x, y).energy += 0.2f;
            trySubdiv();
        };
        push(distX0(gen), distY0(gen));
        push(distX1(gen), distY1(gen));
    }
    
    //
    //
    printQaudTree(qt);
    return 0;
    
    // TODO: 親から見た何番目の子かを返す
    
    
    // TODO: 次元と深さが指定された上でのmorton codeの実装を作成する
    
    return 0;
}

