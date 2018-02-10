#include <cstdio>
#include <cstdint>
#define _USE_MATH_DEFINES
#include <cmath>
#include <chrono>
#include <array>

// 
inline float stdsin(float x)
{
    return std::sinf(x);
}

// $> HornerForm[Normal[Series[Sin[x], {x, 0, 9}]]]
inline float taylorSinOrder9(float x)
{
    const float x2 = x * x;
    return x * (1.0f + x2 * (-1.0f / 6.0f + x2 * (1.0f / 120.0f + x2*( -1.0f / 5040.0f + x2 / 362880.0f ))));
}

// $> HornerForm[Normal[Series[Sin[x], {x, 0, 11}]]]
inline float taylorSinOrder11(float x)
{
    const float x2 = x * x;
    return x * (1.0f + x2 * (-1.0f / 6.0f + x2 * (1.0f / 120.0f + x2 * (-1.0f / 5040.0f + x2 * ( 1.0f/ 362880.0f - x2/39916800.0f )))));
}

// $> HornerForm[PadeApproximant[Sin[x], {x, 0, {5, 5}}]]
inline float padesin_55(float x)
{
    const float x2 = x * x;
    const float nume = x * (166320.0f + x2 * (-22260.0f + 551.0f * x2));
    const float denom = 166320.0f + x2 * (5460.0f + 75.0f);
    return nume / denom;
}

// $> HornerForm[PadeApproximant[Sin[x], {x, 0, {7, 7}}]]
inline float padesin_77(float x)
{
    const float x2 = x * x;
    const float nume = x * (11511339840.0f + x2 * (-1640635920.0f + x2 * (52785432.0f - 479249.0f * x2)));
    const float denom = 11511339840.0f + x2 * (277920720.0f + x2 * (3177720.0f + 18361.0f * x2));
    return nume / denom;
}

// https://code.google.com/archive/p/fastapprox/
inline float fastersin(float x)
{
    static const float fouroverpi = 1.2732395447351627f;
    static const float fouroverpisq = 0.40528473456935109f;
    static const float q = 0.77633023248007499f;
    union { float f; uint32_t i; } p = { 0.22308510060189463f };

    union { float f; uint32_t i; } vx = { x };
    uint32_t sign = vx.i & 0x80000000;
    vx.i &= 0x7FFFFFFF;

    float qpprox = fouroverpi * x - fouroverpisq * x * vx.f;

    p.i |= sign;

    return qpprox * (q + p.f * qpprox);
}

// http://mooooo.ooo/chebyshev-sine-approximation/
inline float sinchebyshev(float x)
{
    float coeffs[] =
    {
        -0.10132118f,         // x
        0.0066208798f,        // x^3
        -0.00017350505f,      // x^5
        0.0000025222919f,     // x^7
        -0.000000023317787f,  // x^9
        0.00000000013291342f, // x^11
    };
    float pi_major = 3.1415927f;
    float pi_minor = -0.00000008742278f;
    float x2 = x * x;
    float p11 = coeffs[5];
    float p9 = p11 * x2 + coeffs[4];
    float p7 = p9 * x2 + coeffs[3];
    float p5 = p7 * x2 + coeffs[2];
    float p3 = p5 * x2 + coeffs[1];
    float p1 = p3 * x2 + coeffs[0];
    return
        (x - pi_major - pi_minor) *
        (x + pi_major + pi_minor) * p1 * x;
}

