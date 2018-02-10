
/*
-
*/
#include <cstdio>
#include <cstdint>
#include <cassert>
#include <array>
#include <vector>
#include <string>
#include <cmath>
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

// シグモイド関数
float sig(float x)
{
    return 1.0f/(std::expf(x)+1.0f);
}

//
template<int32_t width_, int32_t height_>
class Image
{
public:
    Image()
    {
        pixels_.fill(0.0f);
    }
    Image(const std::array<float,width_*height_>& l)
    {
        for(int32_t i=0;i<width_*height_;++i)
        {
            pixels_[i] = l[i];
        }
    }
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
                const uint8_t p = rgb[x+width_*y];
                // 白黒を反転した方が扱いやすいので反転
                get(x,y) = 1.0f - float(p)/255.0f;
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
    int32_t numPixel() const
    {
        return width() * height();
    }
    //
    float& get(int32_t x, int32_t y)
    {
        assert((0<= x) && (x<width_));
        assert((0<= y) && (x<height_));
        return pixels_[x + y * width_];
    }
    //
    const float& get(int32_t x, int32_t y) const
    {
        assert((0<= x) && (x<width_));
        assert((0<= y) && (x<height_));
        return pixels_[x + y * width_];
    }
    //
    float& get(int32_t i)
    {
        assert((0<= i) && (i<pixels_.size()));
        return pixels_[i];
    }
    //
    const float& get(int32_t i) const
    {
        assert((0<= i) && (i<pixels_.size()));
        return pixels_[i];
    }
    //
    void normalize()
    {
        float total = 0.0f;
        for(const auto& p : pixels_)
        {
            total += p;
        }
        if(total == 0.0f)
        {
            return;
        }
        const float it = 1.0f / total;
        for(auto& p : pixels_)
        {
            p *= it;
        }
    }
    //
    void print()
    {
        for(int32_t y=0;y<height_;++y)
        {
            for(int32_t x=0;x<width_;++x)
            {
                printf("%03ff, ", pixels_[x + y * width_]);
            }
            puts("");
        }
    }
private:
    std::array<float,width_*height_> pixels_;
};
// 画像設定
const int32_t imageSize = 28;
const int32_t filterSize = 3;
const int32_t convolutionSize =  imageSize - filterSize + 1;
const int32_t poolingSize = convolutionSize / 2;
const int32_t numHiddenLayer = 3;
static_assert(convolutionSize % 2 == 0, "");
// 画像
typedef Image<imageSize,imageSize> CNNImage;
typedef Image<filterSize,filterSize> FilterImage;
typedef Image<convolutionSize,convolutionSize> ConvolevdImage;
typedef Image<poolingSize,poolingSize> PoolingImage;

// 係数
struct CNNFactors
{
    // 隠れ層のフィルター。3x3で3枚
    std::array<FilterImage,numHiddenLayer> filters;
    //
    std::array<PoolingImage, numHiddenLayer> w1;
    std::array<PoolingImage, numHiddenLayer> w2;
    
