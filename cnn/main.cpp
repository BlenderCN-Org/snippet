
/*
 TODOs
- イメージのローダを作成する(stb image経由でいいかな)
- 最適化の部分を作成する
 */
#include <cstdio>
#include <cstdint>
#include <cassert>
#include <array>
#include <vector>
#include <string>
#include <utility>

//
template<int32_t width_, int32_t height_>
class Image
{
public:
    void load(const std::string& fileName)
    {
        // TODO: ファイルを開く
    }
    int32_t width() const
    {
        return width_;
    }
    int32_t height() const
    {
        return height_;
    }
    //
    float get(int32_t x, int32_t y) const
    {
        assert((0<= x) && (x<width_));
        assert((0<= y) && (x<height_));
        return pixels_[y][x];
    }
    //
private:
    std::array<std::array<int32_t,width_>, height_> pixels_;
};
const int32_t imageWidth = 9;
const int32_t imageHeight = 9;
const int32_t filterSize = 3;
const int32_t numHiddenLayer = 3;
// 画像
typedef Image<imageWidth,imageHeight> CNNImage;

// 係数
struct CNNFactors
{
    // 隠れ層のフィルター。3x3で3枚
    std::array<Image<filterSize,filterSize>,numHiddenLayer> filters;
    //float
};

// 係数
struct CNNResults
{
    
    float maru;
    float batsu;
};

/*
 */
CNNResults cnn(const CNNImage& image, const CNNFactors& factor)
{
    // TODO: 4x4で見ていく
    // TODO: 隠れ層
    for(int32_t tx=0;tx<3;++tx)
    {
        for(int32_t ty=0;ty<3;++ty)
        {
            
        }
    }
    // TODO: Maxプーリング
    // TODO: 出力層
    CNNResults ret;
    return ret;
}

CNNImage loadImage(const std::string& fileName)
{
    int32_t width = 0;
    int32_t height = 0;
    int32_t bpp = 0;
    unsigned char* rgb = stbi_load( fileName.c_str(), &width, &height, &bpp, 3 );
    assert(rgb != nullptr);
    assert(bpp == 3);
    stbi_image_free( rgb );
}

//
int32_t main()
{
    // ○と×の画像を読み込む
    std::vector<std::pair<CNNImage,int32_t>> images;
    for(int32_t i=0;i<10;++i)
    {
        CNNImage img;
        img.load(std::string("./maru/") + std::to_string(i) + std::string(".png"));
        images.push_back(std::make_pair<>(img,0));
    }
    for(int32_t i=0;i<10;++i)
    {
        CNNImage img;
        img.load(std::string("./batsu/") + std::to_string(i) + std::string(".png"));
        images.push_back(std::make_pair<>(img,1));
    }
    
#if 1 // 学習をする場合
    // 学習
    CNNFactors factors;
    for(int32_t ite=0;ite<100;++ite)
    {
        for(auto& imgData : images)
        {
            auto& img = imgData.first;
            auto& tag = imgData.second;
            auto result = cnn(img,factors);
            // TODO: 誤差の評価
        }
    }
#else // 予測をする場合
    CNNFactors factors;
    for(auto& imgData : images)
    {
        auto& img = imgData.first;
        auto& tag = imgData.second;
        auto result = cnn(img,factors);
    }
#endif
}
