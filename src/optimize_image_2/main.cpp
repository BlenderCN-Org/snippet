#pragma warning(disable: 4996)


//
#include <Eigen/Core>
#include <Eigen/Dense>
#include <Eigen/LU>
#include <Eigen/SparseCore>
#include <Eigen/Sparse>
//
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>
//
#include <cstdio>
#include <cstdint>
#include <random>
#include <iostream>
#include <array>

class Image
{
public:
    ~Image()
    {
        stbi_image_free(image_);
    }
    void load(const char* fileName)
    {
        image_ = stbi_load(fileName, &width_, &height_, &bpp_, 0);
    }
    void save(const char* fileName)
    {
        stbi_write_png(fileName, width_, height_, bpp_, image_, width_ * bpp_);
    }
    uint8_t* pixel(int32_t x, int32_t y)
    {
        return &image_[(x + y * width_)*bpp_];
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
    uint8_t * image_ = nullptr;
    int32_t width_ = 0;
    int32_t height_ = 0;
    int32_t bpp_ = 0;
};

#define BASE_DIR "C:/project/mine/snippet/src/optimize_regularization"


// リッジ回帰
template<typename M, typename V, typename P>
M ridge(const M& A, const V& y, P alpha) {
    const auto& svd = A.jacobiSvd(Eigen::ComputeFullU | Eigen::ComputeFullV);
    const auto& s = svd.singularValues();
    const auto r = s.rows();
    const auto& D = s.cwiseQuotient((s.array().square() + alpha).matrix()).asDiagonal();
    return svd.matrixV().leftCols(r) * D * svd.matrixU().transpose().topRows(r) * y;
}

struct Vec2
{
public:
    float x;
    float y;
};

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
        return int32_t(vtxColors_.size());
    }
    // 指定した頂点番号の頂点位置を返す
    Vec2 vpos(int32_t idx) const
    {
        const int32_t ix = idx % numVertexSqrt_;
        const int32_t iy = idx / numVertexSqrt_;
        const float x = float(ix)/ float(numVertexSqrtM1_);
        const float y = float(iy) / float(numVertexSqrtM1_);
        return {x,y};
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
        return vtxColors_[left + up * numVertexSqrt_];
        // 左半分
        if (ffx + ffy < 1.0f)
        {
            const float flu  = vtxColors_[left + up * numVertexSqrt_];
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
private:
    const int32_t numVertexSqrt_ = 32;
    const int32_t numVertexSqrtM1_ = numVertexSqrt_ - 1;
    const int32_t numVertex_ = numVertexSqrt_ * numVertexSqrt_;
    std::vector<float> vtxColors_;
};

//
int main()
{
    /*
    TODOs
    - 普通に最適化をする
    - 正則化して書き戻す
    */
    // イメージのロード
    Image img;
    img.load(BASE_DIR "/test.png");
    Mesh mesh;
    //
    for (int32_t vi=0;vi<mesh.numVtx();++vi)
    {
        auto uv = mesh.vpos(vi);
        const int32_t x = int32_t(uv.x * 32.0f);
        const int32_t y = int32_t(uv.y * 32.0f);
        // ポイントサンプル
        mesh.vcol(vi) = float(*img.pixel(x, y))/255.0f;
    }

    // メッシュの内容を書き出し
    for (int32_t y =0; y<img.height()/2;++y)
    {
        for (int32_t x = 0; x < img.width(); ++x)
        {
            const float fx = float(x) / float(img.width());
            const float fy = float(y) / float(img.height());
            uint8_t* p = img.pixel(x,y);
            const float fc = mesh.fetchColor(fx, fy)*250.0f;
            *p = std::min( int32_t(fc), 0xFF);
        }
    }
    // 書き出し
    img.save(BASE_DIR "/result.png");
}