    void init()
    {
        // Filter
        filters[0] = FilterImage({
            0.015000f, 0.085000f, 0.000000f,
            0.000000f, 0.125000f, 0.000000f,
            0.060000f, 0.040000f, 0.000000f,
        });
        filters[1] = FilterImage({
            0.000000f, 0.020000f, 0.000000f,
            0.195000f, 0.200000f, 0.000000f,
            0.080000f, 0.100000f, 0.000000f,
        });
        filters[2] = FilterImage({
            0.000000f, 0.000000f, 0.380000f,
            0.100000f, 0.005000f, 0.000000f,
            0.060000f, 0.105000f, 0.080000f,
        });
        // w1
        w1[0] = PoolingImage({
            0.000000f, 0.000000f, 0.120000f, 0.315000f, 0.000000f, 0.075000f, 0.000000f, 0.000000f, 0.075000f, 0.080000f, 0.075000f, 0.000000f, 0.000000f,
            0.275000f, 0.020000f, 0.195000f, 0.000000f, 0.220000f, 0.525000f, 0.200000f, 0.080000f, 0.215000f, 0.200000f, 0.000000f, 0.000000f, 0.000000f,
            0.100000f, 0.290000f, 0.515000f, 0.105000f, 0.200000f, 0.530000f, 0.200000f, 0.000000f, 0.100000f, 0.295000f, 0.000000f, 0.000000f, 0.120000f,
            0.035000f, 0.095000f, 0.000000f, 0.105000f, 0.750000f, 0.285000f, 0.200000f, 0.000000f, 0.200000f, 0.350000f, 0.095000f, 0.000000f, 0.000000f,
            0.000000f, 0.000000f, 0.000000f, 0.195000f, 0.120000f, 0.100000f, 0.000000f, 0.000000f, 0.420000f, 0.100000f, 0.220000f, 0.100000f, 0.005000f,
            0.000000f, 0.090000f, 0.000000f, 0.050000f, 0.315000f, 0.000000f, 0.000000f, 0.000000f, 0.100000f, 0.100000f, 0.195000f, 0.000000f, 0.215000f,
            0.100000f, 0.000000f, 0.000000f, 0.300000f, 0.020000f, 0.100000f, 0.015000f, 0.000000f, 0.200000f, 0.220000f, 0.000000f, 0.000000f, 0.000000f,
            0.280000f, 0.300000f, 0.015000f, 0.005000f, 0.220000f, 0.090000f, 0.100000f, 0.180000f, 0.000000f, 0.970000f, 0.060000f, 0.055000f, 0.080000f,
            0.100000f, 0.075000f, 0.300000f, 0.420000f, 0.420000f, 0.105000f, 0.195000f, 0.020000f, 1.085000f, 0.080000f, 0.000000f, 0.080000f, 0.190000f,
            0.000000f, 0.180000f, 0.545000f, 0.420000f, 0.020000f, 0.220000f, 0.020000f, 0.105000f, 0.100000f, 0.445000f, 0.205000f, 0.000000f, 0.000000f,
            0.000000f, 0.240000f, 0.505000f, 0.720000f, 0.220000f, 0.000000f, 0.090000f, 0.000000f, 0.025000f, 0.525000f, 0.000000f, 0.020000f, 0.100000f,
            0.200000f, 0.180000f, 0.000000f, 0.000000f, 0.080000f, 0.000000f, 0.000000f, 0.000000f, 0.000000f, 0.195000f, 0.020000f, 0.100000f, 0.075000f,
            0.520000f, 0.000000f, 0.000000f, 0.080000f, 0.000000f, 0.000000f, 0.100000f, 0.000000f, 0.000000f, 0.000000f, 0.100000f, 0.100000f, 0.000000f,
        });
        w1[1] = PoolingImage({
            0.075000f, 0.000000f, 0.095000f, 0.000000f, 0.000000f, 0.220000f, 0.100000f, 0.075000f, 0.000000f, 0.000000f, 0.075000f, 0.000000f, 0.020000f,
            0.000000f, 0.055000f, 0.300000f, 0.000000f, 0.200000f, 0.440000f, 0.085000f, 0.005000f, 0.200000f, 0.080000f, 0.000000f, 0.095000f, 0.100000f,
            0.000000f, 0.000000f, 0.020000f, 0.300000f, 0.100000f, 0.180000f, 0.100000f, 0.105000f, 0.020000f, 0.000000f, 0.000000f, 0.020000f, 0.000000f,
            0.000000f, 0.000000f, 0.000000f, 0.105000f, 0.200000f, 0.060000f, 0.200000f, 0.020000f, 0.300000f, 0.100000f, 0.000000f, 0.000000f, 0.100000f,
            0.000000f, 0.200000f, 0.000000f, 0.640000f, 0.505000f, 0.085000f, 0.000000f, 0.000000f, 0.080000f, 0.420000f, 0.060000f, 0.000000f, 0.000000f,
            0.020000f, 0.000000f, 0.100000f, 0.195000f, 0.000000f, 0.100000f, 0.000000f, 0.000000f, 0.105000f, 0.005000f, 0.100000f, 0.000000f, 0.175000f,
            0.055000f, 0.100000f, 0.200000f, 0.090000f, 0.320000f, 0.000000f, 0.000000f, 0.000000f, 0.310000f, 0.105000f, 0.100000f, 0.000000f, 0.000000f,
            0.055000f, 0.095000f, 0.100000f, 0.425000f, 0.005000f, 0.020000f, 0.000000f, 0.105000f, 0.445000f, 0.300000f, 0.225000f, 0.095000f, 0.000000f,
            0.000000f, 0.075000f, 0.080000f, 0.320000f, 0.520000f, 0.020000f, 0.320000f, 0.020000f, 0.320000f, 0.205000f, 0.410000f, 0.215000f, 0.190000f,
            0.000000f, 0.000000f, 0.100000f, 0.100000f, 0.105000f, 0.440000f, 0.000000f, 0.000000f, 0.320000f, 0.725000f, 0.400000f, 0.405000f, 0.100000f,
            0.080000f, 0.000000f, 0.125000f, 0.080000f, 0.230000f, 0.395000f, 0.000000f, 0.095000f, 0.160000f, 0.420000f, 0.220000f, 0.080000f, 0.000000f,
            0.015000f, 0.000000f, 0.000000f, 0.000000f, 0.000000f, 0.000000f, 0.000000f, 0.095000f, 0.220000f, 0.275000f, 0.200000f, 0.090000f, 0.000000f,
            0.200000f, 0.000000f, 0.000000f, 0.100000f, 0.080000f, 0.000000f, 0.000000f, 0.020000f, 0.000000f, 0.095000f, 0.205000f, 0.115000f, 0.000000f,
        });
        w1[2] = PoolingImage({
            0.000000f, 0.120000f, 0.720000f, 0.000000f, 0.275000f, 0.020000f, 0.000000f, 0.100000f, 0.100000f, 0.095000f, 0.180000f, 0.075000f, 0.100000f,
            0.120000f, 0.100000f, 0.000000f, 0.000000f, 0.100000f, 0.100000f, 0.200000f, 0.220000f, 0.100000f, 0.000000f, 0.000000f, 0.080000f, 0.100000f,
            0.075000f, 0.000000f, 0.100000f, 0.005000f, 0.330000f, 0.125000f, 0.100000f, 0.745000f, 0.120000f, 0.070000f, 0.000000f, 0.080000f, 0.000000f,
            0.000000f, 0.000000f, 0.075000f, 0.320000f, 0.300000f, 0.200000f, 0.500000f, 0.000000f, 0.000000f, 0.080000f, 0.100000f, 0.000000f, 0.010000f,
            0.015000f, 0.000000f, 0.000000f, 0.015000f, 0.105000f, 0.020000f, 0.000000f, 0.000000f, 0.310000f, 0.200000f, 0.095000f, 0.000000f, 0.020000f,
            0.075000f, 0.000000f, 0.100000f, 0.000000f, 0.075000f, 0.010000f, 0.000000f, 0.095000f, 0.220000f, 0.530000f, 0.600000f, 0.095000f, 0.000000f,
            0.200000f, 0.020000f, 0.095000f, 0.000000f, 0.115000f, 0.120000f, 0.000000f, 0.080000f, 0.620000f, 0.400000f, 0.100000f, 0.000000f, 0.100000f,
            0.000000f, 0.075000f, 0.075000f, 0.105000f, 0.225000f, 0.325000f, 0.220000f, 0.200000f, 0.300000f, 0.120000f, 0.325000f, 0.000000f, 0.075000f,
            0.280000f, 0.020000f, 0.345000f, 0.320000f, 0.120000f, 0.000000f, 0.100000f, 0.030000f, 0.545000f, 0.100000f, 0.200000f, 0.320000f, 0.000000f,
            0.010000f, 0.000000f, 0.120000f, 0.660000f, 0.330000f, 0.100000f, 0.120000f, 0.100000f, 0.740000f, 0.100000f, 0.660000f, 0.120000f, 0.010000f,
            0.000000f, 0.070000f, 0.395000f, 0.245000f, 0.125000f, 0.090000f, 0.115000f, 0.000000f, 0.100000f, 0.320000f, 0.100000f, 0.000000f, 0.100000f,
            0.000000f, 0.000000f, 0.000000f, 0.315000f, 0.000000f, 0.000000f, 0.000000f, 0.000000f, 0.000000f, 0.000000f, 0.000000f, 0.295000f, 0.080000f,
            0.180000f, 0.040000f, 0.100000f, 0.300000f, 0.095000f, 0.000000f, 0.005000f, 0.000000f, 0.000000f, 0.095000f, 0.000000f, 0.095000f, 0.400000f,
        });
        // w2
        w2[0] = PoolingImage({
            0.000000f, 0.295000f, 0.295000f, 0.515000f, 0.000000f, 0.300000f, 0.195000f, 0.000000f, 0.000000f, 0.020000f, 0.200000f, 0.095000f, 0.095000f,
            0.000000f, 0.100000f, 0.320000f, 0.075000f, 0.000000f, 0.000000f, 0.020000f, 0.000000f, 0.100000f, 0.220000f, 0.240000f, 0.210000f, 0.000000f,
            0.100000f, 0.010000f, 0.075000f, 0.000000f, 0.075000f, 0.000000f, 0.000000f, 0.180000f, 0.000000f, 0.080000f, 0.075000f, 0.200000f, 0.200000f,
            0.320000f, 0.415000f, 0.200000f, 0.000000f, 0.000000f, 0.000000f, 0.000000f, 0.100000f, 0.000000f, 0.000000f, 0.095000f, 0.910000f, 0.200000f,
            0.000000f, 0.460000f, 0.020000f, 0.000000f, 0.000000f, 0.000000f, 0.190000f, 0.125000f, 0.100000f, 0.000000f, 0.195000f, 0.100000f, 0.130000f,
            0.300000f, 0.120000f, 0.095000f, 0.020000f, 0.000000f, 0.755000f, 0.425000f, 0.225000f, 0.000000f, 0.000000f, 0.095000f, 0.410000f, 0.195000f,
            0.000000f, 0.300000f, 0.075000f, 0.000000f, 0.000000f, 0.450000f, 0.220000f, 0.105000f, 0.000000f, 0.000000f, 0.000000f, 0.080000f, 0.000000f,
            0.020000f, 0.015000f, 0.050000f, 0.000000f, 0.000000f, 0.000000f, 0.100000f, 0.095000f, 0.000000f, 0.100000f, 0.075000f, 0.000000f, 0.200000f,
            0.220000f, 0.095000f, 0.000000f, 0.000000f, 0.000000f, 0.080000f, 0.080000f, 0.100000f, 0.000000f, 0.000000f, 0.100000f, 0.000000f, 0.000000f,
            0.100000f, 0.200000f, 0.000000f, 0.000000f, 0.000000f, 0.000000f, 0.000000f, 0.000000f, 0.100000f, 0.000000f, 0.000000f, 0.220000f, 0.100000f,
            0.000000f, 0.000000f, 0.000000f, 0.000000f, 0.095000f, 0.100000f, 0.100000f, 0.080000f, 0.095000f, 0.000000f, 0.100000f, 0.095000f, 0.300000f,
            0.100000f, 0.000000f, 0.200000f, 0.045000f, 0.080000f, 0.100000f, 0.205000f, 0.080000f, 0.000000f, 0.000000f, 0.315000f, 0.000000f, 0.080000f,
            0.320000f, 0.065000f, 0.100000f, 0.100000f, 0.300000f, 0.040000f, 0.545000f, 0.100000f, 0.000000f, 0.615000f, 0.215000f, 0.000000f, 0.220000f,
        });
        w2[1] = PoolingImage({
            0.020000f, 0.000000f, 0.300000f, 0.200000f, 0.000000f, 0.280000f, 0.295000f, 0.100000f, 0.000000f, 0.080000f, 0.180000f, 0.300000f, 0.000000f,
            0.100000f, 0.095000f, 0.000000f, 0.070000f, 0.000000f, 0.095000f, 0.100000f, 0.000000f, 0.015000f, 0.000000f, 0.000000f, 0.090000f, 0.075000f,
            0.000000f, 0.095000f, 0.000000f, 0.000000f, 0.020000f, 0.100000f, 0.000000f, 0.000000f, 0.220000f, 0.000000f, 0.000000f, 0.195000f, 0.195000f,
            0.300000f, 0.075000f, 0.100000f, 0.000000f, 0.000000f, 0.200000f, 0.200000f, 0.095000f, 0.000000f, 0.115000f, 0.095000f, 0.095000f, 0.050000f,
            0.000000f, 0.020000f, 0.015000f, 0.300000f, 0.000000f, 0.000000f, 0.425000f, 0.100000f, 0.000000f, 0.195000f, 0.060000f, 0.080000f, 0.020000f,
            0.020000f, 0.100000f, 0.100000f, 0.270000f, 0.120000f, 0.125000f, 0.320000f, 0.520000f, 0.080000f, 0.000000f, 0.000000f, 0.175000f, 0.095000f,
            0.195000f, 0.000000f, 0.000000f, 0.015000f, 0.000000f, 0.305000f, 0.100000f, 0.100000f, 0.000000f, 0.000000f, 0.395000f, 0.000000f, 0.400000f,
            0.095000f, 0.080000f, 0.020000f, 0.000000f, 0.000000f, 0.080000f, 0.100000f, 0.000000f, 0.000000f, 0.000000f, 0.000000f, 0.000000f, 0.515000f,
            0.100000f, 0.000000f, 0.100000f, 0.000000f, 0.000000f, 0.095000f, 0.205000f, 0.000000f, 0.000000f, 0.000000f, 0.000000f, 0.390000f, 0.000000f,
            0.000000f, 0.290000f, 0.000000f, 0.000000f, 0.000000f, 0.175000f, 0.045000f, 0.000000f, 0.000000f, 0.000000f, 0.000000f, 0.000000f, 0.075000f,
            0.320000f, 0.300000f, 0.000000f, 0.000000f, 0.000000f, 0.000000f, 0.100000f, 0.080000f, 0.100000f, 0.000000f, 0.000000f, 0.300000f, 0.000000f,
            0.520000f, 0.040000f, 0.095000f, 0.000000f, 0.195000f, 0.200000f, 0.200000f, 0.100000f, 0.000000f, 0.015000f, 0.300000f, 0.100000f, 0.520000f,
            0.295000f, 0.020000f, 0.020000f, 0.115000f, 0.095000f, 0.100000f, 0.320000f, 0.025000f, 0.000000f, 0.100000f, 0.000000f, 0.120000f, 0.515000f,
        });
        w2[2] = PoolingImage({
            0.090000f, 0.060000f, 0.195000f, 0.070000f, 0.400000f, 0.415000f, 0.015000f, 0.000000f, 0.025000f, 0.000000f, 0.275000f, 0.295000f, 0.000000f,
            0.080000f, 0.300000f, 0.020000f, 0.080000f, 0.000000f, 0.000000f, 0.080000f, 0.095000f, 0.100000f, 0.000000f, 0.320000f, 0.400000f, 0.215000f,
            0.120000f, 0.420000f, 0.055000f, 0.000000f, 0.000000f, 0.000000f, 0.000000f, 0.200000f, 0.000000f, 0.070000f, 0.000000f, 0.000000f, 0.295000f,
            0.200000f, 0.015000f, 0.000000f, 0.000000f, 0.000000f, 0.100000f, 0.120000f, 0.075000f, 0.000000f, 0.000000f, 0.220000f, 0.100000f, 0.100000f,
            0.280000f, 0.200000f, 0.075000f, 0.000000f, 0.075000f, 0.000000f, 0.305000f, 0.100000f, 0.080000f, 0.080000f, 0.100000f, 0.000000f, 0.060000f,
            0.080000f, 0.000000f, 0.300000f, 0.300000f, 0.100000f, 0.100000f, 0.125000f, 0.220000f, 0.100000f, 0.100000f, 0.080000f, 0.100000f, 0.200000f,
            0.000000f, 0.100000f, 0.100000f, 0.000000f, 0.100000f, 0.085000f, 0.760000f, 0.100000f, 0.000000f, 0.000000f, 0.395000f, 0.000000f, 0.120000f,
            0.040000f, 0.000000f, 0.000000f, 0.000000f, 0.000000f, 0.215000f, 0.200000f, 0.095000f, 0.100000f, 0.000000f, 0.000000f, 0.020000f, 0.055000f,
            0.300000f, 0.000000f, 0.000000f, 0.000000f, 0.000000f, 0.000000f, 0.005000f, 0.080000f, 0.000000f, 0.000000f, 0.000000f, 0.075000f, 0.000000f,
            0.420000f, 0.000000f, 0.000000f, 0.000000f, 0.000000f, 0.000000f, 0.195000f, 0.480000f, 0.000000f, 0.000000f, 0.200000f, 0.055000f, 0.055000f,
            0.100000f, 0.320000f, 0.000000f, 0.000000f, 0.000000f, 0.100000f, 0.205000f, 0.295000f, 0.000000f, 0.000000f, 0.015000f, 0.180000f, 0.000000f,
            0.180000f, 0.060000f, 0.075000f, 0.000000f, 0.020000f, 0.120000f, 0.020000f, 0.425000f, 0.100000f, 0.375000f, 0.000000f, 0.110000f, 0.100000f,
            0.005000f, 0.385000f, 0.000000f, 0.000000f, 0.200000f, 0.590000f, 0.060000f, 0.020000f, 0.080000f, 0.100000f, 0.400000f, 0.000000f, 0.000000f,
        });
    }
    //
    void print()
    {
        printf("// Filter\n");
        
        for(int32_t i=0;i<filters.size();++i)
        {
            auto& f = filters[i];
            printf("filters[%d] = FilterImage({\n", i);
            f.print();
            printf("});\n");
        }
        printf("// w1\n");
        for(int32_t i=0;i<w1.size();++i)
        {
            printf("w1[%d] = PoolingImage({\n", i);
            w1[i].print();
            printf("});\n");
        }
        printf("// w2\n");
        for(int32_t i=0;i<w1.size();++i)
        {
            printf("w2[%d] = PoolingImage({\n", i);
            w2[i].print();
            printf("});\n");
        }
    }
    //
    void mutate(float QtNow)
    {
        // 選択する隠れ層
        const int32_t hi = rand() % 3 ;
        // 選択するイメージ位置
        const int32_t f0 = filterSize * filterSize;
        const int32_t f12 = poolingSize * poolingSize;
        const int32_t ff = rand() % (f0 + f12 + f12);
        //
        const auto tryMutate = [](float& v, float m)
        {
            const float sign = (rand()%2 == 0) ?  1.0f : -1.0f;
            m *= sign;
            v += m;
            if(v < 0.0f)
            {
                v -= 2.0f * m;
            }
        };
        const float mBase = 0.001f;
        // filter
        if(ff < f0)
        {
            auto& img = filters[hi];
            tryMutate(img.get(ff), mBase);
            //img.normalize();
        }
        // w1
        else if(ff < f0 + f12)
        {
            auto& img = w1[hi];
            const int32_t idx = ff - f0;
            tryMutate(img.get(idx), mBase);
            //
            //img.normalize();
        }
        // w2
        else if(ff < f0 + f12 + f12)
        {
            auto& img = w2[hi];
            const int32_t idx = ff - f0 - f12;
            tryMutate(img.get(idx), mBase);
            //
            //img.normalize();
        }
        else
        {
            assert(false);
        }
    }
};

