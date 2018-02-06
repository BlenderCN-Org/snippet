/*
 TODOs
 - 最急降下法を実装する
 - 共役勾配法を実装する
 - 放物線補間を入れる
 */

#include <cstdio>
#include <cmath>
#include <cstdint>
#include <functional>

// 三分割法
float ternary_search(const std::function<float(float)>& fun,
                     float a, float b)
{
    int32_t depth = 0;
    //
    while((b-a) > 0.001f)
    {
        // printf("[%f,%f]\n", a, b);
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
        ++depth;
    }
    printf("NumEval:%d\n", depth * 2);
    return (a+b)*0.5f;
}

// 黄金分割
float golden_section_search(const std::function<float(float)>& fun,
                     float a, float b)
{
    float v0, v1;
    bool goLeft = false;
    int32_t depth = 0;
    //
    while((b-a) > 0.001f)
    {
        printf("[%f,%f]\n", a, b);
        const float gamma = 0.38196601125f; // (3-5^(0.5))/2
        const float d = (b-a) * gamma;
        float x1 = a + d;
        float x2 = b - d;
        // 初回は両方とも計算しておく
        if(depth == 0)
        {
            v0 = fun(x1);
            v1 = fun(x2);
        }
        // 二回目以降
        else
        {
            // 左に潜る場合
            if(goLeft)
            {
                v1 = v0;
                v0 = fun(x1);
            }
            // 右に潜る場合
            else
            {
                v0 = v1;
                v1 = fun(x2);
            }
        }
        // [a,x2]の範囲
        if(v0<v1)
        {
            a = a;
            b = x2;
            goLeft = true;
        }
        // [x1,b]の範囲
        else
        {
            a = x1;
            b = b;
            goLeft = false;
        }
        ++depth;
    }
    printf("NumEval:%d\n", (depth-1) + 2);
    return (a+b)*0.5f;
}

// 三分割法
void test_ch2()
{
    // 三分割法のテスト
#if 0
    printf("%f\n", ternary_search([](float x) -> float
                                  {
                                      return x*(x-2.0f)*(x+2.0f);
                                  }, -1.0f, 2.0f));
#endif
    
#if 0
    // 評価回数52
    printf("%f\n", ternary_search([](float x) -> float
                                         {
                                             return (x-10.0f) * (x-10.0f) + x;
                                         }, 0.0f, 27.0f));
#endif
    // 黄金分割のテスト
#if 0
    printf("%f\n", golden_section_search([](float x) -> float
                                  {
                                      return x*(x-2.0f)*(x+2.0f);
                                  }, -1.0f, 2.0f));
#endif
    
#if 1
    // 評価回数23
    printf("%f\n", golden_section_search([](float x) -> float
                                         {
                                             return (x-10.0f) * (x-10.0f) + x;
                                         }, 0.0f, 27.0f));
#endif
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
    //test_ch2();
    // test4_1();
    return 0;
}
