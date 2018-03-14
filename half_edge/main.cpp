/*
cf. http://www.flipcode.com/archives/The_Half-Edge_Data_Structure.shtml
*/
#include <cstdio>
#include <cstdint>
#include <vector>
#include <array>

//
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
};

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
        vertices.resize(mesh.vertices.size());
        auto& srcVtx = mesh.vertices;
        vertices.reserve(srcVtx.size());
        std::copy(srcVtx.begin(), srcVtx.end(), std::back_inserter(vertices));

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
            //
            vertices[face[0]].edge = startEdgeNo;
        }
        //
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

}

//
int32_t main()
{
    //
    const Mesh mesh = constructMesh();
    const HalfEdgeMesh heMesh(mesh);
    // TODO: 何かhalfedgeを利用したクエリーを多数行う
    queryTest(mesh, heMesh);
}
