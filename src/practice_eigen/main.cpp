#include <Eigen/Core>
#include <Eigen/LU>
#include <Eigen/SparseCore>
#include <Eigen/Sparse>
#include <iostream>
#include <array>

// http://cpp-kai.doorblog.jp/archives/2340422.html
void test0()
{
    // 
    {
        Eigen::Matrix<double, 6, 3> X;
        X <<
            1, 2, 4,
            1, 2, 3,
            1, 3, 3,
            1, 4, 1,
            1, 4, 2,
            1, 5, 1;
        Eigen::Matrix<double, 6, 1> y;
        y <<
            2,
            3,
            1,
            4,
            5,
            4;

        Eigen::Matrix<double, 3, 6> Xt;
        Xt = X.transpose();
        Eigen::Matrix<double, 3, 1> beta;
        beta = (Xt*X).inverse()*Xt*y;

        std::cout << beta << std::endl;
    }
    // SparseMatrixを使った場合
    {
        Eigen::SparseMatrix<double> X(6, 3);
        //
        X.insert(0, 0) = 1;
        X.insert(0, 1) = 2;
        X.insert(0, 2) = 4;
        //
        X.insert(1, 0) = 1;
        X.insert(1, 1) = 2;
        X.insert(1, 2) = 3;
        //
        X.insert(2, 0) = 1;
        X.insert(2, 1) = 3;
        X.insert(2, 2) = 3;
        //
        X.insert(3, 0) = 1;
        X.insert(3, 1) = 4;
        X.insert(3, 2) = 1;
        //
        X.insert(4, 0) = 1;
        X.insert(4, 1) = 4;
        X.insert(4, 2) = 2;
        //
        X.insert(5, 0) = 1;
        X.insert(5, 1) = 5;
        X.insert(5, 2) = 1;
        //
        X.finalize();
        //

        Eigen::SparseMatrix<double> y(6, 1);
        y.insert(0, 0) = 2;
        y.insert(1, 0) = 3;
        y.insert(2, 0) = 1;
        y.insert(3, 0) = 4;
        y.insert(4, 0) = 5;
        y.insert(5, 0) = 4;
        y.finalize();

        //
#if 0
        Eigen::Matrix<double, 3, 6> Xt;
        Xt = X.transpose();
        Eigen::Matrix<double, 3, 1> beta;
        beta = (Xt*X).inverse()*Xt*y;

        auto hoge = (Xt*X).inverse();
        const int32_t ccc = hoge.cols();
        const int32_t rrr = hoge.rows();
#else
        const auto Xt = X.transpose();
        auto XXt = Xt * X;
        Eigen::SimplicialLLT<Eigen::SparseMatrix<double> > solver;
        solver.compute(XXt);
        Eigen::SparseMatrix<double> I(3, 3);
        I.setIdentity();
        auto invXXt = solver.solve(I);
        const int32_t ccc = invXXt.cols();
        const int32_t rrr = invXXt.rows();
        auto beta = invXXt*Xt*y;
#endif
        /*
        5.57143
        -0.0714286
        -0.928571
        */
        puts("");
        std::cout << beta << std::endl;
    }
}

/*
2つの変数から重回帰
cf アイスクリーム屋さんで学ぶ 楽しい統計学
http://kogolab.chillout.jp/elearn/icecream/chap6/sec0.html
*/
void test1()
{
    Eigen::Matrix<double, 20, 3> X;
    X <<
        1, 33, 22,
        1, 33, 26,
        1, 34, 27,
        1, 34, 28,
        1, 35, 28,
        1, 35, 27,
        1, 34, 28,
        1, 32, 25,
        1, 28, 24,
        1, 35, 24,
        1, 33, 26,
        1, 28, 25,
        1, 32, 23,
        1, 25, 22,
        1, 28, 21,
        1, 30, 23,
        1, 29, 23,
        1, 32, 25,
        1, 34, 26,
        1, 35, 27;
    Eigen::Matrix<double, 20, 1> y;
    y <<
        382,
        324,
        338,
        317,
        341,
        360,
        339,
        329,
        218,
        402,
        342,
        205,
        368,
        196,
        304,
        294,
        275,
        336,
        384,
        368;

    Eigen::Matrix<double, 3, 20> Xt;
    Xt = X.transpose();
    Eigen::Matrix<double, 3, 1> beta;
    beta = (Xt*X).inverse()*Xt*y;
    puts("");
    /*
    -90.6405
    25.9568
    -16.7031
    */
    std::cout << beta << std::endl;
}

//
int main()
{
    test0();
    //test1();
    return 0;
}
