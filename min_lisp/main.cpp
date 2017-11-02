/*
書籍「やさしいLispの作り方」より
*/
#include <cstdio>
#include <cstdint>
#include <array>

enum class CellTag
{
    Empty,
    Num,
    Sym,
    List,
    Subr,
    Fsubr,
    Fun
};

enum class UseFalg
{
    Free,
    Use
};

//
struct Cell
{
public:
    CellTag tag = CellTag::Empty;
    UseFalg useFlag = UseFalg::Free;
    // シンボル名
    char* name = nullptr;
    union
    {
        int32_t num;
        int32_t bind;
        int(*subr)();
    }val;
    int32_t car = 0;
    int32_t cdr = 0;
};

const int32_t HEAP_SIZE = 256;

//
struct Global
{
    std::array<Cell, HEAP_SIZE> cells;
    // 自由リストの先頭
    int32_t hp = 0;
    int32_t fc = HEAP_SIZE;
    // 環境リストの先頭
    int32_t ep = 0;
    // スタックの先頭番地
    int32_t sp = 0;
    // 引数リストの先頭番地
    int32_t ap = 0;
}g;

//
void initcell()
{
    for (int32_t addr = 0; addr < g.cells.size(); ++addr)
    {
        auto& cell = g.cells[addr];
        cell.useFlag = UseFalg::Free;
        cell.cdr = addr + 1;
    }
}

//
enum class TokenType
{
    // '('
    Lparen,
    // ')'
    Rparen,
    // '"'
    Quote,
    // 
    Dot,
    // 数値
    Number,
    // シンボル
    Symbol,
    // 
    Other,
};

//
enum class BackTrack
{
    Go,
    Back
};

//
struct Token
{
    // 読み込んだ一文字
    char ch;
    // 次の文字も読むか
    BackTrack backtrack;
    // トークンの種類
    TokenType tokenType;
    // トークンの文字列
    char buf[0xff];
};

//
void getToken()
{

}

//
int32_t main()
{
    initcell();


    return 0;
}
