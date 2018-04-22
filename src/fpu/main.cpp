/*
 ソフトウェアで浮動小数演算をする
 参考: https://github.com/ProcessorCompilerExperiment2014-Team0/FPU/
 */
#include <cassert>
#include <cstdint>
#include <cstdio>

//
struct F32
{
public:
    uint32_t sign : 1;
    uint32_t exp  : 8;
    uint32_t frac : 23;
};
static_assert(sizeof(F32)==sizeof(int32_t), "");

//
F32 setF(float v)
{
    F32 f32;
    *reinterpret_cast<float*>(&f32) = v;
    return f32;
}

//
float convF(F32 f32)
{
    return *reinterpret_cast<float*>(&f32);
}

//
int32_t main()
{
    {
        const F32 f0 = setF(1.0f);
        const F32 f1 = setF(2.0f);
        const F32 f2 = fadd(f0,f1);
    }
    return 0;
}
