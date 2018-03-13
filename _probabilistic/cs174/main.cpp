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
#include <array>

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

/*
 Markov bound
 Pr[Y≥t] ≤ E[Y] / t
 の確認
 */
void lecture6_markov_bound()
{
    //
    std::random_device rd;
    std::mt19937 rng(rd());
    FloatStreamStats fs;
    //
    const float mu = 5.0f;
    const float sd = 1.0f;
    std::array<int,10> Count; // Pr[Y≥t]となったカウント
    std::fill(Count.begin(),Count.end(),0);
    std::normal_distribution<> dist(mu, sd);
    const int32_t numSample = 512;
    for(int32_t i=0;i<numSample;++i)
    {
        // 非負はいけないことに注意
        const float v = std::max(dist(rng), 0.0);
        fs.add(v);
        for(int32_t j=1;j<Count.size()+1;++j)
        {
            if(float(j) < v)
            {
                ++Count[j];
            }
        }
    }
    //
    printf("E[X]=%f\n", fs.mean());
    printf("        Pr[Y≥t] ≤ E[Y]/t \n");
    for(int32_t i=0;i<10;++i)
    {
        const float t = (mu)/10.0f * float(i);
        const float Pr = float(Count[i])/float(numSample);
        printf("t:%3.3f  %3.3f  ≤  %3.3f\n", t, Pr, fs.mean()/t);
    }
}

int main()
{
    lecture6_markov_bound();
    return 0;
}
