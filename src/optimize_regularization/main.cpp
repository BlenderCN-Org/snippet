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

template<typename M, typename V, typename P>
M ridge(const M& A, const V& y, P alpha) {
    const auto& svd = A.jacobiSvd(Eigen::ComputeFullU | Eigen::ComputeFullV);
    const auto& s = svd.singularValues();
    const auto r = s.rows();
    const auto& D = s.cwiseQuotient((s.array().square() + alpha).matrix()).asDiagonal();
    return svd.matrixV().leftCols(r) * D * svd.matrixU().transpose().topRows(r) * y;
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
        std::normal_distribution<> dist(0.0f, 0.4f);
        const float x2 = x * x;
        const float x4 = x2 * x2;
        return 0.2f*x4  + x - 1.0f + dist(eng);
    };
    const int32_t numSample = 16;

    Eigen::MatrixXf A;
    Eigen::VectorXf b;
    A.resize(numSample,9);
    b.resize(numSample);

    float minY = std::numeric_limits<float>::infinity();
    float maxY = std::numeric_limits<float>::lowest();

    // 最適化でL2最適化をする
    for (int32_t sn=0;sn<numSample;++sn)
    {
        std::uniform_real_distribution<float> dist(0.0f, 1.0f);
        const float x = dist(eng);
        const float y = fun(x);
        const float x2 = x * x;
        const float x4 = x2 * x2;
        A(sn, 0) = 1.0f;
        A(sn, 1) = x;
        A(sn, 2) = x2;
        A(sn, 3) = x2 * x;
        A(sn, 4) = x4;
        b(sn) = y;

        minY = std::min(minY, y);
        maxY = std::max(maxY, y);
        // プロットする
        fprintf(gp, "set label %d point pt 7 ps 1 lc rgb \"light-red\" at %f, %f\n", sn+1, x, y);
    }

    fprintf(gp, "set xrange[0:1]\n");
    fprintf(gp, "set yrange[%f:%f]\n", minY, maxY);

    // 普通に解く
    {
        Eigen::LeastSquaresConjugateGradient<Eigen::MatrixXf> solver;
        solver.compute(A);
        auto beta = solver.solve(b);
        std::cout << beta << std::endl;
        // プロットする
        fprintf(gp, "plot %f + %f*x**1 + %f*x**2 + %f*x**3 + %f*x**4 + %f*x**5 title \"LeastSquare\"\n", beta(0), beta(1), beta(2), beta(3), beta(4), beta(5));
    }
    // L2正則化
    {
        for (float alpha = 0.2f; alpha <= 1.0f; alpha += 0.2f)
        {
            auto beta = ridge(A, b, alpha);
            std::cout << beta << std::endl;
            // プロットする
            fprintf(gp, "replot %f + %f*x**1 + %f*x**2 + %f*x**3 + %f*x**4 + %f*x**5 title \"Ridge regression(%2.1f)\"\n", beta(0), beta(1), beta(2), beta(3), beta(4), beta(5), alpha);
        }
    }
}