//
template<int32_t TABLE_SIZE>
class LookupSin
{
public:
    LookupSin()
    {
        for (int32_t i=0;i<table_.size();++i)
        {
            const float x = float(i)/float(table_.size()) * float(M_PI);
            table_[i] = std::sinf(x);
        }
    }
    inline float operator ()(float x) const
    {
#if 0 // point sample
        const int32_t idx = int32_t(x * float(M_1_PI) * tableSizeF);
        return table_[idx];
#elif 1 // liner interpolate
        const float fi = x * float(M_1_PI) * tableSizeF;
        float idx0f;
        const float f0 = std::modff(fi, &idx0f);
        const int32_t idx0 = int32_t(idx0f);
        const int32_t idx1 = std::min(idx0+1, TABLE_SIZE -1);
        return table_[idx0] * (1.0f - f0) + table_[idx1] * f0;
#elif 1 // Lagrange interpolate
        const int32_t idx = int32_t(x * float(M_1_PI) * tableSizeF);
        const int32_t idx1 = idx - 1;
        const int32_t idx2 = idx;
        const int32_t idx3 = idx + 1;
        const int32_t idx4 = idx + 2;
        //
        if ((idx1 < 0) ||
            (idx4 > TABLE_SIZE - 1))
        {
            return table_[idx];
        }
        //
        const float x1 = float(idx1) * float(M_PI) / tableSizeF;
        const float x2 = float(idx2) * float(M_PI) / tableSizeF;
        const float x3 = float(idx3) * float(M_PI) / tableSizeF;
        const float x4 = float(idx4) * float(M_PI) / tableSizeF;
        const float y1 = table_[idx1];
        const float y2 = table_[idx2];
        const float y3 = table_[idx3];
        const float y4 = table_[idx4];
        return
            +(y1 * (x - x2) * (x - x3) * (x - x4)) / ((x1 - x2) * (x1 - x3) * (x1 - x4))
            +(y2 * (x - x1) * (x - x3) * (x - x4)) / ((x2 - x1) * (x2 - x3) * (x2 - x4))
            +(y3 * (x - x1) * (x - x2) * (x - x4)) / ((x3 - x1) * (x3 - x2) * (x3 - x4))
            +(y4 * (x - x1) * (x - x2) * (x - x3)) / ((x4 - x1) * (x4 - x2) * (x4 - x3));
#endif
    }
private:
    std::array<float, TABLE_SIZE> table_;
    const float tableSizeF = float(TABLE_SIZE);
};

//
template<typename SinFun>
float rmse(SinFun sinFun, int32_t ite = 4096)
{
    float t = 0.0f;
    const float invIte = 1.0f / float(ite);
    for (int32_t i = 0; i < ite; ++i)
    {
        const float x = float(i) * invIte * float(M_PI);
        const float d = std::sinf(x) - sinFun(x);
        t += d * d * invIte;
    }
    return std::sqrtf(t);
}

//
template<typename SinFun>
float speed(SinFun sinFun)
{
    
    auto start = std::chrono::high_resolution_clock::now();
    const int64_t ite = 1024 * 1024 * 16;
    const float invIte = 1.0f / float(ite);
    for (int64_t i=0;i<ite;++i)
    {
#if 1
        // random access
        const int32_t numPoint = 1 << 14;
        const int32_t idx = int32_t((int64_t(rand()) * int64_t(numPoint)) >> 15);
        const float x = float(idx) / float(numPoint) * float(M_PI);
#else
        const float x = float(i) * invIte * float(M_PI);
#endif
        volatile const float y = sinFun(x);
    }
    auto finish = std::chrono::high_resolution_clock::now();
    auto inms = std::chrono::duration_cast<std::chrono::milliseconds>(finish - start).count();
    return float(inms);
}

//
int32_t main()
{
    /*
                RMSE      TIME(ms)
    std::sin    0.0000000 1100.00
    taylor9     0.0014491  788.00
    taylor11    0.0000858  892.00
    pade55      0.0042525  769.00
    pade77      0.0000020  859.00
    fastersin   0.0005012  745.00
    chebyshev   0.0000000  846.00
    lookup1024  0.0000448 1074.00
    lookup16384 0.0000000 1083.00
    */ 
    printf("            RMSE      TIME(ms)\n");
    printf("std::sin    %7.7f %7.2f\n", rmse(stdsin), speed(stdsin));
    printf("taylor9     %7.7f %7.2f\n", rmse(taylorSinOrder9), speed(taylorSinOrder9));
    printf("taylor11    %7.7f %7.2f\n", rmse(taylorSinOrder11), speed(taylorSinOrder11));
    printf("pade55      %7.7f %7.2f\n", rmse(padesin_55), speed(padesin_55));
    printf("pade77      %7.7f %7.2f\n", rmse(padesin_77), speed(padesin_77));
    printf("fastersin   %7.7f %7.2f\n", rmse(fastersin), speed(fastersin));
    printf("chebyshev   %7.7f %7.2f\n", rmse(sinchebyshev), speed(sinchebyshev));
    printf("lookup1024  %7.7f %7.2f\n", rmse(LookupSin<1024>()), speed(LookupSin<1024>()));
    printf("lookup16384 %7.7f %7.2f\n", rmse(LookupSin<16384>()), speed(LookupSin<16384>()));
    //
    return 0;
}
