#include <Siv3D.hpp>

//
class DQWindow
{
public:
    //
    DQWindow() = default;
    //
    void setRect(const Rect& rect)
    {
        rect_ = rect;
    }
    //
    void setTexts(std::initializer_list<String> texts)
    {
        texts_ = std::vector<String>(texts.begin(), texts.end());
    }
    //
    void setCursorIndex(int32_t cursorIndex)
    {
        cursorIndex_ = cursorIndex;
    }
    // 押されたらtrue
    bool update()
    {
        // カーソル表示状態
        if (!isActive_)
        {
            return;
        }
        // 上
        if (Input::KeyUp.clicked)
        {
            cursorIndex_ -= 1;
        }
        // 下
        else if (Input::KeyDown.clicked)
        {
            cursorIndex_ += 1;
        }
        cursorIndex_ = Min(int32_t(texts_.size()) - 1, cursorIndex_);
        // 決定
        if (Input::KeyEnter.clicked)
        {
            return true;
        }
        else
        {
            return false;
        }

    }
    //
    void draw(int32_t frame)
    {
        //
        rect_.draw(Palette::Black);
        rect_.drawFrame(3);
        //
        auto textPos = rect_.pos + Vec2(23, 10);
        for (auto& text : texts_)
        {
            font_(text).draw(textPos);
            textPos += Vec2(0.0, font_.height);
        }
        //
        if (isActive_)
        {
            // 明滅
            if ((frame/20) % 2 != 0)
            {
                font_(L"▶").draw(rect_.pos + Vec2(9, 10 + cursorIndex_ * font_.height));
            }
        }
    }
public:
    //
    Rect rect_ = { 100, 100, 400, 300 };
    // テキスト
    std::vector<String> texts_;
    // カーソル位置
    int32_t cursorIndex_ = 2;
    // 前のウィンドウ
    int32_t prevWindowIdx_ = -1;
    // 次のウィンドウ
    int32_t nextWindowIdx_ = -1;
    // アクティブ状態か
    bool isActive_ = false;
private:
    //
    Font font_ = Font(10);
    
    Vec2 mergin = { 20, 10 };
};

// 
void Main()
{
    // タイトル
    Window::SetTitle(L"WQ_test");
    // 背景は黒
    Graphics::SetBackground(Palette::Black);
    //
    std::array<DQWindow, 4> windows;
    // Window0 
    windows[0].rect_ = { 100, 100, 400, 300 };
    windows[0].texts_ = 
    {
        L"あたらしいぼうけんしょをつくる",
        L"ひょうじそくどをかえる",
        L"ぼうけんしょをけす"
    };
    windows[0].isActive_ = true;
    // Window1
    windows[1].rect_ = { 200, 200, 400, 300 };
    windows[1].texts_ = 
    {
        L"ぼうけんのしょ １：ゆうや",
        L"ぼうけんのしょ ２：まみ",
        L"ぼうけんのしょ ３：まえはボ",
    };
    // Window2
    windows[2].rect_ = { 300, 300, 400, 300 };
    windows[2].texts_ = 
    {
        L"ゆうしゃ　まえはボ",
        L"レベル　　２５",
        L"のぼうけんのしょ　を",
        L"　けします。",
        L"いいですか？",
    };
    // Window3
    windows[3].rect_ = { 400, 400, 400, 300 };
    windows[3].texts_ = 
    {
        L"はい",
        L"いいえ",
    };
    //
    int32_t frame = 0;
    //
    while (System::Update())
    {
        for (auto& window : windows)
        {
            window.draw(frame);
            window.update();
        }
        ++frame;
    }
}
