/*
 区間演算のテスト
 - ErrFloatクラスを作成する
   - テストのためなのでlong doubleを真値として格納してく
 */

#include <cstdio>

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
        r.high_ = high_ + other.high_;
        r.low_ = low_ + other.low_;
        r.ref_ = ref_ + other.ref_;
        return r;
    }
    ErrFlaot operator - (const ErrFlaot& other) const
    {
        ErrFlaot r;
        r.value_ = value_ - other.value_;
        r.high_ = high_ - other.low_;
        r.low_ = low_ - other.high_;
        r.ref_ = ref_ - other.ref_;
        return r;
    }
    // 内部の状態が正しい状態であるか
    bool valid() const
    {
        return
        ((long double)(low_) < ref_) &&
        (ref_ < (long double)(high_));
    }
    void print() const
    {
        printf("V:%10.10f, Int:[%10.10f,%10.10f]  Ref:%Lf\n", value_, low_, high_, ref_);
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

//
void test0()
{
    ErrFlaot f0(1.0f);
    ErrFlaot f1(10.01f);
    const auto e0 = f0 - f1;
    e0.print();
}

//
int main()
{
    test0();
    return 0;
}
