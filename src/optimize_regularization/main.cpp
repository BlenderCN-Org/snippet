//
#include <Eigen/Core>
#include <Eigen/Dense>
#include <Eigen/LU>
#include <Eigen/SparseCore>
#include <Eigen/Sparse>
//
#include <cstdio>
#include <cstdint>
#include <random>
#include <iostream>
#include <array>

class Gnuplot
{
public:
    Gnuplot()
    {
        file_ = _popen("\"C:/gnuplot/bin/gnuplot.exe\" -persist", "w");
    }
    ~Gnuplot()
    {
        fflush(file_);
        _pclose(file_);
    }
    operator FILE*()
    {
        return file_;
    }
private:
    FILE * file_ = nullptr;
};

/*
Tikhonov regularization, ridge regression
https://en.wikipedia.org/wiki/Tikhonov_regularization
*/
Eigen::MatrixXf ridgeRegression(const Eigen::MatrixXf& A, const Eigen::VectorXf& b, float alpha)
{
    /*
    最終的には
    $$(A^{T} A + \alpha^2 I ) \hat{x} = A^{T}b$$
    をxについて解けばよい。
    ① そのまま $$A' \hat{x} = b'$$ の形として見なして解く
    ➁ SDVした結果のV行列の左は$$A^{T}A$$の固有値、U行列の左は$$A A^{T}$$の固有値であることを使って式変形
    */
#if 0
    //
    auto I = Eigen::MatrixXf::Identity(A.cols(), A.cols());
    auto Ad = (A.transpose() * A + alpha * alpha * I);
    auto bd = A.transpose() * b;
    return Ad.jacobiSvd(Eigen::ComputeThinU | Eigen::ComputeThinV).solve(bd);
#else
    const auto& svd = A.jacobiSvd(Eigen::ComputeFullU | Eigen::ComputeFullV);
    const auto& s = svd.singularValues();
    const auto r = s.rows();
    const auto& D = s.cwiseQuotient((s.array().square() + alpha * alpha).matrix()).asDiagonal();
    return svd.matrixV().leftCols(r) * D * svd.matrixU().transpose().topRows(r) * b;
#endif
}

//
void main()
{
    
    Gnuplot gp;
    //
    std::mt19937 eng(0x123);
    //
    const auto fun = [&eng](float x)
    {
#if 1
        const float x2 = x * x;
        const float x3 = x * x * x;
        const float x4 = x2 * x2;
        return /*-0.25f*x4 + x3  - */x2 - x - 1.0f;
#else
        return x;
#endif
    };
    const int32_t numSample = 16;

    Eigen::MatrixXf A;
    Eigen::VectorXf b;
    A.resize(numSample,10);
    b.resize(numSample);

    float minY = std::numeric_limits<float>::infinity();
    float maxY = std::numeric_limits<float>::lowest();

    std::normal_distribution<> dist(0.0f, 0.2f);
    // 最適化でL2最適化をする
    for (int32_t sn=0;sn<numSample;++sn)
    {
        std::uniform_real_distribution<float> dist(0.0f, 1.0f);
        std::uniform_real_distribution<float> distBig(0.0f, 10.0f);
        const float x = float(sn)/ float(numSample) * 3.0f;
        const float err = (sn==10) ? +10.0f/*: (sn == 3) ? +20.0f */: dist(eng);
        //const float err = dist(eng);
        const float y = fun(x) + err;
        const float x2 = x * x;
        const float x4 = x2 * x2;
        A(sn, 0) = 1.0f;
        A(sn, 1) = x;
        A(sn, 2) = x2;
        A(sn, 3) = x2 * x;
        A(sn, 4) = x4;
        A(sn, 5) = x4 * x;
        A(sn, 6) = x4 * x2;
        A(sn, 7) = x4 * x2 * x;
        A(sn, 8) = x4 * x4;
        //A(sn, 9) = x4 * x4 * x;
        b(sn) = y;

        minY = std::min(minY, y);
        maxY = std::max(maxY, y);
        // プロットする
        fprintf(gp, "set label %d point pt 7 ps 1 lc rgb \"light-red\" at %f, %f\n", sn+1, x, y);
    }

    fprintf(gp, "set xrange[0:3]\n");
    fprintf(gp, "set yrange[%f:%f]\n", minY-2.0f, maxY);


    // 凡例を右下に置く
    fprintf(gp, "set key right bottom\n");    

#if 1
    fprintf(gp, "unset key\n");
    fprintf(gp, "plot 1000+x\n");
    return ;
#endif
    fprintf(gp, "plot 1000+x\n");

#if 1
    // 普通に解く
    {
        Eigen::LeastSquaresConjugateGradient<Eigen::MatrixXf> solver;
        solver.compute(A);
        auto beta = solver.solve(b);
        //std::cout << beta << std::endl;
        // プロットする
        /*fprintf(gp, "plot %f + %f*x**1 + %f*x**2 + %f*x**3 + %f*x**4 + %f*x**5 + %f*x**6 + %f*x**7 + %f*x**8 + %f*x**9 title \"LeastSquare\"\n",
            beta(0), beta(1), beta(2), beta(3), beta(4), beta(5), beta(6), beta(7), beta(8), beta(9) );*/

#if 1
        fprintf(gp, "plot %f + %f*x**1 + %f*x**2 + %f*x**3 + %f*x**4 + %f*x**5 + %f*x**6 + %f*x**7 + %f*x**8 + %f*x**9 linewidth 2 title \"leastsquare\"\n",
            beta(0), beta(1), beta(2), beta(3), beta(4), beta(5), beta(6), beta(7), beta(8), beta(9));
#else
        fprintf(gp, "plot %f + %f*x**1 title \"leastsquare\"\n", beta(0), beta(1)/*, beta(2), beta(3), beta(4), beta(5)*/);
#endif
        //fprintf(gp, "plot %f + %f*x**1  title \"LeastSquare\"\n", beta(0), beta(1));
    }
#endif
    // L2正則化
    {
#if 0
        for (float alpha : std::vector<float>{/*1.0f,2.0f,4.0f,8.0f,*/16.0f/*,32.0f*/})
        {
            auto beta = ridgeRegression(A, b, alpha);
            puts("--------------");
            std::cout << beta << std::endl;
            // プロットする
            fprintf(gp, "replot %f + %f*x**1 + %f*x**2 + %f*x**3 + %f*x**4 + %f*x**5 + %f*x**6 + %f*x**7 + %f*x**8 + %f*x**9 linewidth 2 title \"ridge regression\"\n",
                beta(0), beta(1), beta(2), beta(3), beta(4), beta(5), beta(6), beta(7), beta(8), beta(9), alpha);
            //fprintf(gp, "replot %f + %f*x**1 title \"Ridge regression(%2.1f)\"\n", beta(0), beta(1), alpha);
        }
#endif
    }
}
