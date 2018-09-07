#define _USE_MATH_DEFINES
#include <cmath>
#include <cstdio>
#include <cassert>
#include <cstdint>
//
#include <random>

//
float nextFloatUp(float v){ return std::nextafter(v, (std::numeric_limits<float>::max)()); }
//
float nextFloatDown(float v){ return std::nextafter(v, (std::numeric_limits<float>::lowest)()); }
//
class ErrFloat
{
public:
    ErrFloat() = default;
    ErrFloat(float v)
        :value_(v), high_(v), low_(v), ref_(v)
    {}
    ErrFloat(float v, float high, float low)
        :value_(v), high_(high), low_(low), ref_(v)
    {}
    ErrFloat(const ErrFloat& other)
        :value_(other.value_), high_(other.high_), low_(other.low_), ref_(other.ref_)
    {}
    ErrFloat operator + (const ErrFloat& other) const
    {
        ErrFloat r;
        r.value_ = value_ + other.value_;
        r.high_ = nextFloatUp(high_ + other.high_);
        r.low_ = nextFloatDown(low_ + other.low_);
        r.ref_ = ref_ + other.ref_;
        return r;
    }
    ErrFloat operator - (const ErrFloat& other) const
    {
        ErrFloat r;
        r.value_ = value_ - other.value_;
        r.high_ = nextFloatUp(high_ - other.low_);
        r.low_ = nextFloatDown(low_ - other.high_);
        r.ref_ = ref_ - other.ref_;
        return r;
    }
    ErrFloat operator * (const ErrFloat& other) const
    {
        const float v0 = high_ * other.high_;
        const float v1 = high_ * other.low_;
        const float v2 = low_ * other.high_;
        const float v3 = low_ * other.low_;
        ErrFloat r;
        r.value_ = value_ * other.value_;
        r.high_ = nextFloatUp(std::max(std::max(v0,v1), std::max(v2,v3)));
        r.low_ = nextFloatDown(std::min(std::max(v0,v1), std::min(v2, v3)));
        r.ref_ = ref_ * other.ref_;
        return r;
    }
    ErrFloat operator / (const ErrFloat& other) const
    {
        ErrFloat r;
        r.value_ = value_ / other.value_;
        r.ref_ = ref_ / other.ref_;
        if ((other.low_ <= 0.0f) &&
            (0.0f <= other.high_))
        {
            r.high_ = std::numeric_limits<float>::infinity();
            r.low_ = -std::numeric_limits<float>::infinity();
        }
        else
        {
            const float v0 = high_ / other.high_;
            const float v1 = high_ / other.low_;
            const float v2 = low_ / other.high_;
            const float v3 = low_ / other.low_;
            r.high_ = nextFloatUp(std::max(std::max(v0,v1), std::max(v2, v3)));
            r.low_ = nextFloatDown(std::min(std::min(v0, v1), std::min(v2, v3)));
        }
        return r;
    }
    ErrFloat sqrt() const
    {
        ErrFloat r;
        r.value_ = std::sqrtf(value_);
        r.low_ = nextFloatDown(std::sqrtf(low_));
        r.high_ = nextFloatUp(std::sqrtf(high_));
        return r;
    }
    float value() const
    {
        return value_;
    }
    float high() const
    {
        return high_;
    }
    float low() const
    {
        return low_;
    }
    void printAndCheck() const
    {
        if (std::isnan(value_))
        {
            return;
        }
        printf("V:%+10.10f, Int:[%+10.10f,%+10.10f]  Ref:%+10.10Lf\n", value_, low_, high_, ref_);
        assert((long double)(low_) <= ref_);
        assert(ref_ <= (long double)(high_));
    }
private:
    // 値
    float value_;
    // 区間
    float high_;
    float low_;
    // 真値
    long double ref_;
};

// 二次方程式の解(通常版)
bool solveQuadraticNaive(float a, float b, float c, float* t0, float* t1)
{
    const float D = b * b - 4.0f * a * c;
    if (D < 0.0)
    {
        return false;
    }
    const float Dsqrt = std::sqrtf(D);
    float q;
    if (float(b) < 0.0f)
    {
        q = -0.5f * (b - Dsqrt);
    }
    else
    {
        q = -0.5f * (b + Dsqrt);
    }
    *t0 = q / a;
    *t1 = c / q;
    if (*t1 < *t0)
    {
        std::swap(*t0, *t1);
    }
    return true;
}

