﻿                                                                                                                                                                                                                                                                                                         #include <cstdio>
#include <cstdint>
#include <random>
#include <array>
#include <iostream>
#include <unordered_map>
//
#pragma warning(disable:4996)
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>
//
#include <Eigen/Core>
#include <Eigen/Dense>
#include <Eigen/LU>
#include <Eigen/SparseCore>
#include <Eigen/Sparse>

//
class Image
{
public:
    Image() = default;
    ~Image()
    {
        stbi_image_free(image_);
    }
    void load(const char* fileName)
    {
        image_ = stbi_load(fileName, &width_, &height_, &bpp_, 0);
    }
    void save(const std::string& fileName)
    {
        stbi_write_png(fileName.c_str(), width_, height_, bpp_, image_, width_ * bpp_);
    }
    uint8_t* pixel(int32_t x, int32_t y)
    {
        return &image_[(x + y*width_) * bpp_];
    }
    float pixelF(int32_t x, int32_t y)
    {
        return float(*pixel(x,y))/255.0f;
    }
    uint8_t* buffer()
    {
        return image_;
    }
    int32_t width() const
    {
        return width_;
    }
    int32_t height() const
    {
        return height_;
    }
private:
    int32_t width_ = 0;
    int32_t height_ = 0;
    int32_t bpp_ = 0;
    uint8_t* image_ = nullptr;
};

//
class FloatStreamStats
{
public:
    void add(float v)
    {
        ++n_;
        mu_ += (v - mu_) / float(n_);
    }
    float mu() const
    {
        return mu_;
    }
private:
    int32_t n_ = 0;
    float mu_ = 0.0f;
};

//
struct Vec2
{
public:
    float x;
    float y;
};
//
Vec2 operator + (const Vec2& lhs, const Vec2& rhs) { return { lhs.x + rhs.x,lhs.y + rhs.y }; }
Vec2 operator - (const Vec2& lhs, const Vec2& rhs) { return { lhs.x - rhs.x,lhs.y - rhs.y }; }
Vec2 operator * (const Vec2& v, float s) { return { v.x * s, v.y * s }; }
//
//struct Vec3
//{
//public:
//    float x;
//    float y;
//    float z;
//public:
//    Vec3 normalized() const
//    {
//        const float invLen = 1.0f / std::sqrtf(x*x + y*y + z*z);
//        Vec3 ret;
//        ret.x = invLen * x;
//        ret.y = invLen * y;
//        ret.z = invLen * z;
//        return ret;
//    }
//    Vec3 cross(Vec3 other) const
//    {
//        return
//        {
//            y * other.z - z * other.y,
//            z * other.x - x * other.z,
//            x * other.y - y * other.z
//        };
//    }
//};
//Vec3 operator + (const Vec3& lhs, const Vec3& rhs) { return { lhs.x + rhs.x,lhs.y + rhs.y, lhs.z + rhs.z }; }
//Vec3 operator - (const Vec3& lhs, const Vec3& rhs) { return { lhs.x - rhs.x,lhs.y - rhs.y, lhs.z + rhs.z }; }
//Vec3 operator * (const Vec3& v, float s) { return { v.x * s, v.y * s, v.z * s }; }
typedef Eigen::Matrix<float, 3, 1> Vector3;
typedef Eigen::Matrix<float, 2, 1> Vector2;

//



#define VTX_SHARE_ALL

