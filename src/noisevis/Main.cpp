//
#include <Siv3D.hpp>
//
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>

//
typedef std::array<float, 1024> nvFFTResult;

// 指定した範囲に可視化する
void visualizeFFT(
    const nvFFTResult& fft,
    int32_t x, int32_t y,
    int32_t width, int32_t height,
    const String& title,
    const std::function<float(float)>& fftScaler )
{
    //
    for (auto i : step(width-1))
    {
        const int32_t idx = int32_t(float(i) / float(width) * float(fft.size() - 1));
        RectF(x + i, y + height-1, 1, -fftScaler(fft[idx]) * (height-1)).draw(HSV(double(i) / double(width)*360.0));
    }
    // 枠描画
    RectF(x, y, width, height).drawFrame();
    // タイトル描画
    static const Font font(12, Typeface::Heavy);
    font(title).draw(x+2,y+2);
}

void visualizeFrame(
    const std::vector<nvFFTResult>& frames,
    int32_t minHz,
    int32_t maxHz,
    int32_t x, int32_t y,
    int32_t width,
    int32_t height,
    const String& title,
    const std::function<float(float)>& fftScaler)
{
    // 目盛り
    const int32_t rulerWidth = width/10;
    for (int32_t i=0;i<width; i+= rulerWidth)
    {
        RectF(x+i, y, 1, height).draw(Palette::Gray);
    }

    // 蓄えられているデータを可視化する
    for (auto i : step(width - 1))
    {
        const int32_t idx = int32_t(frames.size()) - i - 1;
        if (idx < 0)
        {
            break;
        }
        //
        const auto& frame = frames[idx];
        // 現在の範囲のHzの最大値を計算する
        float maxValue = 0.0f;
        for (int32_t hz = minHz; hz < maxHz; ++hz)
        {
            maxValue = std::max(frame[hz], maxValue);
        }
        //
        RectF(x+i, y + height - 1, 1, -fftScaler(maxValue) *  float(height-1)).draw(Palette::White);
    }
    // 枠描画
    RectF(x, y, width, height).drawFrame();
    // タイトル描画
    static const Font font(12, Typeface::Heavy);
    font(title).draw(x + 2, y + 2);
}

// 現在の状況をスクショにとってSlackにアップする
void uploadToSlack(std::string proxy, std::string token)
{
    
}

