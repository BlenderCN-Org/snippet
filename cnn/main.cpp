
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
#include <dirent.h>
//
#define STB_IMAGE_IMPLEMENTATION
#include "../thirdparty/stb/stb_image.h"

static std::vector<std::string> getFileList(const std::string& folderPath)
{
    //
    std::vector<std::string> fileList;
    DIR* dir = nullptr;
    struct dirent* ent = nullptr;
    if ((dir = opendir (folderPath.c_str())) != nullptr)
    {
        while ((ent = readdir (dir)) != nullptr)
        {
            const bool isFolder = (ent->d_type == DT_DIR);
            if(isFolder)
            {
                const bool invalidPath =
                (strcmp(ent->d_name, ".") == 0) ||
                (strcmp(ent->d_name, "..") == 0);
                if (!invalidPath)
                {
                    const std::string subFolderPath = folderPath + std::string(ent->d_name) + "/";
                    auto subFolderList = getFileList(subFolderPath);
                    fileList.insert(fileList.end(), subFolderList.begin(), subFolderList.end());
                }
            }
            // ファイルの場合のみ見る
            else if(ent->d_type == DT_REG)
            {
                const bool invalidFile =
                (strcmp(ent->d_name, ".DS_Store") == 0);
                if(!invalidFile)
                {
                    fileList.push_back(folderPath + std::string(ent->d_name));
                }
            }
        }
        closedir (dir);
    }
    return fileList;
}

//
template<int32_t width_, int32_t height_>
class Image
{
public:
    void load(const std::string& fileName)
    {
        int32_t width = 0;
        int32_t height = 0;
        int32_t bpp = 0;
        unsigned char* rgb = stbi_load( fileName.c_str(), &width, &height, &bpp, 1 );
        assert(rgb != nullptr);
        assert(bpp == 1);
        assert(width == width_);
        assert(height == height_);
        for(int32_t y=0;y<height_;++y)
        {
            for(int32_t x=0;x<width_;++x)
            {
                pixels_[y][x] = rgb[x+width_*y];
            }
        }
        //
        stbi_image_free( rgb );
        
        // TODO: スケールして行く
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
    void print()
    {
        for(int32_t y=0;y<height_;++y)
        {
            for(int32_t x=0;x<width_;++x)
            {
                printf("%03d ", pixels_[y][x]);
            }
            puts("");
        }
    }
private:
    std::array<std::array<int32_t,width_>, height_> pixels_;
};
const int32_t imageWidth = 28;
const int32_t imageHeight = 28;
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

//
int32_t main()
{
    system("pwd");
    const std::string traingImageRoot = "../../../../mnist_png/training/";
    // ○と×の画像を読み込む
    std::vector<std::pair<CNNImage,int32_t>> images;
    for(int32_t i=1;i<=2;++i)
    {
        const std::string path = traingImageRoot +
                                std::to_string(i) +
                                std::string("/");
        auto imagelist = getFileList(path);
        for(int32_t imageNo=0;imageNo<10;++imageNo)
        {
            CNNImage img;
            img.load(imagelist[imageNo]);
            images.push_back(std::make_pair<>(img,i-1));
        }
    }
    
    ここまで完了
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
