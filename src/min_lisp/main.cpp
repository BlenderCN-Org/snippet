/*
書籍「やさしいLispの作り方」より
*/
#include <cstdio>
#include <cstdint>
#include <array>
#include <vector>
#include <iostream>
#include <string>

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
    std::string buffer;
};

//
static std::vector<Token> getToken()
{
    std::string buffer;
    std::getline(std::cin, buffer);

    printf("[%s]\n", buffer.c_str());

    std::vector<Token> tokens;
    for(int32_t idx=0;idx<buffer.size();++idx)
    {
        char ch = buffer[idx];
        if (ch == '(')
        {
            Token token;
            token.tokenType = TokenType::Lparen;
            token.buffer = "(";
            tokens.push_back(token);
        }
        else if (ch == ')')
        {
            Token token;
            token.tokenType = TokenType::Rparen;
            token.buffer = ")";
            tokens.push_back(token);
        }
        else if (ch == ',')
        {
            Token token;
            token.tokenType = TokenType::Dot;
            token.buffer = ",";
            tokens.push_back(token);
        }
        // 数値
        else if (isdigit(ch))
        {
            std::string num;
            num += ch;
            while (true)
            {
                ++idx;
                if (buffer.size() <= idx)
                {
                    break;
                }
                ch = buffer[idx];
                if (!isdigit(ch))
                {
                    --idx;
                    break;
                }
                num += ch;
            }
            //
            Token token;
            token.tokenType = TokenType::Number;
            token.buffer = num;
            tokens.push_back(token);
        }
        +とかはどうするのか。
        else
        {
        }
    }

#if 0
    // 確認出力
    for (const auto& token : tokens)
    {
        printf("TokenType:%d Buffer:%s\n", token.tokenType, token.buffer.c_str() );
    }
#endif

    return tokens;
}

//
int32_t main()
{
    initcell();

    getToken();


    return 0;
}
