#include <Eigen/Core>
#include <Eigen/LU>
#include <Eigen/SparseCore>
#include <Eigen/Sparse>
#include <iostream>
#include <array>
#include <random>


/*
------------------------------------------------
浮動小数点の数列の統計情報
------------------------------------------------
*/
struct FloatStreamStats
{
public:
    FloatStreamStats() = default;
    //
    void add(float v)
    {
        ++n_;
        // mean/varの更新
        const float fn = float(n_);
        const float newAverage = (v - mean_) / fn + mean_;
        const float newM = (v - newAverage) * (v - mean_) + m_;
        mean_ = newAverage;
        m_ = newM;
    }
    float mean() const
    {
        return mean_;
    }
    float variance() const
    {
        const float fn = (float)n_;
        return m_ / fn;
    }
private:
    int32_t n_ = 0;
    float mean_ = 0.0f;
    float m_ = 0.0f;
};

//
struct Sample
{
public:
    float x;
    float y;
};

//
struct RegressionResult
{
public:
    float bias;
    float slope;
};

//
static RegressionResult LMSNaive(const std::vector<Sample>& samples)
{
    // 実際に解く
    Eigen::Matrix<float, Eigen::Dynamic, 2> X;
    Eigen::Matrix<float, Eigen::Dynamic, 1> b;
    X.resize(samples.size(), 2);
    b.resize(samples.size(), 1);
    for (int32_t sn = 0; sn < samples.size(); ++sn)
    {
        auto& s = samples[sn];
        X(sn, 0) = 1.0f;
        X(sn, 1) = s.x;
        b(sn) = s.y;
    }
    auto Xt = X.transpose();
    auto beta = (Xt*X).inverse()*Xt* b;
    return { beta(0),beta(1) };
}

//
static RegressionResult LMSNaiveDual(const std::vector<Sample>& samples)
{
    //
    auto cd = LMSNaive(samples);
    // 外れ値と思われるものを除去
    std::vector<Sample> goods;
    for (auto& s : samples)
    {
        const float dist = std::fabsf(-cd.slope * s.x + s.y - cd.bias);
        if (dist < 1.0f)
        {
            goods.push_back(s);
        }
    }
    printf("%d\n", goods.size());
    return LMSNaive(goods);
}

static RegressionResult RANSAC(const std::vector<Sample>& samples, std::default_random_engine& rng)
{
    //
    RegressionResult bestResult;
    int32_t bestCount = 0;
    /*
    0.1: 0.171
    0.5: 0.063
    1.0: 0.0617
    */
    const float thres = 0.4f;
    //
    for (int32_t testNo=0;testNo<512;++testNo)
    {
        // 二点を選択する
        const int32_t i0 = rng() % samples.size();
        const int32_t i1 = rng() % samples.size();
        if (i0 == i1)
        {
            continue;
        }
        // 二点から直線を作成する
        const auto& s0 = samples[i0];
        const auto& s1 = samples[i1];
        const float slope = (s0.y - s1.y) / (s0.x - s1.x);
        const float bias = s0.y - slope * s0.x;
        const float d0 = -slope * s0.x + s0.y - bias;
        const float d1 = -slope * s1.x + s1.y - bias;
        // 閾値以内にあるものの数を算出
        int32_t count = 0;
        for (auto& s : samples)
        {
            const float dif = std::fabsf(-slope * s.x + s.y - bias);
            if (dif < thres)
            {
                ++count;
            }
        }
        if (bestCount < count )
        {
            bestCount = count;
            bestResult.slope = slope;
            bestResult.bias = bias;
        }
    }
    //
    std::vector<Sample> goods;
    for (auto& s : samples)
    {
        const float dif = std::fabsf(-bestResult.slope * s.x + s.y - bestResult.bias);
        if (dif < thres)
        {
            goods.push_back(s);
        }
    }
    printf("%d\n", goods.size());
    return LMSNaive(goods);
}

