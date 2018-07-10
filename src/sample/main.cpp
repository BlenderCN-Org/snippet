/*
TODOs
- LDSampler‚ğì¬‚·‚é
- radicalInverse‚ğì¬‚·‚é
- bestCandidateSampler‚ğì¬‚·‚é
- 
*/
#include <functional>
#include <random>
#include <array>

//
struct Vec2
{
public:
    double x = 0.0;
    double y = 0.0;
public:
    Vec2() = default;
    Vec2(double ax, double ay)
        :x(ax),y(ay)
    {}

};

//
class Sampler
{
public:
    virtual ~Sampler() = default;
    virtual Vec2 get2d() = 0;
};

//
class RandomSampler
    :public Sampler
{
public:
    Vec2 get2d() override
    {
        std::uniform_real_distribution<> dist(0.0, 1.0);
        const double x = dist(engine_);
        const double y = dist(engine_);
        return Vec2(x,y);
    }
private:
    std::default_random_engine engine_ = std::default_random_engine(0x1234);
};

// Ï•ª
double integrate(const std::function<double(const Vec2& uv)>&fun, Sampler* sampler, int32_t sampleSize)
{
    double total = 0.0;
    const double invTotal = 1.0 / sampleSize;
    for (int32_t sn=0;sn<sampleSize;++sn)
    {
        total += fun(sampler->get2d()) * invTotal;
    }
    return total;
}

//
static double fun0(const Vec2& uv)
{
    return (uv.x < 0.5f) ? 0.0 : 1.0;
}

// 
void main()
{
    struct Test
    {
        const std::function<double(const Vec2& uv)> fun;
        const double result;
    }tests[] =
    {
        { fun0, 0.5 },
        { fun0, 0.5 },
    };
    RandomSampler randomSampler;
    Sampler* samplers[] =
    {
        &randomSampler,
        &randomSampler,
    };
    //
    for (const auto& test: tests)
    {
        for (auto& sampler: samplers)
        {
            //
            for (int32_t sn = 10; sn < 10000000; sn *= 10)
            {
                const double r = integrate(test.fun, sampler, sn);
                const double a = test.result;
                const double err = std::abs((r - a) / a);
                printf("Sn:%8d Err:%10.10f\n", sn, err);
            }
        }
    }
}
