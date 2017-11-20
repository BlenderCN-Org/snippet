
#include <cstdio>
#include <array>

/*
やる夫で学ぶ機械学習 - 単回帰問題 -
http://tkengo.github.io/blog/2016/01/04/yaruo-machine-learning2/
*/
void regressionMain()
{
    //
    struct Data
    {
    public:
        float attack;
        float damage;
    public:
        Data(
            float aattack,
            float adamage)
        {
            attack = aattack;
            damage = adamage;
        }
    };

    const std::array<Data, 4> datas =
    {
        Data(3.0f,30.0f),
        Data(3.7f,35.0f),
        Data(4.3f,33.0f),
        Data(4.6f,44.0f)
    };
    //
    float theta0 = 0.0f;
    float theta1 = 0.0f;
    const float eta0 = 0.001f;
    const float eta1 = 0.001f;
    //
    const auto fTheta = [&](float x)
    {
        return theta0 + x * theta1;
    };
    //
    for (int32_t i = 0; i<100; ++i)
    {
        //
        float tmp0 = 0.0f;
        float tmp1 = 0.0f;
        for (auto& d : datas)
        {
            tmp0 += fTheta(d.attack) - d.damage;
            tmp1 += (fTheta(d.attack) - d.damage) * d.attack;
        }
        //
        theta0 = theta0 - eta0 * tmp0;
        theta1 = theta1 - eta1 * tmp1;
        //printf("θ0=%f  θ1=%f\n", theta0, theta1);
    }

    // 答え合わせ
    for (auto& d : datas)
    {
        printf("A:%f D:%f E:%f\n", d.attack, d.damage, fTheta(d.attack));
    }
}

/*
重回帰を最適化として解く。線形回帰であれば普通に解析的に出した方がよさそう。
cf. やる夫で学ぶ機械学習 - 多項式回帰と重回帰 -
    http://tkengo.github.io/blog/2016/06/02/yaruo-machine-learning3/
*/
void multipleRegression()
{
    //
    struct Data
    {
    public:
        float tenin;
        float uriba;
        float syouhin;
        float uriage;
    public:
        Data(
            float atenin,
            float auriba,
            float asyouhin,
            float auriage)
            :tenin(atenin),
            uriba(auriba),
            syouhin(asyouhin),
            uriage(auriage)
        {}
    };

    const std::array<Data, 8> datas =
    {
        Data(8.0f, 4.0f, 8.0f, 18.0f),
        Data(7.0f, 7.0f, 7.0f, 12.0f),
        Data(5.0f, 8.0f, 9.0f, 14.0f),
        Data(4.0f, 3.0f, 3.0f, 6.0f),
        Data(6.0f, 8.0f, 8.0f, 12.0f),
        Data(2.0f, 5.0f, 3.0f, 8.0f),
        Data(3.0f, 6.0f, 6.0f, 10.0f),
        Data(9.0f, 9.0f, 7.0f, 16.0f),
    };
    //
    float theta0 = 0.0f;
    float theta1 = 0.0f;
    float theta2 = 0.0f;
    float theta3 = 0.0f;
    //
    const auto fTheta = [&](const Data& d)
    {
        return
            theta0 +
            d.tenin * theta1 +
            d.uriba * theta2 +
            d.syouhin * theta3;
    };
    //
    const float eta = 0.001f;
    for (int32_t i = 0; i<1000; ++i)
    {
        //
        float tmp0 = 0.0f;
        float tmp1 = 0.0f;
        float tmp2 = 0.0f;
        float tmp3 = 0.0f;
        for (auto& d : datas)
        {
            const float c = fTheta(d) - d.uriage;
            tmp0 += c;
            tmp1 += c * d.tenin;
            tmp2 += c * d.uriba;
            tmp3 += c * d.syouhin;
        }
        //
        theta0 = theta0 - eta * tmp0;
        theta1 = theta1 - eta * tmp1;
        theta2 = theta2 - eta * tmp2;
        theta3 = theta3 - eta * tmp3;
        //printf("θ0=%f  θ1=%f\n", theta0, theta1);
    }

    // 答え合わせ
    printf("%f,%f,%f,%f\n", theta0, theta1, theta2, theta3);
    for (auto& d : datas)
    {
        printf("D:%f E:%f\n", d.uriage, fTheta(d));
    }
}


