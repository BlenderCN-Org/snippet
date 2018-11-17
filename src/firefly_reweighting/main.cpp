/*
"Reweighting Firefly Samples for Improved Finite-Sample Monte Carlo Estimates"
https://cg.ivd.kit.edu/rwmc.php
*/

/*
TODO
- pdfと実際のpdfが大幅に違うようなものを作る
- 
*/
#define _USE_MATH_DEFINES
#include <cmath>
#include <cstdint>
#include <random>
#include <tuple>
#include <array>

//
const double PI = double(M_PI);

//
class FloatStreamStats
{
public:
    void add(double v)
    {
        ++count_;
        mu_ += 1.0f/double(count_) * (v - mu_);
    }
    double mu() const
    {
        return mu_;
    }
private:
    double mu_ = 0.0f;
    uint64_t count_ = 0;
};

// (x,pdf)
std::tuple<double, double> sampleUnif(float u)
{
    return { u, 1.0f };
}

// (x,pdf)
std::tuple<double, double> sampleSin(double u)
{
    const double HPI = 0.5 * PI;
    const double x = std::acos(1.0f - u) / HPI;
    const double pdf = std::sin(x * HPI) * HPI;
    return { x, pdf };
}

// (x,pdf) y = x
std::tuple<double, double> sampleIncriment(float u)
{
    const double x = std::sqrt(u);
    return { x, 2.0f * x };
}


// [0,1]の入力を取り、その場所の値と真のpdfを返す
std::tuple<double, double> fBox(double x)
{
    if (x < 0.1)
    {
        return { 10.0, 10.0 };
    }
    else
    {
        return { 0.0, 0.0 };
    }
}

std::tuple<double, double> fCos(double x)
{
    const double v = std::cos(x*PI*0.5) * PI * 0.5;
    return { v, v };
}

std::tuple<double, double> fConst(double x)
{
    return { 1.0, 1.0 };
}

//
class Reweighter
{
public:
    Reweighter()
    {
        float bj = 1.0f;
        for (auto& frame : frameBuffers_)
        {
            frame.bj = bj;
            bj *= BASE;
        }
    }
    void add(double value)
    {
        //
        const auto logb = [&](float x)
        {
            const float num = std::logf(x);
            const float denom = std::logf(8.0f);
            return num / denom;
        };
        const int32_t frameIdx = int32_t(logb(value));
        const float bj = frameBuffers_[frameIdx].bj;
        const float alphaj = (bj / value - INV_BASE) / (1.0f - INV_BASE);
        const float alphaj1 = 1.0f - alphaj;
        //
        frameBuffers_[frameIdx + 0].v += value * alphaj;
        frameBuffers_[frameIdx + 1].v += value * alphaj1;
        //
        ++total_;

        // rcの算出
        const float kappaMin = 1.0f;
        const float rc = total()/ (sampleCount(frameIdx) - kappaMin);
        // rvの算出
        const float rv = value / Emin_;
        //
        const float rstar = std::min(rc, rv);
        // Eminの更新
        Emin_ += value / (kappa_ + rstar);
    }
    float sampleCount(int32_t frameIndex) const
    {
        const float t0 = frameBuffers_[frameIndex - 1].v / frameBuffers_[frameIndex - 1].bj;
        const float t1 = frameBuffers_[frameIndex + 0].v / frameBuffers_[frameIndex + 0].bj;
        const float t2 = frameBuffers_[frameIndex + 1].v / frameBuffers_[frameIndex + 1].bj;
        const float count = (t0 + t1 + t2);
        return count;
    }
    float total() const
    {
        float t = 0.0f;
        for (auto& frame : frameBuffers_)
        {
            t += frame.v / frame.bj;
        }
        return t;
    }
    float ratio(int32_t index) const
    {
        return sampleCount(index)/ total();
    }
    float E() const
    {

    }
private:
    //
    const float BASE = 8.0f;
    const float INV_BASE = 1.0f / BASE;
    struct FrameBuffer
    {
        float v = 0.0f;
        int32_t count_ = 0;
        float bj = 0.0f;
    };
    // フレームバッファ
    std::array<FrameBuffer, 6> frameBuffers_;
    //
    double alpha_ = 1.0;
    double kappa_ = 10.0;
    // 全サンプル数
    int32_t total_ = 0;
    //
    float Emin_ = 0.0f;
};

void test()
{
    Reweighter rew;
    rew.add(3.0f);
    rew.add(3.0f);
    /*rew.add(8.0f);
    rew.add(8.0f);
    rew.add(8.0f);
    rew.add(8.0f);
    rew.add(8.0f);
    rew.add(8.0f);
    rew.add(8.0f);
    rew.add(8.0f);*/
    //rew.add(8.1f);
    //rew.add(6400.0f);
    //rew.add(4.1f);
    const float total = rew.total();
    const float count = rew.sampleCount(1);
    printf("%f\n", count);
}

//
void main()
{
    test();
    //
    std::mt19937 rng(0x123);
    std::uniform_real_distribution<double> dist(0.0f,1.0f);
    FloatStreamStats fsNaive;
    Reweighter rew;
    //
    //const auto func = fCos;
    const auto func = fBox;
    //const auto func = fConst;

    const auto sampler = sampleSin;
    //const auto sampler = sampleUnif;
    //const auto sampler = sampleIncriment;
    const int32_t N = 1024 * 16;
    //
    for (size_t sn = 0; sn < N; ++sn)
    {
        const double u = dist(rng);
        const auto [x,pdf] = sampler(u);
        const auto [v,truePdf] = func(x);
        // 通常の重点サンプリング
        if (pdf != 0.0)
        {
            fsNaive.add(v / pdf);
        }
        // TODO: 
        rew.add(v/pdf);

        //
        /*if (!(sn % 16))
        {
            printf("%f\n", fsNaive.mu());
        }*/
    }
    //
    printf("%f\n", fsNaive.mu());
    //printf("%f\n", rew.mu());    
}