//
class Mesh
{
public:
    Mesh()
    {
        vtxColors_.resize(numVertex_);
        std::fill(vtxColors_.begin(), vtxColors_.end(), 1.0f);
    }
    int32_t numVtx() const
    {
        return numVertex_;
    }
    int32_t numFace() const
    {
        return numVertexSqrtM1_ * numVertexSqrtM1_ * 2;
    }
    // 指定した頂点番号の頂点位置を返す
    Vec2 vpos(int32_t idx) const
    {
        const int32_t ix = idx % numVertexSqrt_;
        const int32_t iy = idx / numVertexSqrt_;
        const float x = float(ix) / float(numVertexSqrtM1_);
        const float y = float(iy) / float(numVertexSqrtM1_);
        return { x,y };
    }
    // 指定した面番号のインデックスを返す
    std::tuple<int32_t, int32_t, int32_t> index(int32_t faceIdx)
    {
        const int32_t x = faceIdx % (numVertexSqrtM1_ * 2);
        const int32_t y = faceIdx / (numVertexSqrtM1_ * 2);
        // 左
        if (x % 2 == 0)
        {
            const int32_t i0 = numVertexSqrt_ * y + x/2;
            const int32_t i1 = i0 + 1;
            const int32_t i2 = i0 + numVertexSqrt_;
            return {i0, i1, i2};
        }
        else
        {
            const int32_t i0 = numVertexSqrt_ * y + x / 2 + 1;
            const int32_t i1 = i0 + numVertexSqrt_;
            const int32_t i2 = i0 + numVertexSqrtM1_;
            return { i0, i1, i2 };
        }
    }
    // 指定した頂点番号の色を返す
    float& vcol(int32_t idx)
    {
        return vtxColors_[idx];
    }
    // 指定した座標の色を返す
    float fetchColor(float x, float y) const
    {
        // □を判定
        const float fx = x * float(numVertexSqrtM1_);
        const float fy = y * float(numVertexSqrtM1_);
        const int32_t left = int32_t(fx);
        const int32_t up = int32_t(fy);
        const int32_t right = left + 1;
        const int32_t down = up + 1;
        float ffx = fx - float(left);
        float ffy = fy - float(up);
        //
        //return vtxColors_[left + up * numVertexSqrt_];
        // 左半分
        if (ffx + ffy < 1.0f)
        {
            const float flu = vtxColors_[left + up * numVertexSqrt_];
            const float fru = vtxColors_[right + up * numVertexSqrt_];
            const float fld = vtxColors_[left + down * numVertexSqrt_];
            return  (fru - flu)*ffx + (fld - flu)*ffy + flu;
        }
        // 右半分
        else
        {
            ffx = 1.0f - ffx;
            ffy = 1.0f - ffy;
            //
            const float fru = vtxColors_[right + up * numVertexSqrt_];
            const float fld = vtxColors_[left + down * numVertexSqrt_];
            const float frd = vtxColors_[right + down * numVertexSqrt_];
            return  (fld - frd)*ffx + (fru - frd)*ffy + frd;
        }
    }
    void writeToImage(Image& img)
    {
        // メッシュの内容を書き出し
        for (int32_t y = 0; y<img.height(); ++y)
        {
            for (int32_t x = 0; x < img.width(); ++x)
            {
                const float fx = float(x) / float(img.width());
                const float fy = float(y) / float(img.height());
                uint8_t* p = img.pixel(x, y);
                const float fc = fetchColor(fx, fy)*255.0f;
                *p = std::max(std::min(int32_t(fc), 0xFF),0x00);
            }
        }
    }
private:
    const int32_t numVertexSqrt_ = 64;
    const int32_t numVertexSqrtM1_ = numVertexSqrt_ - 1;
    const int32_t numVertex_ = numVertexSqrt_ * numVertexSqrt_;
    std::vector<float> vtxColors_;
};

// 低ランク近似
static void test0()
{
    // イメージのロード
    Image img;
    img.load("../test3.png");
    printf("Load done.\n");
    // TOOD: 行列にロードしてSVDしてランクを下げて書き戻す実験
    Eigen::MatrixXf M;
    M.resize(img.width(), img.height());
    for (int32_t y = 0; y<img.height(); ++y)
    {
        for (int32_t x = 0; x < img.width(); ++x)
        {
            M(x, y) = float(*img.pixel(x, y)) / 255.0f;
        }
    }
    auto svd = M.jacobiSvd(Eigen::ComputeFullU | Eigen::ComputeFullV);
    printf("SVD done.\n");
    Eigen::MatrixXf sigma;
    sigma.resize(img.width(), img.height());
    //
    for (int32_t rank = 1; rank<100; rank += 10)
    {
        auto U = svd.matrixU();
        auto V = svd.matrixV();
        auto sv = svd.singularValues();
        sigma.setZero();
        for (int i = 0; i < rank; i++) {
            sigma(i, i) = sv(i);
        }
        Eigen::MatrixXf Md = U * sigma * V.transpose();
        // 書き戻す
        for (int32_t y = 0; y < img.height(); ++y)
        {
            for (int32_t x = 0; x < img.width(); ++x)
            {
                *img.pixel(x, y) = int32_t(std::min(Md(x, y) * 255.0f, 255.0f));
            }
        }
        printf("rank %d done.\n", rank);
        std::stringstream ss;
        ss << "../result" << rank << ".png";
        auto str = ss.str();
        img.save(str.c_str());
        printf("write done.\n");
    }
}
//
void test1()
{
    // ポイントサンプルで書き出し
    Mesh mesh;
    Image img;
    img.load("../src.png");
    //
    for (int32_t vi = 0; vi<mesh.numVtx(); ++vi)
    {
        auto uv = mesh.vpos(vi);
        const int32_t x = int32_t(uv.x * img.width());
        const int32_t y = int32_t(uv.y * img.height());
        // ポイントサンプル
        mesh.vcol(vi) = float(*img.pixel(x, y)) / 255.0f;
    }
    // 書き出し
    mesh.writeToImage(img);
    img.save("../pointsample.png");
}

