#include <cstdio>
#include <cstdint>
#include <cstring>

//
void main()
{
    class Local
    {
    public:
        static int32_t toLetter(int32_t v)
        {
            switch (v)
            {
            case 1000:return strlen("one") + strlen("thousand");
            case 1: return strlen("one");
            case 2: return strlen("two");
            case 3: return strlen("three");
            case 4: return strlen("four");
            case 5: return strlen("five");
            case 6: return strlen("six");
            case 7: return strlen("seven");
            case 8: return strlen("eight");
            case 9: return strlen("nine");
            case 10: return strlen("ten");
            case 11: return strlen("eleven");
            case 12: return strlen("twelve");
            case 13: return strlen("thirteen");
            case 14: return strlen("fourteen");
            case 15: return strlen("fifteen");
            case 16: return strlen("Sixteen");
            case 17: return strlen("seventeen");
            case 18: return strlen("eighteen");
            case 19: return strlen("nineteen");
            case 20: return strlen("twenty");
            case 30: return strlen("thirty");
            case 40: return strlen("forty");
            case 50: return strlen("fifty");
            case 60: return strlen("sixty");
            case 70: return strlen("seventy");
            case 80: return strlen("eighty");
            case 90: return strlen("ninety");
            }
            if (21 <= v && v < 30)
            {
                return strlen("twenty") + toLetter(v - 20);
            }
            else if (31 <= v && v < 40)
            {
                return strlen("thirty") + toLetter(v - 30);
            }
            else if (41 <= v && v < 50)
            {
                return strlen("forty") + toLetter(v - 40);
            }
            else if (51 <= v && v < 60)
            {
                return strlen("fifty") + toLetter(v - 50);
            }
            else if (61 <= v && v < 70)
            {
                return strlen("sixty") + toLetter(v - 60);
            }
            else if (71 <= v && v < 80)
            {
                return strlen("seventy") + toLetter(v - 70);
            }
            else if (81 <= v && v < 90)
            {
                return strlen("eighty") + toLetter(v - 80);
            }
            else if (91 <= v && v < 100)
            {
                return strlen("ninety") + toLetter(v - 90);
            }
            else
            {
                return Local::toLetter(v / 100) + strlen("hundred") + ((v%100!=0) ? strlen("and") + toLetter(v % 100) : 0);
            }
        }
    };
    size_t total = 0;
    for (int32_t i = 1; i <= 1000;++i)
    {
        total += Local::toLetter(i);
    }
    printf("%lld\n", total);
    //
    return;
}
