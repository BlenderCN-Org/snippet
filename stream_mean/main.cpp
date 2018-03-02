/*
 http://www.nowozin.net/sebastian/blog/streaming-mean-and-variance-computation.html
 */

#include <cstdio>
#include <cstdint>
#include <random>

//
class FloatStreamStats
{
public:
    void add(float value, float weight)
    {
        const float q = value - wmean_;
        const float tmpSumw = sumw_ + weight;
        const float r = q * weight / tmpSumw;
        wmean_ += r;
        t_ += q * r * sumw_;
        sumw_ = tmpSumw;
        n_ += 1;
    }
    int32_t count() const
    {
        return n_;
    }
    float mean() const
    {
        return wmean_;
    }
    float var() const
    {
        return t_ * float(n_) / (sumw_ * float(n_ - 1));
    }
private:
    float sumw_ = 0.0f;
    float wmean_ = 0.0f;
    float t_ = 0.0f;
    int32_t n_ = 0;
};

/*
 Simpler Online Updates for Arbitrary-Order Central Moments
 https://arxiv.org/pdf/1510.04923.pdf
 */
class Stats
{
public:
    void add(float v)
    {
        n_ += 1;
        const float delta = v - mu_;
        const float delta_n = delta / float(n_);
        mu_ += delta_n;
        m2_ += delta * ( delta - delta_n );
        const float delta_2 = delta * delta;
        const float delta_n_2 = delta_n * delta_n;
        m3_ += -3.0 * delta_n * m2_ + delta * ( delta_2 - delta_n_2 );
        m4_ += -4.0 * delta_n * m3_ - 6.0 * delta_n_2 * m2_ + delta * ( delta * delta_2 - delta_n * delta_n_2 );
    }
    float mean() const
    {
        return mu_;
    }
    float variance() const
    {
        return m2_/float(n_);
    }
    float skewness() const
    {
        return sqrtf(float(n_)) * m3_ / std::powf(m2_, 1.5f);
    }
    float kurtosis() const
    {
        return (float(n_) * m4_) / (m2_ * m2_);
    }
private:
    int32_t n_ = 0;
    float mu_ = 0.0f;
    float m2_ = 0.0f;
    float m3_ = 0.0f;
    float m4_ = 0.0f;
};

void test()
{
    Stats stats;
    std::random_device seed_gen;
    std::default_random_engine engine(seed_gen());
    const float lambda = 10.0f;
    std::poisson_distribution<> dist(lambda);
    for(int32_t i=0;i<1024 * 1024;++i)
    {
        stats.add(dist(engine));
    }
    printf("mean    :%f(%f)\n", stats.mean(), lambda);
    printf("variance:%f(%f)\n", stats.variance(), lambda);
    printf("skewness:%f(%f)\n", stats.skewness(), std::powf(lambda,-0.5f));
    printf("kurtosis:%f(%f)\n", stats.kurtosis(), std::powf(lambda,-1.0f));
}

//
int32_t main()
{
    test();
    return 0;
    FloatStreamStats fs;
    fs.add(1.0f, 1.0f);
    fs.add(0.0f, 0.5f);
    printf("%f\n", fs.mean());
    
    return 0;
}
