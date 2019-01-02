#include <cstdio>
#include <cstdint>
#include <cmath>

//
void main()
{
    // (n,r)�̑g�ݍ��킹��10^6�𒴂��邩�ۂ���Ԃ��B
    // �덷�Ō듚�����������邪���̖��ł͖��ɂȂ�Ȃ�����
    const auto comb10_5 = [](int32_t n, int32_t r)
    {
        double digit = 0.0;
        for (int32_t i = 0; i < r; ++i)
        {
            digit += std::log10(double(n-i));
            digit -= std::log10(double(i+1.0));
        }
        return (digit >= 6.0);
    };
    int32_t total = 0;
    for (int32_t n = 1; n <= 100; ++n)
    {
        for (int32_t r = 1; r <= 100; ++r)
        {
            total += comb10_5(n, r) ? 1 : 0;
        }
    }
    printf("%d\n", total);
    return;
}
