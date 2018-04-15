// https://en.wikipedia.org/wiki/Control_variates
#include <cstdint>
#include <random>

//
class FloatStreamStats
{
public:
    void add(float v)
    {
        n_ += 1;
        const float newMean = (v - mean_) / float(n_) + mean_;
        const float newM = (v - mean_) * (v - newMean) + M_;
        M_ = newM;
        mean_ = newMean;
    }
    float mean() const { return mean_; }
    float variance() const { return M_ / float(n_ - 1.0f); }
    float sigma() const { return sqrtf(variance()); }
private:
    int32_t n_ = 0;
    float mean_ = 0.0f;
    float M_ = 0.0f;
};

//
class FloatStreamStats2
{
public:
public:
    FloatStreamStats2() = default;
    void add(float x, float y)
    {
        n_ += 1;
        const float invn = 1.0f/float(n_);
        const float dx = x - mux_;
        //
        const float oldmux = mux_;
        const float oldmuy = muy_;
        mux_ += dx * invn;
        muy_ += (y - muy_) * invn;
        C_ += dx * (y - muy_);
        //
        mx_ = (x - oldmux) * (x - mux_) + mx_;
        my_ = (y - oldmuy) * (y - muy_) + my_;
    }
    int32_t size() const { return n_; }
    float varX() const { return mx_ / (n_ - 1.0f); }
    float varY() const { return my_ / (n_ - 1.0f); }
    float sigmaX() const { return std::sqrtf(varX()); }
    float sigmaY() const { return std::sqrtf(varY()); }
    float cov() const { return C_ / float(n_ - 1); }
    float R2() const { return cov()/(sigmaX()*sigmaY()); }
    
protected:
    float mux_ = 0.0f;
    float muy_ = 0.0f;
    float mx_ = 0.0f;
    float my_ = 0.0f;
    float C_ = 0.0f;
    int32_t n_ = 0.0f;
};
//
int main()
{
    //
    std::mt19937 rng(0x123);
    std::uniform_real_distribution<> dist(0.0f,1.0f);
    std::normal_distribution<> errorDist(0.0f, 0.1f);
    FloatStreamStats2 fs2;
    FloatStreamStats fsm, fst, fsmstar;
    for(int32_t sn=0;sn<1024;++sn)
    {
        //
        const float x = dist(rng);
        const float m = std::expf(x);
        const float t = x;
        //
        float C = (sn < 2 )
        ? 0.0f
        : fsm.sigma()/fst.sigma() * fs2.R2();
        const float ms = m - C * (t - fst.mean());
        //
        fsm.add(m);
        fst.add(t);
        fsmstar.add(ms);
        fs2.add(m,t);
    }
    printf("%f\n", fs2.R2());
    printf("%f,%f\n", fsm.mean(), fsm.variance());
    printf("%f,%f\n", fst.mean(), fst.variance());
    printf("%f,%f\n", fsmstar.mean(), fsmstar.variance());
}
