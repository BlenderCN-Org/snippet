#include <cstdio>
#include <cstdint>
#include <cmath>
#include <array>
#include <vector>

//
void main()
{
    struct Number
    {
    public:
        void set(size_t v)
        {
            digits_.clear();
            while (v!=0)
            {
                digits_.push_back(v % 10);
                v /= 10;
            }
        }
        void process()
        {
            tmp_ = digits_;
            std::reverse(tmp_.begin(), tmp_.end());
            bool carry = false;
            for (int32_t i=0;i<digits_.size();++i)
            {
                const int32_t t = digits_[i] + tmp_[i] + (carry ? 1 : 0);
                tmp_[i] = t % 10;
                carry = (t >= 10);
            }
            if (carry)
            {
                tmp_.push_back(1);
            }
            digits_ = tmp_;
        }
        // ‰ñ•¶‚©
        bool isPali() const
        {
            for (int32_t i = 0, n = digits_.size() / 2; i < n; ++i)
            {
                const int32_t a = digits_[i];
                const int32_t b = digits_[digits_.size() -i - 1];
                if (a != b)
                {
                    return false;
                }
            }
            return true;
        }
        void print()
        {
            for(int32_t i= digits_.size()-1;i>=0;--i)
            {
                printf("%d", digits_[i]);
            }
            printf("\n");
        }
    public:
        std::vector<int32_t> digits_;
        std::vector<int32_t> tmp_;
    };

    Number n;
    int32_t count = 0;
    for (int32_t i=1;i<10000;++i)
    {
        n.set(i);
        const bool isOkay = [&]()
        {
            for (int32_t j = 0; j < 50; ++j)
            {
                n.process();
                if (n.isPali())
                {
                    return true;
                }
            }
            return false;
        }();
        count += isOkay ? 0 : 1;
    }
    printf("%d\n", count);
}