//
void Main()
{
    //
    System::SetExitEvent(WindowEvent::CloseButton);

    //
    FilePath fp = FileSystem::CurrentPath();
    fp.append(L"config.ini");
    const INIReader ini(fp.str());
    const std::string proxy =  ini.get<String>(L"config.proxy").narrow();
    const std::string token = ini.get<String>(L"config.token").narrow();

    //
    Window::Resize(600, 708);
    //
    GUI gui(GUIStyle::Default);
    gui.setPos(0, 500);
    gui.style.width = 600;
    //
    auto uiTextMinHz = GUIText::Create(L"0Hz");
    auto uiTextMaxHz = GUIText::Create(L"1024Hz");
    auto uiTextVisPower = GUIText::Create(L"");
    auto uiTextVisScale = GUIText::Create(L"");
    //
    auto uiSliderMinHzRaw = GUISlider::Create(0.0f, 1024.0f, 100.0f, 500);
    auto uiSliderMaxHzRaw = GUISlider::Create(0.0f, 1024.0f, 1024.0f, 500);
    auto uiSliderVisPowerRaw = GUISlider::Create(0.0f, 2.0f, 0.5f, 500);
    auto uiSliderVisScaleRaw = GUISlider::Create(1.0f, 20.0f, 10.0f, 500);
    //
    gui.add(L"hr", GUIHorizontalLine::Create(1));
    gui.add(L"minHz", uiSliderMinHzRaw);
    gui.add(uiTextMinHz);
    //
    gui.add(L"hr", GUIHorizontalLine::Create(1));
    gui.add(L"maxHz", uiSliderMaxHzRaw);
    gui.add(uiTextMaxHz);
    // 表示の調整
    gui.add(L"hr", GUIHorizontalLine::Create(1));
    gui.add(L"vis_power", uiSliderVisPowerRaw);
    gui.add(uiTextVisPower);
    gui.add(L"hr", GUIHorizontalLine::Create(1));
    gui.add(L"vis_scale", uiSliderVisScaleRaw);
    gui.add(uiTextVisScale);

    //
    auto uiSliderMinHz = gui.slider(L"minHz");
    auto uiSliderMaxHz = gui.slider(L"maxHz");
    auto uiSliderVisPower = gui.slider(L"vis_power");
    auto uiSliderVisScale = gui.slider(L"vis_scale");
    //
    const Font font(30);

    // サウンドの初期化
    Recorder recorder;
    if (!recorder.open(0, 5s, RecordingFormat::S44100, true) ||
        !recorder.start())
    {
        return;
    }

    class Frames
    {
    public:
        typedef std::function<bool(const DateTime& now, const DateTime& last)> IsSameFun;
    public:
        Frames(const DateTime& startTime, const IsSameFun& isSameFun)
            :isSameTime_(isSameFun)
        {
            frames_.push_back(nvFFTResult());
            last_ = startTime;
        }
        const nvFFTResult & frame() const
        {
            return frames_.back();
        }
        const std::vector<nvFFTResult>& frames() const
        {
            return frames_;
        }
        void update(const DateTime& now, const nvFFTResult& fft)
        {
            // 最大値を更新
            auto& f = frames_.back();
            for (int32_t i = 0; i < int32_t(f.size()); ++i)
            {
                f[i] = std::max(fft[i], f[i]);
            }

            // 時間を跨いでいたらチャンクに投入してフレームをリセット
            if(!isSameTime_(now, last_))
            {
                // フレームに追加
                frames_.push_back(nvFFTResult());
                if (frames_.size() > 1024)
                {
                    frames_.erase(frames_.begin());
                }
                //
                last_ = now;
            }
        }
    public:
        IsSameFun isSameTime_;
        DateTime last_;
        std::vector<nvFFTResult> frames_;
    };
    //
    const auto startTime = DateTime::Now();
    //
    Frames sec5Frames(startTime, [](const DateTime& now, const DateTime& last)
    {
        return
            (now.year == last.year) &&
            (now.month == last.month) &&
            (now.day == last.day) &&
            (now.hour == last.hour) &&
            (now.minute == last.minute) &&
            (now.second/5 == last.second / 5);
    });
    Frames min3Frames(startTime, [proxy,token](const DateTime& now, const DateTime& last)
    {
        if ((now.year == last.year) &&
            (now.month == last.month) &&
            (now.day == last.day) &&
            (now.hour == last.hour) &&
            ((now.minute) == (last.minute)))
        {
            
        }
        else
        {
            
        }

        if((now.year == last.year) &&
            (now.month == last.month) &&
            (now.day == last.day) &&
            (now.hour == last.hour) &&
            ((now.minute / 3) == (last.minute / 3)))
        {
            return true;
        }
        else
        {
            uploadToSlack(proxy, token);
            return false;
        }
    });
    Frames min20Frames(startTime, [proxy, token](const DateTime& now, const DateTime& last)
    {
        if ((now.year == last.year) &&
            (now.month == last.month) &&
            (now.day == last.day) &&
            (now.hour == last.hour) &&
            ((now.minute / 20) == (last.minute / 20)))
        {
            return true;
        }
        else
        {
            // 画像を上げるのは一時間に一回
            if (now.hour != last.hour)
            {
                uploadToSlack(proxy, token);
            }
            return false;
        }
    });

    //
    uploadToSlack(proxy, token);

    //
    bool requestCapture = false;

    //
    while (System::Update())
    {
        //
        const nvFFTResult fft = [&recorder]()
        {
            const FFTResult fftRaw = FFT::Analyze(recorder);
            nvFFTResult fft;
            for (int32_t i = 0; i < fft.size(); ++i)
            {
                fft[i] = fftRaw.buffer[i];
            }
            return fft;
        }();

        //
        const auto now = DateTime::Now();
        sec5Frames.update(now, fft);
        min3Frames.update(now, fft);
        min20Frames.update(now, fft);

        //
        const float power = float(uiSliderVisPower.value);
        const float scale = float(uiSliderVisScale.value);
        const auto scaler = [power, scale](float raw)
        {
            return std::min(Pow(raw, power)*scale, 1.0f);
        };
        visualizeFFT(fft,         0, 0, 300, 100, L"Current", scaler);
        visualizeFFT(sec5Frames.frame(), 300, 0, 300, 100, L"Last 5Sec", scaler);
        visualizeFFT(min3Frames.frame(), 0, 100, 300, 100, L"Last 3Min", scaler);
        visualizeFFT(min20Frames.frame(), 300, 100, 300, 100, L"Last 20Hour", scaler);
        //
        const int32_t minHz = int32_t(uiSliderMinHz.value);
        const int32_t maxHz = int32_t(uiSliderMaxHz.value);
        visualizeFrame(sec5Frames.frames(), minHz, maxHz, 0, 200, 600, 100, L"Last 25Min", scaler);
        visualizeFrame(min3Frames.frames(), minHz, maxHz, 0, 300, 600, 100, L"Last 30Hour", scaler);
        visualizeFrame(min20Frames.frames(), minHz, maxHz, 0, 400, 600, 100, L"Last 5Days", scaler);

        // UIの更新
        uiTextMinHz->getText() = Format(int32_t(uiSliderMinHz.value), L"Hz");
        uiTextMaxHz->getText() = Format(int32_t(uiSliderMaxHz.value), L"Hz");
        uiTextVisPower->getText() = Format(DecimalPlace(2), uiSliderVisPower.value);
        uiTextVisScale->getText() = Format(DecimalPlace(2), uiSliderVisScale.value);

        //
        Window::SetTitle(Format(L"NoisVis FPS:", Profiler::FPS(), L"" ));

        //
        if (requestCapture)
        {
            requestCapture = false;
            //
            Image image;
            ScreenCapture::GetFrame(image);
            FilePath fp = FileSystem::CurrentPath();
            fp.append(L"tmp.png");
            image.savePNG(fp.str());
            std::string pstr;
            if (!proxy.empty())
            {
                pstr = std::string("-x ") + proxy;
            }

            // アップロードする
            const std::string curlCommand =
                "curl " + pstr + " -F file=@\"" + fp.narrow() +
                "\" -F channels=#sv_test -F token=" + token +
                " https://slack.com/api/files.upload";
            STARTUPINFOA si = { sizeof(STARTUPINFO) };
            PROCESS_INFORMATION pi;
            ::CreateProcessA(NULL, (LPSTR)curlCommand.c_str(), NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi);
            ::CloseHandle(pi.hProcess);
            ::CloseHandle(pi.hThread);
        }
        //
        static DateTime last = DateTime::Now();
        if (!((now.year == last.year) &&
            (now.month == last.month) &&
            (now.day == last.day) &&
            (now.hour == last.hour)))
        {
            last = now;
            ScreenCapture::Request();
            requestCapture = true;
        }
    }
}