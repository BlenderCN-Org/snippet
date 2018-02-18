/*
 Rabin–Karp algorithm
 高速な文字列検索
 https://en.wikipedia.org/wiki/Rabin%E2%80%93Karp_algorithm
 */
#include <cstdint>
#include <cstdio>
#include <vector>
#include <string>
#include <chrono>

// 総当たり
int32_t matchBF(const std::string& text, const std::string& target)
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
    return -1;
}

int32_t matchSTD(const std::string& text, const std::string& target)
{
    return text.find(target);
}

//
int32_t matchRK(const std::string& text, const std::string& target)
{
    const size_t P = 19;
    // 検索文字列のハッシュを計算する
    size_t PP = 1;
    const int32_t len = int32_t(target.size());
    size_t targetHash = 0;
    for(int32_t i=len-1;i>=0;--i)
    {
        targetHash += int64_t(target[i])*PP;
        PP *= P;
    }
    // テキストの先頭のハッシュを求める
    size_t PP2 = 1;
    size_t textHash = 0;
    for(int32_t i=len-1;i>=0;--i)
    {
        textHash += size_t(text[i])*PP2;
        PP2 *= P;
    }
    //
    const int32_t lenText = int32_t(text.size()) - int32_t(target.size()) + 1;
    const int32_t targetSize = int32_t(target.size());
    int32_t i = 0;
    const char* textStr = text.c_str();
    for(i=0;i<lenText;++i)
    {
        // ハッシュが一致したら検索成功
        if(textHash == targetHash)
        {
#if 0
            // NOTE: 念のため文字列を比較
            if(text.compare(i, targetSize, target) == 0)
            {
                return i;
            }
#else
            return i;
#endif
        }
        textHash = P * textHash + (textStr[i+targetSize]) - PP * (textStr[i]);
    }
    return -1;
}

//
template<typename Fun>
double speed(Fun fun)
{
    const auto start = std::chrono::system_clock::now();
    for(int32_t i=0;i<1024*64;++i)
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
        printf("%d\n", matchBF(text, target));
        printf("%d\n", matchRK(text, target));
    }
#endif
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
        printf("TargetLen, BF,std,RK\n");
        for(int32_t targetLen = 4; targetLen < 256 ; targetLen *= 2)
        {
            auto target = genStr(targetLen);
            auto text = genStr(1024*16) + target;
#if 0
            printf("%d,%d,%d\n",
                   matchBF(text, target),
                   matchSTD(text, target),
                   matchRK(text, target));
#endif
#if 1
            printf("%d, %f,%f,%f\n",
                   targetLen,
                   speed([&](){return matchBF(text, target);}),
                   speed([&](){return matchSTD(text, target);}),
                   speed([&](){return matchRK(text, target);}));
#endif
        }
    }
    //
    return 0;
}
