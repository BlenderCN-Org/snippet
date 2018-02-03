/*
 TODOs
 - 最急降下法を実装する
 - 共役勾配法を実装する
 */

#include <cstdio>
#include <cmath>
#include <cstdint>
#include <functional>

float ternary_search(const std::function<float(float)>& fun,
                     float a, float b)
{
    while((b-a) > 0.001f)
    {
        // printf("[%f,%f]\n", a, b);
        if(b-a < 0.001f)
        {
            return (a+b)*0.5f;
        }
        //
        const float d = (b-a)/3.0f;
        float x1 = a + d;
        float x2 = b - d;
        const float v0 = fun(x1);
        const float v1 = fun(x2);
        // [a,x2]の範囲
        if(v0<v1)
        {
            a = a;
            b = x2;
        }
        // [x1,b]の範囲
        else
        {
            a = x1;
            b = b;
        }
    }
    return (a+b)*0.5f;
}

// 三分割法
void test11()
{
    //
    const auto objFun = [](float x) -> float
    {
        const float p = 0.3f;
        return (x-p)*(x-p);
    };
    printf("%f\n", ternary_search(objFun, -1.0f, 1.0f));
}

// p59の例題4.1
void test4_1()
{
    /*
     目的関数: f(x1,x2) = (x1-1)^4 + (x2-2)^2
     ∇f = (4(x1-1)^3, 2(x2-2))
     */
    struct Point
    {
    public:
        float x;
        float y;
    };
    const auto objFun = [](Point p)
    {
        return std::powf(p.x-1.0f,4.0f) + std::powf(p.x-2.0f,2.0f);
    };
    const auto nablaF = [](Point p) -> Point
    {
        return {4.0f*std::powf(p.x-1.0f,3.0f), 2.0f * (p.x-2.0f)};
    };
    // 最初の位置
    Point p = {0.0f,0.0f};
    // 最初の勾配方向
    Point nf = nablaF(p);
    // TODO: この直接上での最適化のための目的関数ができる
    // TODO: 1次元での探索を実装する
}

//
int32_t main()
{
    test11();
    // test4_1();
    return 0;
}