// https://softwareengineering.stackexchange.com/questions/337617/algorithm-for-windowed-online-covariance
class FloatStreamStats2
{
public:
    void add(float x, float y)
    {
        n_ += 1.0f;
        const float invn = 1.0f / n_;        
        const float dx = (x - ex_) * invn;
        const float dy = (y - ey_) * invn;
        const float nex = ex_ + dx;
        const float ney = ey_ + dy;
        const float nmx = (x - ex_) * (x - nex) + mx_;
        const float nmy = (y - ey_) * (y - ney) + my_;
        ex_ = nex;
        ey_ = ney;
        mx_ = nmx;
        my_ = nmy;
        cov_ += (n_ - 1.0f) * dx * dy - cov_ * invn;
    }
    // 平均
    float ex() const
    {
        return ex_;
    }
    float ey() const
    {
        return ey_;
    }
    // 分散
    float var0() const
    {
        return mx_ / n_;
    }
    float var1() const
    {
        return my_ / n_;
    }
    // 標準偏差
    float sigma0() const
    {
        return std::sqrtf(var0());
    }
    float sigma1() const
    {
        return std::sqrtf(var1());
    }
    // 偏共分散
    float cov() const
    {
        return (n_ == 0) ? 0.0f : cov_;
    }
    // 相関係数
    float R() const
    {
        return cov() / std::sqrtf(var0()*var1());
    }
private:
    float n_ = 0.0f;
    float ex_ = 0.0;
    float ey_ = 0.0;
    float mx_ = 0.0f;
    float my_ = 0.0f;
    float cov_ = 0.0;
};

/*
2つの変数から重回帰
cf アイスクリーム屋さんで学ぶ 楽しい統計学
   http://kogolab.chillout.jp/elearn/icecream/chap6/sec0.html
*/
void multipleRegression2()
{
    //
    struct Data
    {
    public:
        float x1; // 最高気温
        float x2; // 最低気温
        float y;  // 客数
    public:
        Data(
            float ax1,
            float ax2,
            float ay)
            :x1(ax1),
            x2(ax2),
            y(ay)
        {}
    };
    const std::array<Data, 20> datas =
    {
        Data(33, 22, 382),
        Data(33, 26, 324),
        Data(34, 27, 338),
        Data(34, 28, 317),
        Data(35, 28, 341),
        Data(35, 27, 360),
        Data(34, 28, 339),
        Data(32, 25, 329),
        Data(28, 24, 218),
        Data(35, 24, 402),
        Data(33, 26, 342),
        Data(28, 25, 205),
        Data(32, 23, 368),
        Data(25, 22, 196),
        Data(28, 21, 304),
        Data(30, 23, 294),
        Data(29, 23, 275),
        Data(32, 25, 336),
        Data(34, 26, 384),
        Data(35, 27, 368)
    };
    FloatStreamStats2 x1y;
    FloatStreamStats2 x2y;
    FloatStreamStats2 x1x2;
    for (auto& d : datas)
    {
        x1y.add(d.x1, d.y);
        x2y.add(d.x2, d.y);
        x1x2.add(d.x1, d.x2);
    }
    const float r1y = x1y.R();
    const float r2y = x2y.R();
    const float r12 = x1x2.R();
    const float ex1 = x1y.ex();
    const float ex2 = x2y.ex();
    const float ey = x1y.ey();
    const float sig1 = x1y.sigma0();
    const float sig2 = x2y.sigma0();
    const float sigY = x1y.sigma1();
    // 偏回帰係数
    const float p = ((r1y - (r2y * r12)) * (sigY)) / ((1.0f - r12*r12) * sig1);
    const float q = ((r2y - (r1y * r12)) * (sigY)) / ((1.0f - r12*r12) * sig2);
    const float m = ey - (p * ex1 + q * ex2);
    printf("%f, %f, %f\n", p, q, m);
    // 答え合わせ
    for (auto& d : datas)
    {
        const float e = m + p * d.x1 + q * d.x2;
        printf("D:%f E:%f\n", d.y, e);
    }

    return;
}

//
void main()
{
    // 単回帰
    // regressionMain();
    // 重回帰
    //multipleRegression();
    // 重回帰2
    multipleRegression2();
}