// エリアサンプルして書き出し
void test2()
{
    std::mt19937 eng(0x123);
    std::uniform_real_distribution<float> dist(0.0f, 1.0f);
    const auto rng01 = [&]()
    {
        return dist(eng);
    };
    const auto triSample = [&]() -> std::pair<float,float>
    {
        const float x = rng01();
        const float y = rng01();
        if (x + y > 1.0f)
        {
            return { 1.0f - x,1.0f - y };
        }
        else
        {
            return { x,y };
        }
    };
    // エリアサンプルで書き出し
    Mesh mesh;
    Image img;
    img.load("../src.png");
    //
    std::vector<FloatStreamStats> fs;
    fs.resize(mesh.numVtx());
    for(int32_t fi=0;fi<mesh.numFace();++fi)
    {
        auto idx = mesh.index(fi);
        const int32_t i0 = std::get<0>(idx);
        const int32_t i1 = std::get<1>(idx);
        const int32_t i2 = std::get<2>(idx);
        const Vec2 v0 = mesh.vpos(i0);
        const Vec2 v1 = mesh.vpos(i1);
        const Vec2 v2 = mesh.vpos(i2);

        // エリアサンプル
        for (int32_t sn = 0; sn < 512; ++sn)
        {
            const auto tw = triSample();
            const Vec2 uv = v0 + (v1 - v0) * std::get<0>(tw) + (v2 - v0) * std::get<1>(tw);
            const int32_t x = int32_t(uv.x * img.width());
            const int32_t y = int32_t(uv.y * img.height());
            const float s = img.pixelF(x, y);
            fs[i0].add(s);
            fs[i1].add(s);
            fs[i2].add(s);
        }
    }
    for (int32_t vi=0;vi<mesh.numVtx();++vi)
    {
        mesh.vcol(vi) = fs[vi].mu();
    }
    // 書き出し
    mesh.writeToImage(img);
    img.save("../area.png");
}

/*
Tikhonov regularization, ridge regression
https://en.wikipedia.org/wiki/Tikhonov_regularization
*/
Eigen::MatrixXf ridgeRegression(const Eigen::MatrixXf& A, const Eigen::VectorXf& b, float alpha)
{
    /*
    最終的には
    $$(A^{T} A + \alpha^2 I ) \hat{x} = A^{T}b$$
    をxについて解けばよい。
    ① そのまま $$A' \hat{x} = b'$$ の形として見なして解く
    ➁ SDVした結果のV行列の左は$$A^{T}A$$の固有値、U行列の左は$$A A^{T}$$の固有値であることを使って式変形
    */
#if 0
    //
    auto I = Eigen::MatrixXf::Identity(A.cols(), A.cols());
    auto Ad = (A.transpose() * A + alpha * alpha * I);
    auto bd = A.transpose() * b;
    return Ad.jacobiSvd(Eigen::ComputeThinU | Eigen::ComputeThinV).solve(bd);
#else
    const auto& svd = A.jacobiSvd(Eigen::ComputeFullU | Eigen::ComputeFullV);
    const auto& s = svd.singularValues();
    const auto r = s.rows();
    const auto& D = s.cwiseQuotient((s.array().square() + alpha * alpha).matrix()).asDiagonal();
    return svd.matrixV().leftCols(r) * D * svd.matrixU().transpose().topRows(r) * b;
#endif
}

