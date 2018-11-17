/*
 TODOs
 - QR分解を行う
 - 特異値分解を行う
 - テストテスト
 - 放物線を出力する
 */
#include <cstdint>
#include <vector>

// 蜜行列
struct MatrixNxN
{
public:
    MatrixNxN(int32_t n, int32_t m)
    {
        resize(n,m);
    }
    //
    void resize(int32_t n, int32_t m)
    {
        e_ = std::vector<float>(n*m,0.0f);
    }
private:
    std::vector<float> e_;
};

//
int main()
{
    // TODO:
    
    return 0;
}
