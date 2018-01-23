/*
http://postd.cc/probabilistic-data-structure-showdown-cuckoo/
TODO: HyperLogLog実装
 -> https://research.neustar.biz/2012/10/25/sketch-of-the-day-hyperloglog-cornerstone-of-a-big-data-infrastructure/
 -> https://en.wikipedia.org/wiki/HyperLogLog
TODO: カッコウフィルター実装
TODO: ブルームフィルター実装
TODO: LossyCounter実装

- Wikipediaの確率的データ構造の項
 -> https://en.wikipedia.org/wiki/Category:Probabilistic_data_structures
 -> Bloom filter
 -> Count–min sketch
 -> HyperLogLog
 -> Kinetic hanger
 -> Kinetic heater
 -> Locality-sensitive hashing
 -> MinHash
 -> Quotient filter
 -> Random binary tree
 -> Random tree
 -> Rapidly-exploring random tree
 -> SimHash
 -> Skip list
 -> Treap

- ブルームフィルター
 -> https://ja.wikipedia.org/wiki/%E3%83%96%E3%83%AB%E3%83%BC%E3%83%A0%E3%83%95%E3%82%A3%E3%83%AB%E3%82%BF
 -> 

- HyperLogLog
 -> https://research.neustar.biz/2012/10/25/sketch-of-the-day-hyperloglog-cornerstone-of-a-big-data-infrastructure/
 -> 
*/

#include <cstdio>
#include <cstdint>
#include <vector>
#include <array>
#include <bitset>
#include <functional>
#include <unordered_map>
#include <cstdint>
#include <string>
#include <set>
#include <algorithm>

template <class T>
inline void hash_combine(std::size_t& seed, const T& v)
{
    std::hash<T> hasher;
    seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

//
static uint32_t hash0(size_t v)
{
    return ((v+1)*2654435761) % (1<<31);
}

//
static uint32_t hash1(uint32_t x)
{
    x = ((x >> 16) ^ x) * 0x45d9f3b;
    x = ((x >> 16) ^ x) * 0x45d9f3b;
    x = (x >> 16) ^ x;
    return x;
}

//
uint32_t hash2(uint32_t x)
{
    x = ((x >> 16) ^ x) * 0x119de1f3;
    x = ((x >> 16) ^ x) * 0x119de1f3;
    x = (x >> 16) ^ x;
    return x;
}

/*
TODO: 普通の一つのハッシュテーブルの場合に比べて二つ以上取るメリットを知る
TODO: 限界性能はどうやって出せるのか知る -> Wikipediaに書かれているのでそれを見るようにする
TODO: 数値以外のハッシュでも試してみる
TODO: Bloomier filterを調べる
*/
class BloomFilter
{
public:
    void add(int32_t id)
    {
        for (int32_t i=0;i<K;++i)
        {
            const int32_t idx = hashId(id,i);
            bits[idx] = 1;
        }
    }
    bool maybeExist(int32_t id)
    {
        for (int32_t i = 0; i<K; ++i)
        {
            const int32_t idx = hashId(id, i);
            if (bits[idx] == 0)
            {
                return false;
            }
        }
        return true;
    }
private:
    int32_t hashId(int32_t id, int32_t kIndex)
    {
        size_t hashValue = 0;
        hash_combine(hashValue, id);
        hash_combine(hashValue, kIndex);
        return hashValue % M;
    }
private:
    enum { K = 4 };
    enum { M = 1024 * 8 };
    std::bitset<M> bits;
};

// BloomFilterのテスト
void testBloomFilter()
{
    // 
    BloomFilter bf;
    //
    const int32_t num = 1024;
    for (int32_t i = 0; i<num; i+=2)
    {
        bf.add(i);
    }
    //
    int32_t missCount = 0;
    for (int32_t i=0;i<num;++i)
    {
        const bool correct = ((i % 2 == 0) == bf.maybeExist(i));
        missCount += correct ? 0 : 1;
    }
    printf("Miss:%d\n", missCount);
}

/*
 cf. Using Linear Counting, LogLog, and HyperLogLog to Estimate Cardinality
     http://moderndescartes.com/essays/hyperloglog
 */
// Naiveな実装。O(N)の時間とO(N)のスペースが必要
size_t computeCardinality_Naive(const std::vector<size_t>& data)
{
    std::set<size_t> results;
    for(auto& d : data)
    {
        results.insert(d);
    }
    return results.size();
}

// Linear Countingの実装
// TODO: テーブルサイズについての適切な決め方を知る
size_t computeCardinality_LinerCounting(const std::vector<size_t>& data)
{
    const int32_t tableSize = 0xFFF;
    std::bitset<tableSize> bits;
    for(auto& d : data)
    {
        bits[std::hash<size_t>()(d) & tableSize] = true;
    }
    return bits.count();
}

// LogLogCounintgの実装
size_t computeCardinality_LogLog(const std::vector<size_t>& data)
{
    uint32_t maxLeadingZeros = 0;
    for(auto& d : data)
    {
        maxLeadingZeros = std::max(std::__clz(hash0(d)), maxLeadingZeros);
    }
    return 1 << maxLeadingZeros;
}

// LogLogCounting
size_t computeCardinality_LogLog_Improve(const std::vector<size_t>& data)
{
    uint32_t maxLeadingZeros = 0;
    for(auto& d : data)
    {
        struct Hashed
        {
            // バケットの選択
            uint32_t bucket : 5;
            // 実際のハッシュ値
            uint32_t hash : 27;
        };
        const uint32_t hashedRaw = hash0(d);
        const Hashed hashed = *reinterpret_cast<Hashed*>(&hashedRaw);
        hash_combine(
        hashed.bucket;
        std::__clz(hashed.hash);
        
        maxLeadingZeros0 = std::max(std::__clz(h0), maxLeadingZeros0);
        maxLeadingZeros1 = std::max(std::__clz(h1), maxLeadingZeros1);
        maxLeadingZeros2 = std::max(std::__clz(h2), maxLeadingZeros2);
    }
    return
    ((1 << maxLeadingZeros0) +
    (1 << maxLeadingZeros1) +
    (1 << maxLeadingZeros2))/3;
}

//
static size_t computeCardinality_HyperLogLog(const std::vector<size_t>& data)
{
    // TODO: 実装
    return 0;
}



// loglogCountingのテスト
void testLogLogCounting()
{
    // データセットを作成する
    std::vector<size_t> data;
    for(int32_t i=0;i<10000;++i)
    {
        data.push_back((i%1000)*3);
    }
    // 各種実装のテスト
    printf("Naive:    %lu\n", computeCardinality_Naive(data));
    printf("LinCount: %lu\n", computeCardinality_LinerCounting(data));
    printf("LogLog0:  %lu\n", computeCardinality_LogLog(data));
    printf("LogLog1:  %lu\n", computeCardinality_LogLog_Improve(data));
}

//
int32_t main()
{
    // testBloomFilter();
    testLogLogCounting();
}
