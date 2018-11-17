/*
cf. http://www.flipcode.com/archives/The_Half-Edge_Data_Structure.shtml
*/
#include <cstdio>
#include <cstdint>
#include <vector>
#include <array>
#include <unordered_set>

#include "../../thirdparty/vdb/vdb-win/vdb.h"

// 通常のメッシュ構造
struct Mesh
{
public:
    struct Vertex
    {
    public:
        float x;
        float y;
        float z;
    };
    typedef std::array<int32_t, 3> Face;
    std::vector<Face> faces;
    std::vector<Vertex> vertices;

public:
    //
    std::vector<int32_t> findAdjVtx(int32_t vtxIdx) const
    {
        std::unordered_set<int> s;
        for (auto& face : faces)
        {
            if ((face[0] == vtxIdx) ||
                (face[1] == vtxIdx) ||
                (face[2] == vtxIdx))
            {
                for (int32_t i=0;i<3;++i)
                {
                    if (face[i] != vtxIdx)
                    {
                        s.insert(face[i]);
                    }
                }
            }
        }
        //
        std::vector<int32_t> ret(s.begin(),s.end());
        std::sort(ret.begin(), ret.end());
        return ret;
    }

    //
    void print() const
    {
        for (auto& face : faces)
        {
            const int32_t i0 = face[0];
            const int32_t i1 = face[1];
            const int32_t i2 = face[2];
            const auto v0 = vertices[i0];
            const auto v1 = vertices[i1];
            const auto v2 = vertices[i2];
            const auto drawline = [](const Vertex& v0, const Vertex& v1)
            {
                vdb_line(v0.x, v0.y, v0.z, v1.x, v1.y, v1.z);
            };
            drawline(v0, v1);
            drawline(v1, v2);
            drawline(v2, v0);            
        }
    }
};

// ハーフエッジのメッシュ構造
struct HalfEdgeMesh
{
    //
    struct Vertex
    {
        float x;
        float y;
        float z;
        // その頂点が属している面のエッジのどれか
        int32_t edge;
    };
    //
    struct Edge
    {
    public:
        // 終点の頂点番号
        int32_t vertex;
        // 同じEdgeを共有する反対側のEdge
        int32_t pair;
        // Face番号
        int32_t face;
        // 面の中の次のエッジの番号
        int32_t next;
    };
    std::vector<Vertex> vertices;
    std::vector<Edge> edges;

public:
    //
    HalfEdgeMesh(const Mesh& mesh)
    {
        const auto& srcVtx = mesh.vertices;
        vertices.reserve(srcVtx.size());
        for (int32_t vi=0;vi<srcVtx.size();++vi)
        {
            auto& dst = vertices[vi];
            auto& src = srcVtx[vi];
            dst.x = dst.x;
            dst.y = dst.y;
            dst.z = dst.z;
        }

        // TODO: 
        for (int32_t fn = 0; fn<mesh.faces.size(); ++fn)
        {
            auto& face = mesh.faces[fn];
            auto startEdgeNo = edges.size();
            for (int32_t i = 0; i<3; ++i)
            {
                HalfEdgeMesh::Edge newEdge;
                newEdge.vertex = face[i];
                newEdge.pair; // TODO: どうやってpairを効率的に得るのだ？
                newEdge.face = fn;
                newEdge.next = startEdgeNo + ((i + 1) % 3);
                //
                edges.push_back(newEdge);
            }
            pairの詰め方は
        https://github.com/neilmendoza/ofxHalfEdgeMesh/blob/master/src/core/Mesh.cpp
            辺りを見れば良さそう
            //
            vertices[face[0]].edge = startEdgeNo;
        }
        //
    }

    //
    std::vector<int32_t> findAdjVtx(int32_t vtxIdx) const
    {
        const int32_t startEdge = vertices[vtxIdx].edge;
        TODO: 実装
        //
        std::vector<int32_t> ret(s.begin(), s.end());
        std::sort(ret.begin(), ret.end());
        return ret;
    }
};

// 何か適当なメッシュを作成する
Mesh constructMesh()
{
    Mesh ret;
    // TODO: 碁盤の目の作成
    const int32_t nx = 10;
    const int32_t ny = 10;
    for (int32_t y = 0; y < ny; ++y)
    {
        for (int32_t x = 0; x < nx; ++x)
        {
            ret.vertices.push_back({ float(x) * 1.0f, float(y) * 1.0f, 0.0f });
        }
    }
    for (int32_t y = 0; y < ny - 1; ++y)
    {
        for (int32_t x = 0; x < nx - 1; ++x)
        {
            // 左上
            int32_t i0 = (x + 0) + (y + 0) * nx;
            // 右上
            int32_t i1 = (x + 1) + (y + 0) * nx;
            // 左下
            int32_t i2 = (x + 0) + (y + 1) * nx;
            // 右下
            int32_t i3 = (x + 1) + (y + 1) * nx;
            //
            ret.faces.push_back({ i0,i1,i2 });
            ret.faces.push_back({ i2,i1,i3 });
        }
    }
    //
    return ret;
}

//
void queryTest(const Mesh& mesh, const HalfEdgeMesh& heMesh)
{
    // 指定した頂点の周りにある頂点を取ってくる
    for (auto& i : mesh.findAdjVtx(12))
    {
        printf("%d\n",i);
    }
    for (auto& i : heMesh.findAdjVtx(12))
    {
        printf("%d\n", i);
    }
}

//
int32_t main()
{
    //
    const Mesh mesh = constructMesh();
    //mesh.print();
    const HalfEdgeMesh heMesh(mesh);
    // TODO: 何かhalfedgeを利用したクエリーを多数行う
    queryTest(mesh, heMesh);
}
