/*
matplotの練習
*/
#include <cstdio>

// 基本的な使い方
void basic()
{
    FILE* gp = nullptr;
    if ((gp = _popen("\"C:/Program Files/gnuplot/bin/gnuplot.exe\" -persist", "w")) == nullptr) {
        return;
    }
    fprintf(gp, "splot sin(sqrt((x*x)+(y*y)))\n");
    fprintf(gp, "exit\n");
    fflush(gp);
    _pclose(gp);
}

// 散布図

//
int main()
{
    basic();
    return 0;
}