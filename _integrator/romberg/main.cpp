/*
 Romberg integration, Richardson extrapolation
 https://en.wikipedia.org/wiki/Romberg%27s_method
 https://en.wikipedia.org/wiki/Richardson_extrapolation
 */
#include <cstdio>
#include <cstdint>
#include <random>
#include <cmath>
#include <functional>
#include <array>

struct TestSet
{
public:
    std::function<double(double)> fun;
    double g;
};
const TestSet testSets[] =
{
    // integra[ e^x, {x,0,1}] -> e-1
    [](double x) -> double
    {
        return std::exp(x);
    },
    M_E - 1.0,
    
    // integra[ 1/(1+25*x^2), {x,0,1}] -> atan(5)/5
    [](double x) -> double
    {
        return 1.0/(1.0+25.0*x*x);
    },
    std::atan(5.0)/5.0,
    
    // integra[ 1/(1+x), {x,0,1}] -> log(2)
    [](double x) -> double
    {
        return 1.0/(x+1.0);
    },
    std::log(2.0)
};

//
int32_t main()
{
    const int32_t numSample = 16;
    for(auto& testSet: testSets)
    {
        //
        double faSimpson1 = 0.0;
        double faSimpson2 = 0.0;
        double faSimpson3 = 0.0;
        double faSimpson4 = 0.0;
        double fsRomberg = 0.0;
        //
        auto& fun = testSet.fun;
        for(int32_t sn=0;sn<numSample;++sn)
        {
            // trapezoidal(simpson order 1)
            {
                const double x = double(sn)/double(numSample);
                const double dx =1.0f/double(numSample);
                const double y = (fun(x + 0.0) + fun(x + dx)) * 0.5;
                faSimpson1 += y * dx;
            }
            // simpson order 2
            {
                const double x = double(sn)/double(numSample);
                const double dx =1.0f/double(numSample);
                const double y =
                (1.0*fun(x + 0.0) +
                 4.0*fun(x + dx*0.5) +
                 1.0*fun(x + dx))
                / 6.0;
                faSimpson2 += y * dx;
            }
            // simpson order 3
            {
                const double x = double(sn)/double(numSample);
                const double dx =1.0f/double(numSample);
                const double y =
                (1.0*fun(x + 0.0) +
                 3.0*fun(x + dx*1.0/3.0) +
                 3.0*fun(x + dx*2.0/3.0) +
                 1.0*fun(x + dx)) / 8.0;
                faSimpson3 += y * dx;
            }
            // Boole's rule(simpson order 4)
            {
                const double x = double(sn)/double(numSample);
                const double dx =1.0f/double(numSample);
                const double y =
                (7.0*fun(x + 0.0) +
                 32.0*fun(x + dx*1.0/4.0) +
                 12.0*fun(x + dx*2.0/4.0) +
                 32.0*fun(x + dx*3.0/4.0) +
                 7.0*fun(x + dx)) / 90.0;
                faSimpson4 += y * dx;
            }
        }
        // romberg integration, richardson extrapolation
        {
            //
            const auto simpson = [fun](int32_t numSample)
            {
                double faSimpson = 0.0;
                for(int32_t sn=0;sn<numSample;++sn)
                {
                    const double x = double(sn)/double(numSample);
                    const double dx =1.0f/double(numSample);
                    const double y = (fun(x + 0.0) + fun(x + dx)) * 0.5;
                    faSimpson += y * dx;
                }
                return faSimpson;
            };
            //
            const int32_t size = 5;
            double table[size][size];
            for(int32_t i=0;i<size;++i)
            {
                table[0][i] = simpson(1<<i);
                
            }
            //
            for(int32_t j=1;j<size;++j)
            {
                for(int32_t i=j;i<size;++i)
                {
                    const double fi = std::pow(4.0,j);
                    table[j][i] = (fi*table[j-1][i] - table[j-1][i-1])/(fi-1.0);
                }
            }
            fsRomberg = table[size-1][size-1];
#if 0
            for(int32_t j=1;j<size;++j)
            {
                for(int32_t i=j;i<size;++i)
                {
                    printf("%f ", table[j][i]);
                }
                puts("");
            }
#endif
        }
        const double g = testSet.g;
        printf("-------------------------\n");
        printf("Simpson1: %.16f\n", std::abs(1.0f-faSimpson1/g));
        printf("Simpson2: %.16f\n", std::abs(1.0f-faSimpson2/g));
        printf("Simpson3: %.16f\n", std::abs(1.0f-faSimpson3/g));
        printf("Simpson4: %.16f\n", std::abs(1.0f-faSimpson4/g));
        printf("Romberg:  %.16f\n", std::abs(1.0f-fsRomberg/g));
    }
}
