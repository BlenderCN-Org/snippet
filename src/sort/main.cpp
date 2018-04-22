#include <cstdio>
#include <cstdint>

#include <iostream>
#include <vector>
#include <numeric>
#include <random>
#include <cmath>
#include <iostream>
#include <iomanip>

// merge sort
template<typename RandomIter>
void merge_sort(RandomIter first, RandomIter last)
{
    auto const len = std::distance(first, last);
    if (len <= 1) { return; }
    auto middle = first + len / 2;
    merge_sort(first, middle);
    merge_sort(middle, last);
    std::inplace_merge(first, middle, last);
}

//
int32_t main()
{
    //
    auto disp = [](const std::vector<int32_t>& datas)
    {
        for (auto i : datas)
        {
            std::cout << i << std::endl;
        }
    };
    // ランダムなデータを作成する
    std::vector<int32_t> datas(128);
    std::iota(datas.begin(), datas.end(), 0);
    std::random_device seed_gen;
    std::mt19937 engine(seed_gen());
    std::shuffle(datas.begin(), datas.end(), engine);
    //disp(datas);
    //
    ms(datas.begin(),datas.end());
    //
    disp(datas);
    
    return 0;
}
