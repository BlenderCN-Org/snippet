/*
ネタは？
catch_rareについて
-> 普通に分散だすと検出力が落ちる?
-> どのように動作するか？
-> ture,false,false,false,falseで初めて発動したりするの？
-> CLTを使って判定するのではなく、小さい確率に関してはポワソン分布のほうが適している
-> 応用例として、分類にも使える。どのパスを選択したのかみたいな？
-> 応用例: 本当に半分で起こるか(コイントス、さいころ)
-> 応用例: 分布の割合の算出

参照
https://pharr.org/matt/blog/2018/05/31/check-rare.html

TODOs
- 乱数を作成する
- ちゃんと検出しているのを確認する
- TODO: サンプルプログラムを作成する
- TODO: プログラムを書きながら基本的な原理を理解する
- TODO: 応用例なども作ってみる
- TODO: サンプルが完全に動いたら原理を書いてみる
- TODO: 一回も起こらないことの区間推定は1/N
- TODO: 検出するまでのカウントで測る
*/

#include <cstdint>
#include <cstdio>
#include <random>

// true/falseを生成する
class Generator
{
public:
    Generator(float freq)
        :freq_(freq)
    {}
    bool gen()
    {
        std::uniform_real_distribution<float> dist(0.0f, 1.0f);
        return (dist(rng_) < freq_);
    }
private:
    std::mt19937 rng_;
    float freq_ = 0.0f;
};

// true,falseの確認
class Checker
{
public:
    //
    Checker(float maxFreq)
        :maxFreq_(maxFreq)
    {}
    //
    bool add(bool isTrue)
    {
        ++total_;
        numTrue_ += (isTrue ? 1 : 0);
        //
        const float freqEstimate = float(numTrue_) / float(total_);
        const float varEst = freqEstimate * (1 - freqEstimate) / float(total_);
        //
        return
            (freqEstimate - sigma_ * varEst >= maxFreq_);
    }
private:
    float maxFreq_ = 0.0f;
    float sigma_ = 2.0f;
    int32_t total_ = 0;
    int32_t numTrue_ = 0;
};

int32_t main()
{
    // 
    Generator generator(0.05f);
    Checker checker(0.05f);
    for (int32_t i=0;i<128;++i)
    {
        if (checker.add(generator.gen()))
        {
            printf("find\n");
            break;
        }
    }
}