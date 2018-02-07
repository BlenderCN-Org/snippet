/*
 - ABC
 */
#include <cstdint>
#include <cstdio>
#include <iostream>
#include <vector>
//
#include "eigen-qp.hpp"
//
#include <Eigen/Core>
#include <Eigen/Dense>
#include <Eigen/LU>
#include <Eigen/SparseCore>
#include <Eigen/Sparse>
#include <unsupported/Eigen/IterativeSolvers>


/*
 解きたい問題
 - パラメーター自体に制限がある
 - ペナルティ法もパラメーター自体に制限がある...? がどうやってサンプルにないところをペナルティとして指定するのか？
 */

/*
 二次方程式も解けることの確認
 */
int32_t main()
{
        //test<double,PROB_VARS,PROB_INEQ,PROB_EQ>();
    
    //return 0;
    struct Sample
    {
        float x;
        float y;
    };
    std::vector<Sample> samples;
    
    const int32_t numSample = 128;
    for(int32_t sn=0;sn<numSample;++sn)
    {
        // 放物線に沿ったデータの作成
        const auto getSample = [](float x)
        {
            return 1.1f * x * x + 0.2f * x + 0.3f;
            //return x * x;
            //auto R = solver.matrixR();
            //return x * x + 1.5f * x + 2.7f;
        };
        const float x = float(sn)/float(numSample);
        samples.push_back({x, getSample(x)});
    }
    // col majorであることに注意
    Eigen::VectorXd b;
    Eigen::SparseMatrix<double> A;
    A.resize(int32_t(samples.size()),3);
    b.resize(int32_t(samples.size()));
    for(int32_t sn=0;sn<samples.size();++sn)
    {
        auto& sample = samples[sn];
        const float x = sample.x;
        const float y = sample.y;
        A.insert(sn,0) = x * x;
        A.insert(sn,1) = x;
        A.insert(sn,2) = 1.0f;
        b(sn) = y;
    }
    A.makeCompressed();
    
#if 0
    {
        // 直接正規方程式を使う
        auto At = A.transpose();
        auto beta = (At*A).inverse()*At*b;
        std::cout << beta << std::endl;
    }
    {
        // 特異値分解で解く
        std::cout << A.jacobiSvd(Eigen::ComputeThinU | Eigen::ComputeThinV).solve(b) << std::endl;
    }
#endif
    //
#if 0
    {
        Eigen::LeastSquaresConjugateGradient<Eigen::SparseMatrix<double>> solver;
        solver.compute(A);
        auto beta = solver.solve(b);
        /*
         1
         1.5
         2.7
         */
        std::cout << beta << std::endl;
    }
#endif
    
#if 0
    {
        Eigen::SparseQR<Eigen::SparseMatrix<double>, Eigen::COLAMDOrdering<int32_t>> solver;
        //Eigen::HouseholderQR<Eigen::SparseMatrix<double>> hoge;
        solver.compute(A);
        auto beta = solver.solve(b);
        std::cout << beta << std::endl;
    }
#endif
    
#if 1
    {
        // https://scicomp.stackexchange.com/a/11588
        Eigen::MatrixXd Q = A.transpose() * A;
        Eigen::VectorXd c = -A.transpose() * b;
        Eigen::MatrixXd A2;
        Eigen::VectorXd b2;
        const int32_t t = 3;
        A2.resize(t,t);
        b2.resize(t);
        A2.setIdentity();
        b2.setOnes();
        b2 << 1.0/10.0, 1.0/10.0, 1.0/10.0;
        //
        Eigen::VectorXd x;
        x.resize(t);
        EigenQP::quadprog(Q, c, A2, b2, x);
        std::cout << x << std::endl;
        //
        std::cout << "Rest" << std::endl;
        std::cout << A2 * x  << std::endl;
#if 0
        template<typename Scalar, int NVars, int NIneq>
        void quadprog(Eigen::Matrix<Scalar,NVars,NVars> &Q, Eigen::Matrix<Scalar,NVars,1> &c,
                      Eigen::Matrix<Scalar,NIneq,NVars> &A, Eigen::Matrix<Scalar,NIneq,1> &b,
                      Eigen::Matrix<Scalar,NVars,1> &x)
#endif

    }
#endif
}