// 係数
struct CNNResults
{
    // 1さ加減
    float t1 = 0.0f;
    // 2さ加減
    float t2 = 0.0f;
};

/*
 */
CNNResults cnn(const CNNImage& image, const CNNFactors& factor)
{
    // 隠れ層
    std::array<ConvolevdImage, numHiddenLayer> convolevdImages;
    for(int32_t hi=0;hi<numHiddenLayer;++hi)
    {
        auto& filter = factor.filters[hi];
        auto& convolevdImage = convolevdImages[hi];
        //
        for(int32_t y=0;y<convolevdImage.height();++y)
        {
            for(int32_t x=0;x<convolevdImage.width();++x)
            {
                // フィルター処理
                float& convolved = convolevdImage.get(x,y);
                convolved = 0.0f;
                for(int32_t fx=0;fx<filterSize;++fx)
                {
                    for(int32_t fy=0;fy<filterSize;++fy)
                    {
                        const float xx = fx + x;
                        const float yy = fx + y;
                        const float f = filter.get(fx,fy);
                        const float p = image.get(xx,yy);
                        convolved += sig(f * p);
                    }
                }
                convolved *= 1.0f/float(filterSize*filterSize);
                // 閾値を引く
                convolved -= 0.5f;
                //assert(convolved <= 1.0f);
            }
        }
    }
    // Maxプーリング
    CNNResults ret;
    for(int32_t hi=0;hi<numHiddenLayer;++hi)
    {
        //
        auto& convolevdImage = convolevdImages[hi];
        //
        for(int32_t hi=0;hi<numHiddenLayer;++hi)
        {
            auto& w1i = factor.w1[hi];
            auto& w2i = factor.w2[hi];
            //
            for(int32_t y=0;y<poolingSize;++y)
            {
                for(int32_t x=0;x<poolingSize;++x)
                {
                    const float c0 = convolevdImage.get(x*2+0,y*2+0);
                    const float c1 = convolevdImage.get(x*2+1,y*2+0);
                    const float c2 = convolevdImage.get(x*2+0,y*2+1);
                    const float c3 = convolevdImage.get(x*2+1,y*2+1);
                    const float p = std::max({c0,c1,c2,c3});
                    ret.t1 += sig(w1i.get(x,y) * p) -0.5f;
                    ret.t2 += sig(w2i.get(x,y) * p) -0.5f;
                }
            }
        }
    }
    return ret;
}

