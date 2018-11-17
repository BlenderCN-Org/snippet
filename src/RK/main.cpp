/*
 Rabin–Karp algorithm
 文字列検索アルゴリズム
 同じ長さの検索文字列が大量にある場合にその文字列の量にほぼ関係なく全文検索ができる。
 代わりに一つの文字列の検索の速度は他の文字列検索アルゴリズムよりも大幅に劣る。
 https://en.wikipedia.org/wiki/Rabin%E2%80%93Karp_algorithm
 */
#include <cstdint>
#include <cstdio>
#include <vector>
#include <string>
#include <chrono>
#include <unordered_map>
#include <map>

// 総当たり
int32_t matchBF(const std::string& text, const std::vector<std::string>& targets)
{
    for(auto& target : targets)
    {
        const size_t textEnd = text.size()-target.size()+1;
        const size_t targetEnd = target.size();
        for(size_t i=0;i<textEnd;++i)
        {
            if([&]()
               {
                   for(size_t j=0;j<targetEnd;++j)
                   {
                       if( text[i+j] != target[j])
                       {
                           return false;
                       }
                   }
                   return true;
               }())
            {
                return i;
            }
        }
        //
    }
    return -1;
}

int32_t matchSTD(const std::string& text, const std::vector<std::string>& targets)
{
    for(auto& target : targets)
    {
        auto pos = text.find(target);
        if(pos != std::string::npos)
        {
            return pos;
        }
    }
    return std::string::npos;
}



//
int32_t matchRK(const std::string& text, const std::vector<std::string>& targets)
{
    const size_t P = 19;
    std::unordered_map<size_t,std::string> targetHashs;
    // 検索文字列のハッシュを計算する
    for(auto& target : targets)
    {
        size_t PP = 1;
        const int32_t len = int32_t(target.size());
        size_t targetHash = 0;
        for(int32_t i=len-1;i>=0;--i)
        {
            targetHash += int64_t(target[i])*PP;
            PP *= P;
        }
        targetHashs.insert({targetHash, target});
    }
    const int32_t targetLen = int32_t(targets[0].size());
    size_t PP = 1;
    for(int32_t i=0;i<targetLen;++i)
    {
        PP *= P;
    }
    // テキストの先頭のハッシュを求める
    size_t PP2 = 1;
    size_t textHash = 0;
    for(int32_t i=targetLen-1;i>=0;--i)
    {
        textHash += size_t(text[i])*PP2;
        PP2 *= P;
    }
    //
    const int32_t lenText = int32_t(text.size()) - int32_t(targetLen) + 1;
    int32_t i = 0;
    const char* textStr = text.c_str();
    for(i=0;i<lenText;++i)
    {
        // ハッシュが一致したら検索成功
        auto ite = targetHashs.find(textHash);
        if(ite != targetHashs.end())
        {
#if 1
            auto target = ite->second;
            // NOTE: 念のため文字列を比較
            if(text.compare(i, targetLen, target) == 0)
            {
                return i;
            }
#else
            return i;
#endif
        }
        textHash = P * textHash + (textStr[i+targetLen]) - PP * (textStr[i]);
    }
    return -1;
}

//
template<typename Fun>
double speed(Fun fun)
{
    const auto start = std::chrono::system_clock::now();
    for(int32_t i=0;i<128;++i)
    {
        volatile int f = fun();
    }
    const auto end = std::chrono::system_clock::now();
    return std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count();
}

//
int32_t main()
{
#if 0
    {
        const std::string text = "aabc_aabbcdbccefg";
        const std::string target = "cce";
        printf("%d\n", matchBF(text, {target}));
        printf("%d\n", matchRK(text, {target}));
    }
#endif
    
#if 1
    //
    {
        //
        const auto genAlphabet = []()
        {
            const int32_t d = 'Z' - 'A';
            const int32_t i = int32_t(double(rand())/double(RAND_MAX) * double(d));
            return char(i + 'A');
        };
        const auto genStr = [&](int32_t len)
        {
            std::string str;
            str.reserve(len);
            for(int32_t i=0;i<len;++i)
            {
                str += genAlphabet();
            }
            return str;
        };
        printf("TargetNum, BF,std,RK\n");
        for(int32_t targetNum = 2; targetNum < 2048; targetNum *= 2)
        {
            int32_t targetLen = 128;
            std::vector<std::string> targets;
            for(int32_t i=0;i<targetNum;++i)
            {
                auto target = genStr(targetLen);
                targets.push_back(target);
            }
            // 最後の検索文字列をテキストの末尾に追加
            auto text = genStr(1024*64) + targets[targets.size()-1];
#if 0
            printf("%d,%d,%d\n",
                   matchBF(text, targets),
                   matchSTD(text, targets),
                   matchRK(text, targets));
#endif
#if 1
            printf("%d, %f,%f,%f\n",
                   targetNum,
                   speed([&](){return matchBF(text, targets);}),
                   speed([&](){return matchSTD(text, targets);}),
                   speed([&](){return matchRK(text, targets);}));
#endif
        }
    }
#endif
    //
    return 0;
}
