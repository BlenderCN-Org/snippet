/*
 - ABC
 */
#include <cstdint>
#include <cstdio>
#include <iostream>
#include <vector>
//
#include <Eigen/Core>
#include <Eigen/LU>
#include <Eigen/SparseCore>
#include <Eigen/Sparse>

/*
二次方程式も解けることの確認
*/
int32_t main()
{
    struct Sample
    {
        float x;
        float y;
    };
    std::vector<Sample> samples;
    
    for(int32_t sn=0;sn<128;++sn)
    {
        // 放物線に沿ったデータの作成
        const auto getSample = [](float x)
        {
            //return x + 0.5f;
            return x * x + 1.5f * x + 2.7f;
        };
        const float x = float(sn)/128.0f;
        samples.push_back({x, getSample(x)});
    }
    // col majorであることに注意
    Eigen::MatrixXd A, b;
    A.resize(int32_t(samples.size()),3);
    b.resize(int32_t(samples.size()),1);
    for(int32_t sn=0;sn<samples.size();++sn)
    {
        auto& sample = samples[sn];
        const float x = sample.x;
        const float y = sample.y;
        A(sn,0) = x * x;
        A(sn,1) = x;
        A(sn,2) = 1.0f;
        b(sn) = y;
    }
    // 直接正規方程式を使う
    auto At = A.transpose();
    auto beta = (At*A).inverse()*At*b;
    std::cout << beta << std::endl;
}
