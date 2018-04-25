#include <cstdio>
#include <cstdint>
//
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
    void save(const char* fileName)
    {
        stbi_write_png(fileName, width_, height_, bpp_, image_, width_ * bpp_);
    }
    uint8_t* pixel(int32_t x, int32_t y)
    {
        return &image_[(x + y*width_) * bpp_];
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
    // TODO: 三角形を定義してそれをピクセルとして書き出せるようにする
}

//
int32_t main()
{
    //test0();
    test1();
    
    //
    return 0;
}