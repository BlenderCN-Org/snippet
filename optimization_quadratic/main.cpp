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

#include <Eigen/Eigenvalues>
#include <iostream>

#include "eigen-qp.hpp"

using namespace std;
using namespace Eigen;
using namespace EigenQP;
/*
 *  min 0.5 x.Q.x + c.x
 *
 *  Ax <= b
 *  Ex = d
 *
 * See: http://etd.dtu.dk/thesis/220437/ep08_19.pdf
 */

#define PROB_VARS 16
#define PROB_INEQ 8
#define PROB_EQ 3

template<typename MatrixT>
void checkPosDef(MatrixT& A)
{
    //Eigen::MatrixXd A(2, 2);
    //A << 1, 0 , 0, -1; // non semi-positive definitie matrix
    //std::cout << "The matrix A is" << std::endl << A << std::endl;
    Eigen::LLT<Eigen::MatrixXd> lltOfA(A); // compute the Cholesky decomposition of A
    const bool isPosDef = lltOfA.info() != Eigen::NumericalIssue;
    auto mes = isPosDef ? "Positive" : "Negative";
    std::cout << mes << std::endl;
}

template<typename Scalar, int NV_FIXED, int NC_FIXED, int NE_FIXED>
void test()
{
    
    
    typedef Eigen::Matrix<Scalar,-1,-1> MatrixXs;
    typedef Eigen::Matrix<Scalar,-1,1> VectorXs;
    
    // Make a random problem
    int num_vars = NV_FIXED;
    int num_ineq = NC_FIXED;
    
    // Random matrices
    MatrixXs Q = MatrixXs::Random(num_vars,num_vars);
    Q *= Q.adjoint()/sqrt(num_vars); // Make it pos def
    
    //checkPosDef(Q);
    
    VectorXs c = VectorXs::Random(num_vars);
    
    MatrixXs A = MatrixXs::Random(num_ineq,num_vars);
    VectorXs b = VectorXs::Random(num_ineq);
    
    // Solve unconstrainted system
    cout << "Unconstrained..." << endl;
    VectorXs x_unc = -Q.ldlt().solve(c);
    // これはほぼ０
    //cout << (Q * -x_unc - c).norm() << endl;
    
    std::cout << x_unc << std::endl;
    VectorXs x(num_vars);
    // Generate inequality constraints
    b.array() = (A*x_unc).array() - 0.15;
    cout << "constraint..." << endl;
    std::cout << b << std::endl;
    // Inequality constrained problem
    cout << "quadprog, dynamic code" << endl;
    quadprog(Q,c,A,b,x);
    std::cout << x << std::endl;
    cout << "    error: " << (x - x_unc).norm() << endl;
}

/*
 二次方程式も解けることの確認
 */
int32_t main()
{
#if 0
    test<double,PROB_VARS,PROB_INEQ,PROB_EQ>();
    return 0;
#endif
    
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
        b2(0)  = 1/3.0f;
        b2(1)  = 1/3.0f;
        b2(2)  = 1/3.0f;
        //
        Eigen::VectorXd x;
        x.resize(t);
        EigenQP::quadprog(Q, c, A2, b2, x);
        std::cout << x << std::endl;
        //
        std::cout << "Rest" << std::endl;
        std::cout << A2 * x  << std::endl;
        
        //checkPosDef(Q);
#if 0
        template<typename Scalar, int NVars, int NIneq>
        void quadprog(Eigen::Matrix<Scalar,NVars,NVars> &Q, Eigen::Matrix<Scalar,NVars,1> &c,
                      Eigen::Matrix<Scalar,NIneq,NVars> &A, Eigen::Matrix<Scalar,NIneq,1> &b,
                      Eigen::Matrix<Scalar,NVars,1> &x)
#endif

    }
#endif
}
