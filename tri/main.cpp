#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cassert>
#include <algorithm>

//
const float PI = 3.1415926535f;
/*
三辺の長さ(a,b,c)が与えられたときに、頂点から距離rの弧の合計
*/
float arcsum(float a, float b, float c, float r)
{
    if (r == 0.0f)
    {
        return 0.0f;
    }
    //
    assert(c < a + b);
    assert(b < a + c);
    assert(a < b + c);
    // 対辺までの距離の算出
    const float tmp = ((a*a) - (b*b) + (c*c)) / (2.0f*c);
    const float distToC = std::sqrtf((a * a) - (tmp*tmp));
    // 三つの内角を出す
    const float theta0 = std::acosf((a * a + b * b - c * c) / (2.0f * a * b));
    const float theta1 = std::acosf((a * a + c * c - b * b) / (2.0f * a * c));
    const float theta2 = std::acosf((b * b + c * c - a * a) / (2.0f * b * c));
    // 対辺以下の距離の場合
    if (r <= distToC)
    {
        // そのまま弧の長さ
        return theta0 * r;
    }
    // 弧が対辺と交わる場合
    else if ((r <= a) || (r <= b))
    {
        // 二つの弧を出す
        const float a0 = PI - (theta1 + (PI - std::asinf(std::sinf(theta1) * a / r)));
        const float a1 = PI - (theta2 + (PI - std::asinf(std::sinf(theta2) * b / r)));
        //printf("%f, %f\n", a0, a1);
        // TODO: 実際の交点は一つしかない場合に対応する
        return (std::max(a0,0.0f) + std::max(a1,0.0f)) * r;
    }
    // 弧が▲の外の場合
    else
    {
        return 0.0f;
    }
}

// 
void main()
{
#if 1
    const int32_t numSample = 1024;
    float sum = 0.0f;
    for (int32_t i=0;i<numSample;++i)
    {
        const float r = float(i) * std::sqrtf(3.0f) / float(numSample);
        const float len = arcsum(1.0f, 1.0f, std::sqrtf(2.0f), r);
        //const float len = arcsum(1.0f, std::sqrtf(3.0f), 2.0f, r);
        sum += len;
        //printf("%f\n",len);
    }
    printf("%f\n", sum/float(numSample));
#else
    arcsum(1.0f, 1.0f, std::sqrtf(2.0f), 0.9f);
#endif
}
