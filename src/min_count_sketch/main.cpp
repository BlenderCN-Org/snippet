/*
 Count-Min sketch(CM, sketch,2003)
 BloomFilterが存在の近似を返すのに大して、Count-Min sketchは数値の近似を返す
 
 アルゴリズム
 - intのテーブルを持つ
 - k個のハッシュ関数を持っておく
 - 頻度を計算するアイテムが投入されるときにk個のハッシュを計算する。
 - マップ先のintを全てインクリメントする
 - 頻度を得るときは同様にk個のハッシュを使って全てのテーブルで一番少ない数が頻度値
 
 その他
 - 衝突しなければ正しい値が得られる
 - 頻度が多いものを検出するのに使う。(頻度が少ない大多数は....?)
 
 公式？: https://sites.google.com/site/countminsketch/
 わかりやすい解説: http://lkozma.net/blog/sketching-data-structures/
 JS実装: https://github.com/mikolalysenko/count-min-sketch
*/
#include <cstdio>
#include <cstdint>
#include <array>

//
class MinCountSketch
{
public:
    MinCountSketch()
    {
        table_.fill(0);
    }
    void add(int32_t value)
    {
        std::hash<int32_t>
    }
    
private:
    std::array<int32_t,512> table_;
};


//
int32_t main()
{
    
}
