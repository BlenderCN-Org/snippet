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


//
void quadProb()
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
            return x + 0.5f;
            //return x * x + 2.0f * x + 3.0f;
        };
        const float x = float(sn)/128.0f;
        samples.push_back({x, getSample(x)});
    }
    Eigen::MatrixXd A, b;
    A.resize(int32_t(samples.size()),2);
    b.resize(int32_t(samples.size()),1);
    for(int32_t sn=0;sn<samples.size();++sn)
    {
        auto& sample = samples[sn];
        A(sn,0) = sample.x;
        A(sn,1) = 1.0f;
        b(sn) = sample.y;
    }
    // 正規方程式
    auto At = A.transpose();
    auto beta = (At*A).inverse()*At*b;
    std::cout << beta << std::endl;
    
#if 0
    Eigen::Matrix<double, 6, 3> X;
    Eigen::Matrix<double, 6, 1> y;
    /*
     5.57143
     -0.0714286
     -0.928571
     */
    X.row(0) = Eigen::Vector3d({1, 2, 4});
    X.row(1) = Eigen::Vector3d({1, 2, 3});
    X.row(2) = Eigen::Vector3d({1, 3, 3});
    X.row(3) = Eigen::Vector3d({1, 4, 1});
    X.row(4) = Eigen::Vector3d({1, 4, 2});
    X.row(5) = Eigen::Vector3d({1, 5, 1});
    //
    y(0) = 2;
    y(1) = 3;
    y(2) = 1;
    y(3) = 4;
    y(4) = 5;
    y(5) = 4;
    //
    {
        Eigen::Matrix<double, 3, 6> Xt;
        Xt = X.transpose();
        Eigen::Matrix<double, 3, 1> beta;
        beta = (Xt*X).inverse()*Xt*y;
        std::cout << beta << std::endl;
    }
    {
        // TODO: solverを使う場合
    }
#endif
}

//
int32_t main()
{
    quadProb();
}