// 二次方程式の解(ErrFloat版)
bool solveQuadraticErrFloat(ErrFloat a, ErrFloat b, ErrFloat c, ErrFloat* t0, ErrFloat* t1)
{
    // 解の判定のみdoubleで行う
    const ErrFloat D = b * b - ErrFloat(4.0f) * a * c;
    const double Dhigh = double(b.value()) * double(b.value()) - 4.0 * (double)a.value() * double(c.value());
    // 解なし
    if (Dhigh < 0.0)
    {
        return false;
    }
#if 0
    const double DsqrtTmp = std::sqrt(Dhigh);
    const float eps = std::numeric_limits<float>::epsilon() * 0.5f * DsqrtTmp;
    ErrFloat Dsqrt(float(DsqrtTmp), float(DsqrtTmp)-eps, float(DsqrtTmp)+eps);
#else
    const ErrFloat Dsqrt = D.sqrt();
#endif
    ErrFloat q;
    if (b.value() < 0.0f)
    {
        q = ErrFloat(-0.5f) * (b - Dsqrt);
    }
    else
    {
        q = ErrFloat(-0.5f) * (b + Dsqrt);
    }
    *t0 = q / a;
    *t1 = c / q;
    if (t1->value() < t0->value())
    {
        std::swap(*t0, *t1);
    }
    return true;
}

//
struct Vec3
{
public:
    float x;
    float y;
    float z;
    //
    Vec3 operator - (Vec3 other) const
    {
        Vec3 ret;
        ret.x = x - other.x;
        ret.y = y - other.y;
        ret.z = z - other.z;
        return ret;
    }
};

//
bool intersectSphereNaive(
    Vec3 org,
    Vec3 dir,
    Vec3 center,
    float radius,
    float* t)
{
    const Vec3 dif = org - center;
    const float ox(dif.x);
    const float oy(dif.y);
    const float oz(dif.z);
    const float dx(dir.x);
    const float dy(dir.y);
    const float dz(dir.z);
    const float r2 = radius * radius;
    const float a = dx * dx + dy * dy + dz * dz;
    const float b = 2 * (dx * ox + dy * oy + dz * oz);
    const float c = ox * ox + oy * oy + oz * oz - r2;
    //
    ErrFloat t0, t1;
    if (!solveQuadraticErrFloat(a, b, c, &t0, &t1))
    {
        return false;
    }
    //
    if (t1.low() <= 0.0f)
    {
        return false;
    }
    //
    ErrFloat rayt;
    if (t0.low() <= 0.0f)
    {
        rayt = t1;
    }
    else
    {
        rayt = t0;
    }
    // 安全側に倒す
    *t = float(rayt.low());
    //
    return true;
}

//
bool intersectSphereErrFloat(
    Vec3 org,
    Vec3 dir,
    Vec3 center,
    float radius,
    float* t)
{
    const ErrFloat orgx(org.x);
    const ErrFloat orgy(org.y);
    const ErrFloat orgz(org.z);
    const ErrFloat cx(center.x);
    const ErrFloat cy(center.y);
    const ErrFloat cz(center.z);
    const ErrFloat ox = orgx - cx;
    const ErrFloat oy = orgy - cy;
    const ErrFloat oz = orgz - cz;
    const ErrFloat dx(dir.x);
    const ErrFloat dy(dir.y);
    const ErrFloat dz(dir.z);
    const ErrFloat r2 = ErrFloat(radius) * ErrFloat(radius);
    const ErrFloat a = dx * dx + dy * dy + dz * dz;
    const ErrFloat b = ErrFloat(2.0f) * (dx * ox + dy * oy + dz * oz);
    const ErrFloat c = ox * ox + oy * oy + oz * oz - r2;
    //
    ErrFloat t0, t1;
    if (!solveQuadraticErrFloat(a, b, c, &t0, &t1))
    {
        return false;
    }
    //
    if (t1.low() <= 0.0f)
    {
        return false;
    }
    //
    ErrFloat rayt;
    if (t0.low() <= 0.0f)
    {
        rayt = t1;
    }
    else
    {
        rayt = t0;
    }
    // 安全側に倒す
    *t = float(rayt.low());
    //
    return true;
}

