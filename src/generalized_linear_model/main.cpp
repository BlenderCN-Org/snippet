#include <cstdio>
#include <cstdint>
#include <array>
#include <cmath>
#include <iostream>
//
//#include "../../thirdparty/eigen/Eigen/src/Cholesky/LDLT.h"
#include <Eigen/Core>
#include <Eigen/Dense>
#include <Eigen/SparseCore>
#include <Eigen/Sparse>

/*
ベルヌーイ分布: logit
二項分布: logit
ポワソン分布: log
ガンマ分布: log?
正規分布: identity
*/

// "統計思考の世界" 138p 付近
void main()
{
    struct Data
    {
    public:
        // 個体数
        int32_t num;
        // メス
        int32_t female;
    };
    std::array<Data, 8> datum =
    {
        1, 1,
        4, 3,
        10, 7,
        22, 18,
        55, 22,
        121, 41,
        210, 52,
        444, 79
    };
    //
    Eigen::MatrixXf A;
    Eigen::VectorXf b;
    A.resize(datum.size(), 2);
    b.resize(datum.size());
    //
    //for (auto& d : datum)
    for(int32_t sn=0;sn<datum.size();++sn)
    {
        const auto logit = [](int32_t num, int32_t total)
        {
            const float p = float(num)/float(total);
            const float tmp = p / (1.0f - p);
            return std::logf(tmp);
        };
        auto& d = datum[sn];
        const int32_t male = d.num - d.female;
        //
        if (male != 0)
        {
            const float l = logit(male, d.num);
            A(sn, 0) = 1.0f;
            A(sn, 1) = std::logf( float(d.num));
            b(sn, 0) = l;
        }
        // infの時はどう扱うのがいいのだろう？
        else
        {
            A(sn, 0) = 0.0f;
            A(sn, 1) = 0.0f;
            b(sn, 0) = 0.0f;
        }
    }
    //
    Eigen::LeastSquaresConjugateGradient<Eigen::MatrixXf> solver;
    solver.compute(A);
    auto beta = solver.solve(b);
    std::cout << beta << std::endl;
}