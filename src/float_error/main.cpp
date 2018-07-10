#include <cstdint>
#include <cstdio>
#include <limits>

//
constexpr float boundGamma(int32_t n)
{
    const float eps = std::numeric_limits<float>::epsilon() * 0.5f;
    return (n * eps) / (1 - n * eps);
}


void test()
{
    //
    Vec3 e1, e2;  //Edge1, Edge2
    Vec3 P, Q, T;
    float det, inv_det;
    // v0を共有するエッジ
    e1 = v1 - v0;
    e2 = v2 - v0;
    // detとuの準備
    P = Vec3::cross(ray.d, e2);
    // ほぼ平行な場合かをチェック
    det = Vec3::dot(e1, P);
    if (det == 0.0f)
    {
        return false;
    }
    inv_det = 1.0f / det;
    // レイ原点からv0への距離
    T = ray.o - v0;
    // uを計算し、範囲内に収まっているかをチェック
    *u = Vec3::dot(T, P) * inv_det;
    if (*u < 0.0f || *u > 1.0f)
    {
        return false;
    }
    // vも同様の計算
    Q = Vec3::cross(T, e1);
    *v = Vec3::dot(ray.d, Q) * inv_det;
    if (*v < 0.0f || *u + *v  > 1.0f)
    {
        return false;
    }
    // tの範囲チェック
    *t = Vec3::dot(e2, Q) * inv_det;
    if (*t < ray.mint || ray.maxt < *t)
    {
        return false;
    }
    // 面の方向を返す
    *isFlip = (det < 0.0f);
}

//
int32_t main()
{
    // +
    {
        const int32_t a = 123;
        const int32_t b = 456;
        const float r = float(a) + float(b);
        const float err = boundGamma(1);
        printf("%d [%f.%f]\n", a + b, r*(1.0f - err), r*(1.0f + err));
    }
    // * 
    {
        const float a = 0.001f;
        const float b = 0.1f;
        const float c = 1.0f;
        const float d = 10.0f;
        const float r = float(a) + float(b) + float(c) + float(d);
        const float err = (a + b)*(boundGamma(3)) + c*(boundGamma(2)) + d* (boundGamma(1));
        printf("%f [%f,%f,%f]\n", a+b+c+d, r*(1.0f - err), r, r*(1.0f + err));
    }
    return 0;
}