// 面単位で最適化して出す
void test3()
{
    std::mt19937 eng(0x123);
    std::uniform_real_distribution<float> dist(0.0f, 1.0f);
    const auto rng01 = [&]()
    {
        return dist(eng);
    };
    const auto triSample = [&]() -> std::pair<float, float>
    {
        const float x = rng01();
        const float y = rng01();
        if (x + y > 1.0f)
        {
            return { 1.0f - x,1.0f - y };
        }
        else
        {
            return { x,y };
        }
    };

    //for (float alpha : std::array<float, 5>({ 0.0f, 0.01f, 0.1f, 0.2f, 0.5f }))
    {
        //
        const float alpha = 0.0f;
        //
        Mesh mesh;
        Image img;
        img.load("../src.png");
        std::vector<FloatStreamStats> fs;
        fs.resize(mesh.numVtx());
        for (int32_t fi = 0; fi < mesh.numFace(); ++fi)
        {
            auto idx = mesh.index(fi);
            const int32_t i0 = std::get<0>(idx);
            const int32_t i1 = std::get<1>(idx);
            const int32_t i2 = std::get<2>(idx);
            const Vec2 v0 = mesh.vpos(i0);
            const Vec2 v1 = mesh.vpos(i1);
            const Vec2 v2 = mesh.vpos(i2);

            // 最適化
            const int32_t numSample = 128;
            Eigen::MatrixXf A;
            Eigen::VectorXf b;
            A.resize(numSample, 3);
            b.resize(numSample);
            for (int32_t sn = 0; sn < numSample; ++sn)
            {
                const auto uv = triSample();
                const float u = std::get<0>(uv);
                const float v = std::get<1>(uv);
                const Vec2 uvIimage = v0 + (v1 - v0) * u + (v2 - v0) * v;
                const int32_t x = int32_t(uvIimage.x * img.width());
                const int32_t y = int32_t(uvIimage.y * img.height());
                const float s = img.pixelF(x, y);
                A(sn, 0) = 1.0f;
                A(sn, 1) = u;
                A(sn, 2) = v;
                b(sn) = s;
            }
            // 通常のリッジ回帰
            Eigen::VectorXf solved = ridgeRegression(A, b, alpha);
            const float a0 = solved(0);
            const float a1 = solved(0) + solved(1);
            const float a2 = solved(0) + solved(2);
            //
            fs[i0].add(a0);
            fs[i1].add(a1);
            fs[i2].add(a1);
        }
        for (int32_t vi = 0; vi < mesh.numVtx(); ++vi)
        {
            mesh.vcol(vi) = fs[vi].mu();
            //mesh.vcol(vi) = float(vi)/float(mesh.numVtx()-1);
        }
        // 書き出し
        mesh.writeToImage(img);
        std::stringstream ss;
        ss << "../_opt" << alpha << ".png";
        img.save(ss.str());
    }
}

