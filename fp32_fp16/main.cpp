/*
https://aras-p.info/blog/2007/03/03/a-day-well-spent-encoding-floats-to-rgba/
https://aras-p.info/blog/2009/07/30/encoding-floats-to-rgba-the-final/
FP32‚©‚çunorm‚ÌRGBA‚Ö‚Ì•ÏŠ·
TODO: ‰½‚©ƒoƒO‚Á‚Ä‚¢‚é‚Ì‚©“¯‚¶Œ‹‰Ê‚É‚È‚é
*/

#include <cstdio>
#include <cmath>
#include <cstdint>

struct float4
{
public:
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;
public:
    float4() = default;
    float4(uint8_t ir, uint8_t ig, uint8_t ib, uint8_t ia)
    {
        r = ir;
        g = ig;
        b = ib;
        a = ia;
    }
    float4(float fr, float fg, float fb, float fa)
    {
        setF(fr, 0);
        setF(fg, 1);
        setF(fb, 2);
        setF(fa, 3);
    }
    float getF(int32_t idx) const
    {
        return (float(*(&r + idx)) - 0.5f) / 255.0f;
    }
    void setF(float v, int32_t idx)
    {
        const uint8_t vi = uint8_t(v * 255.0f + 0.5f);
        (*(&r + idx)) = vi;
    }
    float4 yzww() const
    {
        return float4(g,b,a,a);
    }
};

static float4 operator * (const float4& f4, float v)
{
    return float4(
        f4.getF(0) * v,
        f4.getF(1) * v,
        f4.getF(2) * v,
        f4.getF(3) * v);
}

static float4 operator * (const float4& rhs, const float4& lhs)
{
    return float4(
        lhs.getF(0) * rhs.getF(0),
        lhs.getF(1) * rhs.getF(1),
        lhs.getF(2) * rhs.getF(2),
        lhs.getF(3) * rhs.getF(3));
}

static void operator -= (float4& rhs, const float4& lhs)
{
    rhs.setF(rhs.getF(0) - lhs.getF(0), 0);
    rhs.setF(rhs.getF(1) - lhs.getF(1), 1);
    rhs.setF(rhs.getF(2) - lhs.getF(2), 2);
    rhs.setF(rhs.getF(3) - lhs.getF(3), 3);
}

static float4 frac(const float4& f4)
{
    float4 ret;
    ret.setF(std::fmodf(f4.getF(0), 1.0f), 0);
    ret.setF(std::fmodf(f4.getF(1), 1.0f), 1);
    ret.setF(std::fmodf(f4.getF(2), 1.0f), 2);
    ret.setF(std::fmodf(f4.getF(3), 1.0f), 3);
    return ret;
}

static float dot(const float4& lhs, const float4& rhs)
{
    return 
        lhs.getF(0) * rhs.getF(0) +
        lhs.getF(1) * rhs.getF(1) +
        lhs.getF(2) * rhs.getF(2) +
        lhs.getF(3) * rhs.getF(3);
}

namespace Old
{
    inline float4 EncodeFloatRGBA(float v) {
        return frac(float4(1.0f, 256.0f, 65536.0f, 16777216.0f) * v);
    }
    inline float DecodeFloatRGBA(float4 rgba) {
        return dot(rgba, float4(1.0f, 1.0f / 256.0f, 1.0f / 65536.0f, 1.0f / 16777216.0f));
    }
}

namespace New
{
    inline float4 EncodeFloatRGBA(float v) {
        float4 enc = float4(1.0f, 255.0f, 65025.0f, 160581375.0f) * v;
        enc = frac(enc);
        enc -= enc.yzww() * float4(1.0f / 255.0f, 1.0f / 255.0f, 1.0f / 255.0f, 0.0f);
        return enc;
    }
    inline float DecodeFloatRGBA(float4 rgba) {
        return dot(rgba, float4(1.0f, 1.0f / 255.0f, 1.0f / 65025.0f, 1.0f / 160581375.0f));
    }
}

void main()
{
    const int32_t N = 200;
    for (int32_t i=0;i<N;++i)
    {
        const float v = float(i)/ float(N);
        const float ov = Old::DecodeFloatRGBA(Old::EncodeFloatRGBA(v));
        const float nv = New::DecodeFloatRGBA(New::EncodeFloatRGBA(v));
        printf("%f,%f,%f\n", v, std::fabsf(ov-v), std::fabsf(nv-v));
    }
}
