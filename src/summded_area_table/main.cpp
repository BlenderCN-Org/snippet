/*
 TODOs
 - 特定領域で重点サンプリングできるようにする。
 - カラムメジャーに置き換えたりできるようにする
 - if()をなくす。ボーダーの領域に値を入れておく
 */

#include "sat.h"

// third party
#pragma warning(disable:4996)
#define STB_IMAGE_IMPLEMENTATION
#include "../../thirdparty/stb/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../../thirdparty/stb/stb_image_write.h"

// CRT
#include <cstdio>
#include <vector>
#include <cstdint>

#include <random>
#include <tuple>

//
struct Pixel
{
public:
    float r;
    float g;
    float b;
public:
    float Y() const
    {
        return r + g + b;
    }
};

//
class HdrImage
{
public:
    void load( const char* fileName)
    {
        int32_t comp = 0;
        float* p = stbi_loadf(fileName, &width_, &height_, &comp, 0);
        data_.resize(width_ * height_);
        for(int32_t x=0;x<width_;++x)
        {
            for(int32_t y=0;y<height_;++y)
            {
                const int32_t idx = x + y * width_;
                auto& dst = data_[idx];
                dst.r = p[idx*comp+0];
                dst.g = p[idx*comp+1];
                dst.b = p[idx*comp+2];
            }
        }
        stbi_image_free(p);
    }
    void write(const char* fileName)const
    {
        stbi_write_hdr(fileName, width_, height_, 3, (const float*)data_.data());
    }
    void resize(int32_t width, int32_t height)
    {
        width_ = width;
        height_ = height;
        data_.resize(width*height);
    }
    Pixel value(int32_t x, int32_t y) const
    {
        const int32_t idx = x + y * width_;
        return data_[idx];
    }
    void setValue(int32_t x, int32_t y, Pixel p)
    {
        const int32_t idx = x + y * width_;
        data_[idx] = p;
    }
    void assign(const HdrImage& other)
    {
        //
        resize(other.width(), other.height());
        //
        auto& srcImage = other.pixels();
        for (int32_t x = 0; x<width_; ++x)
        {
            for (int32_t y = 0; y<height_; ++y)
            {
                const int32_t idx = x + y * width_;
                auto& dst = data_[idx];
                const auto& src = srcImage[idx];
                dst = src;
            }
        }
    }
    const std::vector<Pixel>& pixels() const
    {
        return data_;
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
    std::vector<Pixel> data_;
};

//
class FloatStreamStats
{
public:
    void add(float v)
    {
        ++n_;
        const float delta = v - mu_;
        const float delta_n = delta / float(n_);
        mu_ += delta_n;
    }
    float mu() const
    {
        return mu_;
    }
private:
    float mu_ = 0.0f;
    int32_t n_;
};

//
void samplingMain()
{
    // imageのロード
    HdrImage img;
    img.load("../machine_shop_01_1k.hdr");
    // SATの作成
    SummedAreaTable<double> sat;
    sat.construct(img.width(), img.height(), [&](int32_t x, int32_t y)
    //sat.construct(3, 3, [&](int32_t x, int32_t y)
                  {
                      //return 1.0f/(16 * 16);
                      //return 1.0f;
                      auto p = img.value(x,y);
                      return p.Y();
                  });
    
    // 普通に全体の合計値を出す
    float trueValue = 0.0f;
    {
        FloatStreamStats fs;
        for(auto& p : img.pixels())
        {
            fs.add(p.Y());
        }
        printf("TrueValue: %f\n", fs.mu());
        trueValue = fs.mu();
    }

    struct
    {
        int32_t left = 594;
        int32_t top = 157;
        int32_t right = left + 355;
        int32_t bottom = top + 200;
    }region;
    HdrImage writeImage;
    writeImage.assign(img);
    // 一様サンプルで出す
    {
        FloatStreamStats fsUnif, fsIS;
        std::mt19937 rng(0x123);
        std::uniform_real_distribution<float> dist(0, 1.0f);
        for(int32_t sn=0;sn<1024 * 32;++sn)
        {
            const float u = dist(rng);
            const float v = dist(rng);
            {
                // 一様にサンプルする場合
                const int32_t x = int32_t(u * float(img.width()));
                const int32_t y = int32_t(v * float(img.height()));
                fsUnif.add(img.value(x,y).Y());
            }
            {
                // 重点サンプルする場合
                auto nuv = sat.sample(u, v);
                const float nu = std::get<0>(nuv);
                const float nv = std::get<1>(nuv);
                const float pdf = std::get<2>(nuv);
                //
                const int32_t x = int32_t(nu * float(img.width()));
                const int32_t y = int32_t(nv * float(img.height()));
                if (pdf == 0.0)
                {
                    puts("FUCK");
                }
                if (pdf != 0.0f)
                {
                    fsIS.add(img.value(x, y).Y() / pdf);
                }
                
                if ((region.left < x) && (x < region.right) &&
                    (region.top < y) && (y < region.bottom))
                {
                    writeImage.setValue(x, y, { 1.0f,0.0f,0.0f });
                }
            }
            //
            if(!((sn+1)%(1024)))
            {
                printf("%f %f\n",
                       std::fabsf(fsUnif.mu() - trueValue),
                       std::fabsf(fsIS.mu() - trueValue)
                       );
            }
        }
    }
    //
    for (int32_t y = 0; y < img.height(); ++y)
    {
        for (int32_t x = 0; x < img.width(); ++x)
        {
            if ((region.left < x) && (x < region.right) &&
                (region.top < y) && (y < region.bottom))
            {
            }
            else
            {
                auto p = writeImage.value(x, y);
                p.r *= 0.1f;
                p.g *= 0.1f;
                p.b *= 0.1f;
                writeImage.setValue(x, y, p);
            }
        }
    }
    //
    writeImage.write("./out.hdr");
}

//
void test()
{
    //
    // 元の関数
    const auto gen = [](int32_t x, int32_t y) -> float
    {
        return 1.0f;
        return float(x + y);
    };
    //
    SummedAreaTable<float> sat;
    sat.construct(4, 4, gen);
    //
    {
        //printf("%f (1)\n", sat.sum(0, 0, 1, 1));
        //printf("%f (16)\n", sat.sum(0, 0, 4, 4));
        //printf("%f (9)\n", sat.sum(1, 1, 3, 3));
    }

#if 0
    // 特定の場所の値があっているかをチェック
    for (int32_t y = 0; y<sat.height(); ++y)
    {
        for (int32_t x = 0; x<sat.width(); ++x)
        {
            const float v0 = sat.value(x, y);
            const float v1 = gen(x, y);
            if (v0 != v1)
            {
                printf("%f,%f\n", v0, v1);
            }
        }
    }
#endif

#if 1
    // 一様な場合にUVがちゃんと一致しているか
    for (int32_t y = 0; y<sat.height(); ++y)
    {
        for (int32_t x = 0; x<sat.width(); ++x)
        {
            const float u = (float(x) + 0.5f) / float(sat.width());
            const float v = (float(y) + 0.5f) / float(sat.height());
            auto ret = sat.sample(u, v);
            printf("%f-%f, %f-%f, %f\n", u, std::get<0>(ret), v, std::get<1>(ret), std::get<2>(ret));
        }
    }
#endif
}

//
int32_t main()
{
#if 0
    samplingMain();
#else
    test();
#endif
    return 0;
    
}

