// 全ての面を使って最適化
void test4()
{
    std::mt19937 eng(0x123);
    std::uniform_real_distribution<float> dist(0.0f, 1.0f);
    const auto rng01 = [&]()
    {
        return dist(eng);
    };
    const auto triSample = [&]() -> std::pair<float, float>
    {
        const float x = rng01();
        const float y = rng01();
        if (x + y > 1.0f)
        {
            return { 1.0f - x,1.0f - y };
        }
        else
        {
            return { x,y };
        }
    };

    //for (float alpha : std::array<float, 5>({ 0.0f, 0.01f, 0.1f, 0.2f, 0.5f }))
    {
        const float alpha = 0.1f;
        //
        Mesh mesh;
        Image img;
        img.load("../src.png");
        //
        const int32_t numSamplePerTri = 128;
        Eigen::SparseMatrix<float, Eigen::RowMajor> A(numSamplePerTri*mesh.numFace(), mesh.numVtx());
        Eigen::VectorXf b(numSamplePerTri*mesh.numFace());
        // 行毎に3つ要素を確保する
        A.reserve(Eigen::VectorXi::Constant(numSamplePerTri*mesh.numFace(), 3));
        //
        for (int32_t fi = 0; fi < mesh.numFace(); ++fi)
        {
            auto idx = mesh.index(fi);
            const int32_t i0 = std::get<0>(idx);
            const int32_t i1 = std::get<1>(idx);
            const int32_t i2 = std::get<2>(idx);
            const Vec2 v0 = mesh.vpos(i0);
            const Vec2 v1 = mesh.vpos(i1);
            const Vec2 v2 = mesh.vpos(i2);
            const int32_t snBase = fi * numSamplePerTri;
            // 最適化
            for (int32_t sn = 0; sn < numSamplePerTri; ++sn)
            {
                const auto uv = triSample();
                const float u = std::get<0>(uv);
                const float v = std::get<1>(uv);
                const Vec2 uvIimage = v0 + (v1 - v0) * u + (v2 - v0) * v;
                const int32_t x = int32_t(uvIimage.x * img.width());
                const int32_t y = int32_t(uvIimage.y * img.height());
                const float s = img.pixelF(x, y);
                A.insert(snBase+sn, i0) = 1.0f - (u+v);
                A.insert(snBase + sn, i1) = u;
                A.insert(snBase + sn, i2) = v;
                b(snBase + sn) = s;
            }
        }
        // eq.9, Tikhonov
        const float scale = 1.0f + alpha;
        A *= scale;
        //
        Eigen::LeastSquaresConjugateGradient<Eigen::SparseMatrix<float>> solver;
        solver.compute(A);
        const Eigen::VectorXf solved = solver.solve(b);
        //
        for (int32_t vi = 0; vi < mesh.numVtx(); ++vi)
        {
            mesh.vcol(vi) = solved(vi);
        }
        // 書き出し
        printf("write image\n");
        mesh.writeToImage(img);
        std::stringstream ss;
        ss << "../_opt_area" << alpha << ".png";
        img.save(ss.str());
    }
}

// https://github.com/nvpro-samples/optix_prime_baking/blob/master/bake_filter_least_squares.cpp
struct CreateMassMatrixSampleInfo
{
public:
    // wvu
    float u;
    float v;
    // 頂点インデックス
    int32_t vi0;
    int32_t vi1;
    int32_t vi2;
    // サンプル値
    float value;
};

// 
void createMassMatrixAndB(
    std::function<CreateMassMatrixSampleInfo(int32_t)> getSample,
    int32_t numSampleTotal,
    int32_t numVertex,
    int32_t numFace,
    Eigen::SparseMatrix<float>& massMatrix,
    Eigen::VectorXf& vertexAos // TODO: もう少しわかりやすい名前に変更する
)
{
    //
    std::map<std::pair<int32_t, int32_t>, float> tripletMap;
    const int32_t sampleSizePerPrim = numSampleTotal/numFace;
    const float invSamplesize = 1.0f / float(sampleSizePerPrim);
    //
    vertexAos.resize(numVertex);
    vertexAos.setZero();
    //
    for (int32_t sn=0;sn<numSampleTotal;++sn)
    {
        const auto si = getSample(sn);
        /*
        eq(4)の
        $$A_{ ij } = \int_{ S }{h_i(p)h_j(p)} dp$$
        を解く。
        A_{ii}は頂点iが属する面積の総和を6で割り、
        A_{ij}はエッジijが属する面積の総和を12で割れば解析的に出るが
        少ないサンプル数ではAも数値積分すると答えが安定する(らしい)
        */
        const float dA = invSamplesize; // TODO: 面積を掛ける必要がある
        const float w = 1.0f - (si.u + si.v);
        const float ww = w * w;
        const float uu = si.u * si.u;
        const float vv = si.v * si.v;
        const float wu = w * si.u;
        const float wv = w * si.v;
        const float uv = si.u * si.v;
        // A_{ii}
        tripletMap[std::make_pair(si.vi0, si.vi0)] += ww * dA;
        tripletMap[std::make_pair(si.vi1, si.vi1)] += vv * dA;
        tripletMap[std::make_pair(si.vi2, si.vi2)] += uu * dA;
        // A_{ij}
        tripletMap[std::make_pair(si.vi0, si.vi1)] += wu * dA;
        tripletMap[std::make_pair(si.vi1, si.vi0)] += wu * dA;
        tripletMap[std::make_pair(si.vi0, si.vi2)] += wv * dA;
        tripletMap[std::make_pair(si.vi2, si.vi0)] += wv * dA;
        tripletMap[std::make_pair(si.vi1, si.vi2)] += uv * dA;
        tripletMap[std::make_pair(si.vi2, si.vi1)] += uv * dA;
        /*
        eq(5)の
        $$b_{ i } = \int_{ S }{h_i(p)f(p)} dp$$
        */
        vertexAos[si.vi0] += w * si.value * dA;
        vertexAos[si.vi1] += si.u * si.value * dA;
        vertexAos[si.vi2] += si.v * si.value * dA;
    }
    float ao0 = vertexAos[0];
    float ao1 = vertexAos[1];
    float ao2 = vertexAos[2];

    // Mass行列の構築
    // NOTE: setFromTriplets()を使わないで自前でソートした方が早いかもしれない
    std::vector<Eigen::Triplet<float>> triplets;
    triplets.reserve(tripletMap.size());
    for (auto it : tripletMap)
    {
        triplets.push_back(Eigen::Triplet<float>(it.first.first, it.first.second, it.second));
    }
    massMatrix.resize(numVertex, numVertex);
    massMatrix.setFromTriplets(triplets.begin(), triplets.end());
    // どこからも参照されていない頂点があると対角要素に0ができてしまうので1を入れておく
    const Eigen::VectorXf lumped = massMatrix * Eigen::VectorXf::Ones(numVertex);
    for (int i = 0; i < numVertex; ++i)
    {
        if (lumped(i) <= 0.0)
        {
            massMatrix.coeffRef(i, i) = 1.0;
        }
    }
}

