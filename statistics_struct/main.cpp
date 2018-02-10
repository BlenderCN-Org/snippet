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
#include <numeric>

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

#if 1
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
#endif

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

/*
 http://moderndescartes.com/essays/hyperloglog
 http://algo.inria.fr/flajolet/Publications/DuFl03.pdf
 LogLogCounting
 */
float computeCardinality_LogLog(const std::vector<size_t>& data)
{
    std::array<int32_t,32> buckets;
    buckets.fill(0);
    //
    for(auto& d : data)
    {
        struct Hashed
        {
            // バケットの選択
            uint32_t bucket;
            // 実際のハッシュ値
            uint32_t hash;
            Hashed(uint32_t v)
            {
                bucket = (v & 0x1F);
                hash = (v>>5);
            }
        };
        const Hashed hashed = Hashed(hash1(d));
        int32_t& b =buckets[hashed.bucket];
        int32_t lzb = std::__clz(hashed.hash);
        b = std::max(b,lzb);
    }
    const float ave = float(std::accumulate(buckets.begin(), buckets.end(), 0))/float(buckets.size());
    const float alpha = 0.79f;
    return std::powf(2.0f,ave) * alpha;
}

/*
 HyperLogLog
 http://moderndescartes.com/essays/hyperloglog
 https://research.neustar.biz/2012/10/25/sketch-of-the-day-hyperloglog-cornerstone-of-a-big-data-infrastructure/
 https://research.neustar.biz/2013/04/02/sketch-of-the-day-probabilistic-counting-with-stochastic-averaging-pcsa/
 */
static float computeCardinality_HyperLogLog(const std::vector<size_t>& data)
{
    std::array<int32_t,32> buckets;
    buckets.fill(0);
    //
    for(auto& d : data)
    {
        struct Hashed
        {
            // バケットの選択
            uint32_t bucket;
            // 実際のハッシュ値
            uint32_t hash;
            Hashed(uint32_t v)
            {
                bucket = (v & 0x1F);
                hash = (v>>5);
            }
        };
        const Hashed hashed = Hashed(hash1(d));
        int32_t& b =buckets[hashed.bucket];
        int32_t lzb = std::__clz(hashed.hash);
        b = std::max(b,lzb);
    }
    // 調和平均を取るようにする
    float hm = 0.0f;
    int32_t zeroBucketCount = 0;
    for(auto b: buckets)
    {
        if(b == 0)
        {
            ++zeroBucketCount;
        }
        else
        {
            hm += 1.0f/b;
        }
    }
    hm = 1.0f / hm;
    hm *= float(buckets.size());
    const float alpha = 0.79f;
    //
    const float E0 = std::powf(2.0f,hm) * alpha;
    // 異常に項目数が少ない場合
    if((E0 < 2.5f * 32.0f) && (zeroBucketCount != 0) )
    {
        const float E1 = -32.0f * std::log(float(zeroBucketCount)/32.0f);
        return E1;
    }
    // 異常に項目数が多い場合
    else if( E0 > std::powf(2.0f,32.0f)/30.0f)
    {
        const float H = std::powf(2.0f,32.0f);
        const float E2 = - H * std::log(1 - E0 / H);
        return E2;
    }
    // 通常の項目数の場合
    else
    {
        return E0;
    }
}

/*
 loglog関連のアルゴリズムのテスト
 */
void testLogLogCounting()
{
    printf("Naive, LineCount,    LogLog,HyperLogLog\n");
    for(int32_t dn=10;dn<100000;dn*=1.25)
    {
        // データセットを作成する
        std::vector<size_t> data;
        for(int32_t i=0;i<dn;++i)
        {
            data.push_back(hash0(i));
        }
        printf("%5lu, %9lu, %9.0f, %10.0f\n",
               computeCardinality_Naive(data),
               computeCardinality_LinerCounting(data),
               computeCardinality_LogLog(data),
               computeCardinality_HyperLogLog(data) );
    }
}

//
int32_t main()
{
    // testBloomFilter();
    testLogLogCounting();
}
