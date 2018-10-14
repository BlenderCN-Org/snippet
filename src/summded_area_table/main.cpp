/*
 TODOs
 - 特定の領域の積分の結果があっているかのテストを書く
 - 画像をロードして扱えるようにする
 - dumpを実行して元のイメージに戻ることを確認する
 - カラムメジャーに置き換えたりできるようにする
 - Envmap用に外から特定の領域の
 - 内部で使うfloatとdoubleを選べるようにすり
 */


// CRT
#include <cstdio>
#include <vector>
#include <cstdint>
#include <functional>

//
class SummedAreaTable
{
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
                   const std::function<float(int32_t x, int32_t y)>& gen)
    {
        width_ = width;
        height_ = height;
        //
        table_.resize(width*height);
        // 一番左上
        table_[0] = gen(0,0);
        // 一行目
        for(int32_t x=1;x<width;++x)
        {
            table_[x] = table_[x-1] + gen(x,0);
        }
        //
        for(int32_t y=1;y<height;++y)
        {
            for(int32_t x=0;x<width;++x)
            {
                const int32_t idx = x + y * width_;
                const float value = gen(x,y);
                // 一つ目
                if(x == 0)
                {
                    table_[idx] = table_[idx-width_] + value;
                }
                else
                {
                    table_[idx] = table_[idx-1] + table_[idx-width_] + value;
                }
            }
        }
    }
    
    // 特定の位置の値
    float value(int32_t x, int32_t y) const
    {
        // 左上,右上,左下,右下
        //
        //
        const int32_t idx = x + y * width_;
        return table_[idx];
    }
    
    // 特定のサブ領域の和を返す
    float sum(int32_t x, int32_t y, int32_t w, int32_t h)
    {
        // 左上,右上,左下,右下
        const int32_t luIdx =  (x + 0 ) + (y + 0 ) * width_;
        const int32_t ruIdx =  (x + w) + (y + 0 ) * width_;
        const int32_t ldIdx =  (x + 0) + (y + h ) * width_;
        const int32_t rdIdx =  (x+ w) + (y + h ) * width_;
        return table_[luIdx] + table_[rdIdx] - table_[ruIdx] - table_[ldIdx];
    }
    
private:
    int32_t width_ = 0;
    int32_t height_ = 0;
    // summed are table
    std::vector<float> table_;
};

//
int32_t main()
{
    void test();
    test();
    
}

void test()
{
    // 元の関数
    const auto gen = [](int32_t x, int32_t y)
    {
        return 1;
        return x + y;
    };
    //
    SummedAreaTable sat;
    sat.construct(4, 4, gen);
    //
    for(int32_t y=0;y<sat.height();++y)
    {
        for(int32_t x=0;x<sat.width();++x)
        {
            const float v0 = sat.value(x, y);
            const float v1 = gen(x,y);
            if(v0 != v1)
            {
                printf("%f,%f\n", v0, v1);
            }
        }
    }
}





