//
void test0()
{
    // テスト用の浮動小数を作成する
    const auto genTestFloat = [](int32_t index) -> float
    {
        switch (index)
        {
        case 0: return 0.0f;
        case 1: return -0.0f;
        case 2: return +1.0f;
        case 3: return -1.0f;
        case 4: return +1.000000001f;
        case 5: return -1.000000001f;
        case 6: return +2.000000000001f;
        case 7: return +100000.000000000001f;
        case 8: return +100000000.000000000001f;
        case 9: return -100000000.000000000001f;
        default:
        {
            std::mt19937 gen(index);
            for(int32_t i=0;i<128;++i){ gen();}
            std::uniform_real_distribution<float> dist(-1.0f, 1.0f);
            return dist(gen);
        }
        break;
        }
    };
    //
    const int32_t fi = 100;
    for(int32_t i=0;i<fi;++i)
    {
        for(int32_t j=0;j<fi;++j)
        {
            ErrFloat f0(genTestFloat(i));
            ErrFloat f1(genTestFloat(j));
            const auto e0 = f0 - f1;
            const auto e1 = f0 + f1;
            const auto e2 = f0 * f1;
            const auto e3 = f0 / f1;
            e0.printAndCheck();
            e1.printAndCheck();
            e2.printAndCheck();
            e3.printAndCheck();
        }
    }
}

//
void test1()
{
    //
    const auto testSphere = [](float dist)
    {
        const Vec3 testCenter = { dist,0.0f,0.0f };
        std::mt19937 rng(123);
        const int32_t numSample = 1024 * 4;
        int32_t invalidDistCountNaive = 0, invalidDistCountErrFloat = 0;
        int32_t invalidHitCountNaive = 0, invalidHitCountErrFloat = 0;
        for (int32_t sn = 0; sn < numSample; ++sn)
        {
            //
            const auto getSphere = [&]() ->Vec3
            {
                std::uniform_real_distribution<> dist(0.0, 1.0);
                const float u = float(dist(rng));
                const float v = float(dist(rng));
                const float z = 2.0f * u - 1.0f;
                const float theta = (2.0f * v - 1.0f) * float(M_PI);
                const float iz = sqrtf(1.0f - z * z);
                const float x = sinf(theta) * iz;
                const float y = cosf(theta) * iz;
                return Vec3{ x, y, z };
            };
            // テスト1
            {
                const Vec3 org = testCenter;
                const Vec3 dir = getSphere();
                const Vec3 center = org;
                const float radius = 1.0f;
                // 通常版
                {
                    float t = 0;
                    const bool hit = intersectSphereNaive(org, dir, center, radius, &t);
                    if (!hit)
                    {
                        ++invalidHitCountNaive;
                    }
                    if (t > 1.0f)
                    {
                        ++invalidDistCountNaive;
                    }
                }
                // ErrFloat版
                {
                    float t = 0;
                    const bool hit = intersectSphereErrFloat(org, dir, center, radius, &t);
                    if (!hit)
                    {
                        ++invalidHitCountErrFloat;
                    }
                    if (t > 1.0f)
                    {
                        ++invalidDistCountErrFloat;
                    }
                }
            }
            // テスト2
            {
                const Vec3 dir = getSphere();
                const Vec3 org =
                {
                    float(double(testCenter.x) - double(dir.x) * 2.0),
                    float(double(testCenter.y) - double(dir.y) * 2.0),
                    float(double(testCenter.z) - double(dir.z) * 2.0)
                };
                const Vec3 center = testCenter;
                const float radius = 1.0f;
                // 通常版
                {
                    float t = 0;
                    const bool hit = intersectSphereNaive(org, dir, center, radius, &t);
                    if (!hit)
                    {
                        ++invalidHitCountNaive;
                    }
                    if (t > 1.0f)
                    {
                        ++invalidDistCountNaive;
                    }
                }
                // ErrFloat版
                {
                    float t = 0;
                    const bool hit = intersectSphereErrFloat(org, dir, center, radius, &t);
                    if (!hit)
                    {
                        ++invalidHitCountErrFloat;
                    }
                    if (t > 1.0f)
                    {
                        ++invalidDistCountErrFloat;
                    }
                }
            }
        }
        printf("%f,%f,%f\n", dist, float(invalidDistCountNaive) / numSample, float(invalidDistCountErrFloat) / numSample);
    };
    testSphere(0.0f);
    for (float d = 1.0f; d < 1000.0f; d += 1.0f)
    {
        testSphere(d);
    }
}

//
int main()
{
    //test0();
    test1();
    return 0;
}
