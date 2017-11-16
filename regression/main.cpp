
#include <cstdio>
#include <array>

/*
���v�Ŋw�ԋ@�B�w�K - �P��A��� -
http://tkengo.github.io/blog/2016/01/04/yaruo-machine-learning2/
*/

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

//
void main()
{
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
    for (int32_t i=0;i<100;++i)
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
        //printf("��0=%f  ��1=%f\n", theta0, theta1);
    }

    // �������킹
    for (auto& d : datas)
    {
        printf("A:%f D:%f E:%f\n", d.attack, d.damage, fTheta(d.attack));
    }
}
