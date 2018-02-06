/*
- ABC
*/
#include <cstdint>
#include <cstdio>
//
#include <Eigen/Core>
#include <Eigen/LU>
#include <Eigen/SparseCore>
#include <Eigen/Sparse>

//
void quadProb()
{
    // 放物線に沿ったデータの作成
    const auto getSample = [](float x)
    {
        return x * x + 2.0f * x + 3.0f;
    };
    getSample(1.0f);
}

//
int32_t main()
{
    quadProb();
}