// http://home.hiroshima-u.ac.jp/tkurita/lecture/statimage/node30.html
static RegressionResult LMedS(const std::vector<Sample>& samples, std::default_random_engine& rng)
{
    RegressionResult bestResult;
    float bestMidError = 100000000.0f;
    std::vector<float> errors(samples.size());
    //
    for (int32_t testNo = 0; testNo<512; ++testNo)
    {
        // 二点を選択する
        const int32_t i0 = rng() % samples.size();
        const int32_t i1 = rng() % samples.size();
        if (i0 == i1)
        {
            continue;
        }
        // 二点から直線を作成する
        const auto& s0 = samples[i0];
        const auto& s1 = samples[i1];
        const float slope = (s0.y - s1.y) / (s0.x - s1.x);
        const float bias = s0.y - slope * s0.x;
        const float d0 = -slope * s0.x + s0.y - bias;
        const float d1 = -slope * s1.x + s1.y - bias;
        // 閾値以内にあるものの数を算出
        int32_t count = 0;
        errors.clear();
        for (auto& s : samples)
        {
            const float dif = std::fabsf(-slope * s.x + s.y - bias);
            errors.push_back(dif);
        }
        std::nth_element(errors.begin(), errors.begin() + errors.size() / 2, errors.end());
        const float midError = errors[errors.size() / 2];
        if (midError < bestMidError )
        {
            bestMidError = midError;
            bestResult.slope = slope;
            bestResult.bias = bias;
        }
    }
    //
    const float sigma = bestMidError * 2.5f;
    //
    std::vector<Sample> goods;
    for (auto& s : samples)
    {
        const float dif = std::fabsf(-bestResult.slope * s.x + s.y - bestResult.bias);
        if (dif < sigma)
        {
            goods.push_back(s);
        }
    }
    //
    return LMSNaive(goods);
}

//
void test1()
{
    //
    FloatStreamStats fs0, fs1;
    //
    for (int32_t testNo=0; testNo<128;++testNo)
    {
        //
        const int32_t numSample = 1024;
        // 正規分布の作成
        std::default_random_engine engine(1234 * testNo);
        std::normal_distribution<> dist(0.0, 1.0);

        // サンプル位置の生成
        const auto genX = [&](int32_t sn)
        {
#if 1
            return float(sn) / float(numSample-1);
#else
            if (sn < numSample / 2)
            {
                return 0.0f;
            }
            else
            {
                return 1.0f;
            }
#endif
        };

        // 対称の関数
        const auto fun = [&](float x) -> float
        {
#if 1
            // ただの y = 0x + c
            return float(dist(engine)) * 1.0f + 10.0f;
#elif 1
            // 二次関数
            return (x-1.0f)*(x-0.0f) + float(dist(engine)) * 0.2f;
#else
            // 0.5で階段関数
            return (x <= 0.5f) ? 0.0f : 1.0f;
#endif
        };
        
        // サンプルを作成する
        std::vector<Sample> samples;
        for (int32_t sn = 0; sn < numSample; ++sn)
        {
            // 
            const float x = genX(sn);
            const float y = fun(x);
            samples.push_back({x,y});
        }
        //
        //auto ret = LMSNaive(samples);
        //auto ret = LMSNaiveDual(samples);
        //auto ret = RANSAC(samples, engine);
        auto ret = LMedS(samples, engine);
        // 定数項
        fs0.add(ret.bias);
        fs1.add(ret.slope);
    }
    /*
    頂点位置にした場合
    ① 一次関数
    Const +9.999527 0.025346
    Slope -0.000065 0.035189

    ② 二次関数
    Const -0.000175 0.024047
    Slope -0.000108 0.035431

    一様にサンプルした場合
    ① 一次関数
    Const +9.999574 0.034384
    Slope +0.000370 0.061719

    ② 二次関数
    Const -0.165766 0.034384
    Slope +0.000365 0.061719
    */
    printf("Const %+f %f\n", fs0.mean(), std::sqrtf(fs0.variance()));
    printf("Slope %+f %f\n", fs1.mean(), std::sqrtf(fs1.variance()));
}

//
int main()
{
    test1();
    //test1();
    return 0;
}
