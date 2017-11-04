/*
 CS174 Combinatorics and Discrete Probability, John Canny
 https://people.eecs.berkeley.edu/~jfc/cs174/
 */

// TODO: マルコフバウンド
// TODO: チェビシェフバウンド
// TODO: 乱択選択問題

#include <cstdio>
#include <cstdint>
#include <random>

//
class FloatStreamStats
{
public:
    FloatStreamStats() = default;
    //
    virtual void add(float v)
    {
        //
        n_ += 1.0f;
        //
        const float newMean = (v - mean_) / n_ + mean_;
        const float newM = (v - mean_) * (v - newMean) + M_;
        //
        M_ = newM;
        mean_ = newMean;
        //
        min_ = std::min(v, min_);
        max_ = std::max(v, max_);
    }
    float mean() const
    {
        return mean_;
    }
    float variance() const
    {
        return M_ / (n_ - 1.0f);
    }
protected:
    float mean_ = 0.0f;
    float M_ = 0.0f;
    float n_ = 0.0f;
    float min_ = std::numeric_limits<float>::max();
    float max_ = std::numeric_limits<float>::min();
};

//
void lecture6_markov_bound()
{
    // TODO: Markov bound
    //
    std::random_device rd;
    std::mt19937 rng(rd());
    FloatStreamStats fs;
    //
    std::normal_distribution<> dist(0.0f, 1.0f);
    for(int32_t i=0;i<512;++i)
    {
        // 非負はいけないことに注意
        const float v = std::min(dist(rng), 0.0);
        fs.add(v);
    }
    
    // TODO: なんらかの分布に従うように生成する
    printf("DONE¥n");
}

int main()
{
    printf("fff\n");
    puts("FUCK");
    printf("ggg\n");
    fflush(stdout);
    return 0;
}