//
struct Butterfly
{
    std::pair<int32_t, int32_t> wingverts = std::make_pair(-1, -1);
    int32_t count = 0;
    Butterfly() = default;
};
typedef std::map<std::pair<int, int>, Butterfly > EdgeMap;
typedef Eigen::Matrix<float, 2, 4> Matrix24;
typedef Eigen::Matrix<float, 2, 3> Matrix23;
typedef Eigen::Matrix<float, 4, 4> Matrix44;

//
float triangleArea(const Vector3& a, const Vector3& b, const Vector3& c)
{
    Vector3 ba = b - a, ca = c - a;
    Vector3 crop = ba.cross(ca);
    return crop.norm() * 0.5f;
}

// Embeds 3D triangle v[0], v[1], v[2] into a plane, such that:
//  p[0] = (0, 0), p[1] = (0, positive number), p[2] = (positive number, any number)
// If triangle is close to degenerate returns false and p is undefined.
bool planarizeTriangle(const Vector3 v[3], Vector2 p[3])
{
    double l01 = (v[0] - v[1]).norm();
    double l02 = (v[0] - v[2]).norm();
    double l12 = (v[1] - v[2]).norm();

    const double eps = 0.0;
    if (l01 <= eps || l02 <= eps || l12 <= eps) return false;

    double p2y = (l02*l02 + l01*l01 - l12*l12) / (2.0 * l01);
    double tmp1 = l02*l02 - p2y*p2y;
    if (tmp1 <= eps) return false;

    p[0] = Vector2(0.0f, 0.0f);
    p[1] = Vector2(0.0f, l01);
    p[2] = Vector2(sqrt(tmp1), p2y);
    return true;
}

// Computes gradient operator (2 x 3 matrix 'grad') for a planar triangle.  If
// 'normalize' is false then division by determinant is off (and thus the
// routine cannot fail even for degenerate triangles).
bool triGrad2D(const Vector2 p[3], const bool normalize, Matrix23 &grad)
{
    double det = 1.0;
    if (normalize) {
        det = -double(p[0](1))*p[1](0) + double(p[0](0))*p[1](1) + double(p[0](1))*p[2](0)
            - double(p[1](1))*p[2](0) - double(p[0](0))*p[2](1) + double(p[1](0))*p[2](1);
        const double eps = 0.0;
        if (fabs(det) <= eps) {
            return false;
        }
    }

    grad(0, 0) = p[1](1) - p[2](1);
    grad(0, 1) = p[2](1) - p[0](1);
    grad(0, 2) = p[0](1) - p[1](1);

    grad(1, 0) = p[2](0) - p[1](0);
    grad(1, 1) = p[0](0) - p[2](0);
    grad(1, 2) = p[1](0) - p[0](0);

    grad /= det;
    return true;
}

