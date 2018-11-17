#pragma once

//
#include <vector>
#include <functional>

//
template<typename Float = float>
class SummedAreaTable
{
    static_assert(std::is_floating_point<Float>::value);

public:
    SummedAreaTable() = default;
    ~SummedAreaTable() = default;

    //
    int32_t width() const
    {
        return width_;
    }

    //
    int32_t height() const
    {
        return height_;
    }
    // ラムダ経由で構築
    void construct(int32_t width, int32_t height,
        const std::function<Float(int32_t x, int32_t y)>& gen)
    {
        width_ = width;
        height_ = height;
        //
        table_.resize(width*height);
        //
        for (int32_t y = 0; y<height; ++y)
        {
            for (int32_t x = 0; x<width; ++x)
            {
                // 左
                const Float lValue = (x == 0) ? 0.0f : table_[(x - 1) + (y + 0) * width_];
                // 上
                const Float uValue = (y == 0) ? 0.0f : table_[(x - 0) + (y - 1) * width_];
                // 左上
                const Float luValue = ((y == 0) || (x == 0)) ? 0.0f : table_[(x - 1) + (y - 1) * width_];
                //
                table_[x + y * width_] = gen(x, y) + lValue + uValue - luValue;
            }
        }
    }

    // 特定の位置の値
    Float value(int32_t x, int32_t y) const
    {
        return sum(x, y, 1, 1);
    }

    // 特定のサブ領域の和を返す
    Float sum(int32_t x, int32_t y, int32_t w, int32_t h) const
    {
        // TODO: 範囲チェック
        //
        const int32_t xi = x - 1;
        const int32_t yi = y - 1;
        // 左
        const Float lValue = (x == 0) ? 0.0f : table_[(xi + 0) + (yi + h) * width_];
        // 上
        const Float uValue = (y == 0) ? 0.0f : table_[(xi + w) + (yi + 0) * width_];
        // 左上
        const Float luValue = ((y == 0) || (x == 0)) ? 0.0f : table_[xi + yi * width_];
        // 現在の場所
        const Float cValue = table_[(xi + w) + (yi + h) * width_];
        //
        return cValue - lValue - uValue + luValue;
    }

    // 
    Float total() const
    {
        return table_.back();
    }

    // u,v,pdf
    std::tuple<float, float, float> sample(float u, float v) const
    {
        //
        const auto lowerBound = [](int32_t size, Float value, const std::function<Float(int32_t)>& cdf)
        {
            int32_t low = 0;
            int32_t high = size;
            //
            while (low < high)
            {
                const int32_t mid = (low + high) / 2;
                const Float c = cdf(mid);
                if (value <= c)
                {
                    high = mid;
                }
                else
                {
                    low = mid + 1;
                }
            }
            return high;
        };

        // Y方向の二分探索
        const auto cdfCol = [this](int32_t y)
        {
            return (y == 0) ? 0 : table_[(y)* width_ - 1];
        };
        const int32_t col = std::max( lowerBound(height_, total() * v, [&](int32_t y)
        {
            const Float cdf = cdfCol(y);
            return cdf;
        }) - 1, 0);
        // X方向の二分探索
        const auto cdfRow = [this, col](int32_t x)
        {
            if (x == 0)
            {
                return Float(0.0f);
            }
            const int32_t xi = x - 1;
            const Float uValue = (col == 0) ? 0.0f : table_[xi + (col - 1) * width_];
            return table_[xi + col * width_] - uValue;
        };
        const int32_t row = std::max(lowerBound(width_, cdfRow(width_) * u, [&](int32_t x)
        {
            const Float cdf = cdfRow(x);
            return cdf;
        }) - 1, 0);
        // UVにして返す
        const float nu = (float(row) + 0.5f) / float(width_);
        const float nv = (float(col) + 0.5f) / float(height_);
        const float pdf = float(value(row, col) / total() * Float(width_*height_));
        return std::make_tuple(nu, nv, pdf);
    }

private:
    int32_t width_ = 0;
    int32_t height_ = 0;
    // summed are table
    std::vector<Float> table_;
};