//
int32_t main()
{
    //const std::string imageRoot = "../../../../mnist_png/training/";
    const std::string imageRoot = "../../../../mnist_png/testing/";
    // ○と×の画像を読み込む
    printf("start load\n");
    std::vector<std::pair<CNNImage,int32_t>> images;
    for(int32_t i=1;i<=2;++i)
    {
        const std::string path = imageRoot +
        std::to_string(i) +
        std::string("/");
        auto imagelist = getFileList(path);
        for(int32_t imageNo=0;imageNo<1000;++imageNo)
        {
            CNNImage img;
            img.load(imagelist[imageNo]);
            //img.print();
            images.push_back(std::make_pair<>(img,i));
        }
    }
    
#if 0 // 学習をする場合
    printf("start traing\n");
    // 学習
    CNNFactors factors;
    factors.init();
    //
    float Qbest = std::numeric_limits<float>::max();
    const int32_t numIte = 10000;
    for(int32_t ite=0;ite<numIte;++ite)
    {
        CNNFactors factorsTmp = factors;
        for(int32_t nm=0;nm<1;++nm)
        {
            factorsTmp.mutate(Qbest);
        }
        //
        int okayCount = 0;
        float Qt = 0.0f;
        for(auto& imgData : images)
        {
            auto& img = imgData.first;
            auto& no = imgData.second;
            auto result = cnn(img,factorsTmp);
            const bool maybe1 = result.t1 > result.t2;
            const bool correct = ((no == 1) && maybe1) || ((no == 2) && !maybe1);
            okayCount += correct ? 1 : 0;
            // 誤差の評価
            float Q = 0.0f;
            if(no == 1)
            {
                const float d1 = (result.t1 - 1.0f);
                const float d2 = (result.t2 - 0.0f);
                Q = d1*d1 + d2*d2;
            }
            else if(no == 2)
            {
                const float d1 = (result.t1 - 0.0f);
                const float d2 = (result.t2 - 1.0f);
                Q = d1*d1 + d2*d2;
            }
            else
            {
                assert(false);
            }
            Qt += Q;
        }
        // イメージ数で正規化
        Qt /= float(images.size());
        //
        if(Qt < Qbest)
        {
            factors = factorsTmp;
            Qbest = Qt;
            //printf("QtBest: %f\n", Qbest);
        }
        
        if(ite*100/numIte != (ite-1)*100/numIte)
        {
            printf("[%d%%] QtBest: %f (%d/%lu)\n", ite*100/numIte, Qbest, okayCount, images.size());
        }
    }
    //
    printf("QtBest: %f\n", Qbest);
    factors.print();
    
#else // 予測をする場合
    CNNFactors factors;
    factors.init();
    int okayCount = 0;
    for(auto& imgData : images)
    {
        auto& img = imgData.first;
        auto& no = imgData.second;
        auto result = cnn(img,factors);
        const bool maybe1 = result.t1 > result.t2;
        const bool correct = ((no == 1) && maybe1) || ((no == 2) &&!maybe1);
        okayCount += correct ? 1 : 0;
    }
    printf("%d/%lu\n", okayCount, images.size());
#endif
}