// Computes difference of gradients operator (2 x 4 matrix 'GD') for a butterfly, i.e., 
// two edge-adjacent triangles.
// Performs normalization so that units are [m], so GD^T * GD will have units of area [m^2]:
bool butterflyGradDiff(const Vector3 v[4], Matrix24& GD)
{
    Vector3 v1[3] = { v[0], v[1], v[2] };
    Vector3 v2[3] = { v[0], v[1], v[3] };
    Vector2 p1[3], p2[3];
    bool success = planarizeTriangle(v1, p1);
    if (!success) return false;
    success = planarizeTriangle(v2, p2);
    if (!success) return false;
    p2[2](0) *= -1.0; // flip the x coordinate of the last vertex of the second triangle so we get a butterfly

    Matrix23 grad1, grad2;
    success = triGrad2D(p1, /*normalize*/ true, grad1);
    if (!success) return false;
    success = triGrad2D(p2, true, grad2);
    if (!success) return false;

    Matrix24 gradExt1, gradExt2;
    for (int i = 0; i<2; i++) {
        gradExt1(i, 0) = grad1(i, 0);  gradExt1(i, 1) = grad1(i, 1);  gradExt1(i, 2) = grad1(i, 2); gradExt1(i, 3) = 0.0;
        gradExt2(i, 0) = grad2(i, 0);  gradExt2(i, 1) = grad2(i, 1);  gradExt2(i, 2) = 0.0;         gradExt2(i, 3) = grad2(i, 2);
    }
    GD = gradExt1 - gradExt2;

    const float area1 = triangleArea(v1[0], v1[1], v1[2]);
    const float area2 = triangleArea(v2[0], v2[1], v2[2]);
    GD *= (area1 + area2);

    return true;
}

//
void createRegressionMatrix(
    int32_t numFace,
    int32_t numVertex,
    const std::function<std::array<int32_t,3>(int32_t)>& getIndexes,
    const std::function<Vector3(int32_t)>& getVertex,
    Eigen::SparseMatrix<float>& regularization)
{
    // 同じエッジを挟んだ反対側の頂点同士をまとめる
    EdgeMap edges;
    for (size_t fn = 0; fn < numFace; ++fn)
    {
        const auto indices = getIndexes(fn);
        for (int vi = 0; vi < 3; ++vi)
        {
            const int32_t index0 = std::min(indices[vi], indices[(vi + 1) % 3]);
            const int32_t index1 = std::max(indices[vi], indices[(vi + 1) % 3]);
            const std::pair<int32_t, int32_t> edge = std::make_pair(index0, index1);
            if (index0 == indices[vi])
            {
                edges[edge].wingverts.first = indices[(vi + 2) % 3];
                edges[edge].count++;
            }
            else
            {
                edges[edge].wingverts.second = indices[(vi + 2) % 3];
                edges[edge].count++;
            }

        }
    }
    // 
    size_t skipped = 0;
    std::map<std::pair<int, int>,float> tripletMap;
    size_t edgeIndex = 0;
    for (auto ite = edges.begin(); ite != edges.end(); ++ite, ++edgeIndex)
    {
        if (ite->second.count != 2)
        {
            continue;  // not an interior edge, ignore
        }

        std::array<int32_t,4> vertIdx =
        {
            ite->first.first,
            ite->first.second,
            ite->second.wingverts.first,
            ite->second.wingverts.second
        };
        // duplicate face, ignore
        if ((ite->second.wingverts.first < 0) ||
            (ite->second.wingverts.second < 0))
        {
            continue;
        }

        Vector3 butterfly_verts[4];
        for (size_t i = 0; i < 4; ++i)
        {
            butterfly_verts[i] = getVertex(vertIdx[i]);
        }
        //
        Matrix24 GD;
        if (!butterflyGradDiff(butterfly_verts, GD))
        {
            skipped++;
            continue;
        }

        Matrix44 GDtGD = GD.transpose() * GD; // units will now be [m^2]
                                              // scatter GDtGD:
        for (int i = 0; i < 4; i++)
        {
            for (int j = 0; j < 4; j++)
            {
                tripletMap[std::make_pair(vertIdx[i], vertIdx[j])] += GDtGD(i, j);
            }
        }
    }
    //
    std::vector<Eigen::Triplet<float>> triplets;
    triplets.reserve(tripletMap.size());
    for (auto it = tripletMap.begin(); it != tripletMap.end(); ++it) {
        triplets.push_back(Eigen::Triplet<float>(it->first.first, it->first.second, it->second));
    }
    regularization.resize(numVertex, numVertex);
    regularization.setFromTriplets(triplets.begin(), triplets.end());
    //
    if (skipped > 0)
    {
        std::cerr << "edgeBasedRegularizer: skipped " << skipped << " edges out of " << edges.size() << std::endl;
    }
}

