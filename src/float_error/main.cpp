/*
 区間演算のテスト
 - ErrFloatクラスを作成する
   - テストのためなのでlong doubleを真値として格納してく
 */

#include <cstdio>
#include <cassert>
#include <random>
#include <tuple>
#include <cstdint>

//
float
uint2float(uint32_t v)
{
    float r;
    memcpy(&r, &v, sizeof(uint32_t));
    return r;
}

//
uint32_t
float2uint(float v)
{
    uint32_t r;
    memcpy(&r, &v, sizeof(float));
    return r;
}

//
float
nextFloatUp(float v)
{
    // inf -> inf
    if (std::isinf(v) && v > 0.0f)
    {
        return v;
    }
    //
    if (v == -0.0f)
    {
        v = +0.0f;
    }
    //
    uint32_t u = float2uint(v);
    if (v >= 0.0f)
    {
        ++u;
    }
    else
    {
        --u;
    }
    const float nv = uint2float(u);
    return nv;
}

//
float
nextFloatDown(float v)
{
    // -inf -> -inf
    if (std::isinf(v) && v < 0.0f)
    {
        return v;
    }
    //
    if (v == +0.0f)
    {
        v = -0.0f;
    }
    //
    uint32_t u = float2uint(v);
    if (v >= 0.0f)
    {
        --u;
    }
    else
    {
        ++u;
    }
    const float nv = uint2float(u);
    return nv;
}

//
class ErrFlaot
{
public:
    ErrFlaot() = default;
    ErrFlaot(float v)
    {
        ref_ = v;
        value_ = v;
        high_ = v;
        low_ = v;
    }
    ErrFlaot(float v, float high, float low)
    {
        ref_ = v;
        value_ = v;
        high_ = high;
        low_ = low;
    }
    ErrFlaot(const ErrFlaot& other)
    {
        ref_ = other.value_;
        high_ = other.high_;
        low_ = other.low_;
        ref_ = other.ref_;
    }
    ErrFlaot operator + (const ErrFlaot& other) const
    {
        ErrFlaot r;
        r.value_ = value_ + other.value_;
        r.high_ = nextFloatUp(high_ + other.high_);
        r.low_ = nextFloatDown(low_ + other.low_);
        r.ref_ = ref_ + other.ref_;
        return r;
    }
    ErrFlaot operator - (const ErrFlaot& other) const
    {
        ErrFlaot r;
        r.value_ = value_ - other.value_;
        r.high_ = nextFloatUp(high_ - other.low_);
        r.low_ = nextFloatDown(low_ - other.high_);
        r.ref_ = ref_ - other.ref_;
        return r;
    }
    // 内部の状態が正しい状態であるか
    void printAndCheck() const
    {
        printf("V:%10.10f, Int:[%10.10f,%10.10f]  Ref:%Lf\n", value_, low_, high_, ref_);
        const bool v =
        ((long double)(low_) <= ref_) &&
        (ref_ <= (long double)(high_));
        assert(v);
    }
private:
    // 値
    float value_;
    // 区間
    float high_;
    float low_;
    // 真値
    long double ref_;
};

// テスト用の浮動小数を作成する
float genTestFloat(int32_t index)
{
    switch(index)
    {
        case 0: return 0.0f;
            case 1: return -0.0f;
            case 2: return +1.0f;
            case 3: return -1.0f;
            case 4: return +1.000000001f;
            case 5: return -1.000000001f;
            case 6: return +2.000000000001f;
            case 7: return +100000.000000000001f;
            case 8: return +100000000.000000000001f;
            case 9: return -100000000.000000000001f;
        default:
        {
            std::mt19937 gen(index);
            std::uniform_real_distribution<float> dist(-1.0f, 1.0f);
            return dist(gen);
        }
            break;
    }
}

std::tuple<float,float> genFloat2(int32_t index)
{
    const int i0 = index % 100;
    const int i1 = index / 100;
    return std::make_tuple(genTestFloat(i0),genTestFloat(i1));
}

//
void test0()
{
    //
    const int32_t fi = 100;
    for(int32_t i=0;i<fi;++i)
    {
        for(int32_t j=0;j<fi;++j)
        {
            ErrFlaot f0(genTestFloat(i));
            ErrFlaot f1(genTestFloat(j));
            const auto e0 = f0 - f1;
            e0.printAndCheck();
        }
    }
}

//
int main()
{
    nextFloatDown(-0.0f);
    //
    test0();
    return 0;
}