//
void test5()
{
    std::mt19937 eng(0x123);
    std::uniform_real_distribution<float> dist(0.0f, 1.0f);
    const auto rng01 = [&]()
    {
        return dist(eng);
    };
    const auto triSample = [&]() -> std::pair<float, float>
    {
        const float x = rng01();
        const float y = rng01();
        if (x + y > 1.0f)
        {
            return { 1.0f - x,1.0f - y };
        }
        else
        {
            return { x,y };
        }
    };

    //
    Mesh mesh;
    Image img;
    img.load("../src.png");
    printf("load done\n");
    //
    const int32_t numVertex = mesh.numVtx();
    const int32_t numFace = mesh.numFace();
    const int32_t numSample = 128 * numFace;
    Eigen::SparseMatrix<float> massMatrix;
    Eigen::VectorXf vertexAos;
    createMassMatrixAndB([numSample,triSample,&mesh,&img](int32_t sn)
    {
        const int32_t faceNo = int32_t(size_t(mesh.numFace()) * size_t(sn) / size_t(numSample));
        auto idxs = mesh.index(faceNo);
        const int32_t idx0 = std::get<0>(idxs);
        const int32_t idx1 = std::get<1>(idxs);
        const int32_t idx2 = std::get<2>(idxs);
        const Vec2 v0 = mesh.vpos(idx0);
        const Vec2 v1 = mesh.vpos(idx1);
        const Vec2 v2 = mesh.vpos(idx2);
        //
        auto uv = triSample();
        const float u = std::get<0>(uv);
        const float v = std::get<1>(uv);
        const Vec2 uvIimage = v0 + (v1 - v0) * u + (v2 - v0) * v;
        const int32_t x = int32_t(uvIimage.x * img.width());
        const int32_t y = int32_t(uvIimage.y * img.height());
        const float value = img.pixelF(x, y);
        //
        CreateMassMatrixSampleInfo si;
        si.u = u;
        si.v = v;
        si.vi0 = idx0;
        si.vi1 = idx1;
        si.vi2 = idx2;
        si.value = value;
        return si;
    }, numSample, numVertex, numFace, massMatrix, vertexAos);

    Eigen::SparseMatrix<float> regression;
    createRegressionMatrix(numFace, numVertex, 
        [&](int32_t faceNo) ->std::array<int32_t, 3>
    {
        auto is = mesh.index(faceNo);
        return {std::get<0>(is), std::get<1>(is), std::get<2>(is)};
    },
     [&](int32_t vi)
    {
        const Vec2 p = mesh.vpos(vi);
        return Vector3(p.x, p.y, 0.0f);
    }, regression);

    printf("constrct matrxi done\n");
    //
    Eigen::SimplicialLDLT<Eigen::SparseMatrix<float>> solver;
    auto nonz = regression.nonZeros();
    auto A = massMatrix + regression * 0.0f;
    solver.compute(A);
    const Eigen::VectorXf solved = solver.solve(vertexAos);
    printf("solve done\n");
    // 書き出し
    for (int32_t vi = 0; vi < mesh.numVtx(); ++vi)
    {
        mesh.vcol(vi) = solved(vi);
    }
    mesh.writeToImage(img);
    img.save("../_opt.png");
}

//
int32_t main()
{
    //test0();
    //test1();
    //test2();
    //test3();
    test4();
    //test5();
    
    //
    return 0;
}