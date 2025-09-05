// main.cpp
#include <wx/wx.h>
#include <wx/timer.h>
#include <wx/datetime.h>
#include <wx/timectrl.h>
#include <wx/spinctrl.h>
#include <iomanip>
#include <sstream>

enum { ID_Timer = wxID_HIGHEST + 1, ID_Timer2 = wxID_HIGHEST + 2 };

class ResultFrame : public wxFrame {
public:
    ResultFrame(int totalSeconds, const wxString& inputDesc)
        : wxFrame(nullptr, wxID_ANY, wxString::FromUTF8("倒计时结果"),
                  wxDefaultPosition, wxDefaultSize, wxPOPUP_WINDOW),
          m_timer(this, ID_Timer2) {
        wxPanel* panel = new wxPanel(this);
        wxBoxSizer* vs = new wxBoxSizer(wxVERTICAL);
        panel->SetSizer(vs);

        vs->AddStretchSpacer();

        wxBoxSizer* hs = new wxBoxSizer(wxHORIZONTAL);
        int h = totalSeconds / 3600;
        int m = (totalSeconds % 3600) / 60;
        int s = totalSeconds % 60;
        std::ostringstream oss;
        oss << std::setw(2) << std::setfill('0') << h << ":" << std::setw(2)
            << std::setfill('0') << m << ":" << std::setw(2)
            << std::setfill('0') << s;
        wxString totalStr = wxString::FromUTF8(oss.str().c_str());

        hs->Add(new wxStaticText(panel, wxID_ANY,
                                 wxString::FromUTF8("总计时: ") + totalStr),
                0, wxALIGN_CENTER);
        hs->Add(new wxStaticText(panel, wxID_ANY,
                                 wxString::FromUTF8("输入参数: ") + inputDesc),
                0, wxALIGN_CENTER | wxLEFT, 50);
        vs->Add(hs, 0, wxALIGN_CENTER);

        vs->AddSpacer(30);
        wxButton* ok = new wxButton(panel, wxID_OK, wxString::FromUTF8("确定"));
        vs->Add(ok, 0, wxALIGN_CENTER);
        vs->AddStretchSpacer();

        ok->Bind(wxEVT_BUTTON, [this](wxCommandEvent&) { Close(); });

        Layout();
        ShowFullScreen(true);
        m_time_count = 10;
        Bind(wxEVT_TIMER, &ResultFrame::OnTimer, this, ID_Timer2);
        m_timer.Start(1000, wxTIMER_CONTINUOUS);
    }

    wxTimer m_timer;
    int m_time_count;
    void OnTimer(wxTimerEvent&) {

        HWND hwnd = GetHWND();
        SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_SHOWWINDOW);
        SetForegroundWindow(hwnd);
        m_time_count--;
        if (m_time_count < 0) {
            m_timer.Stop();
        }
    }
};

class CountdownFrame : public wxFrame {
public:
    CountdownFrame()
        : wxFrame(nullptr, wxID_ANY, wxString::FromUTF8("倒计时器"),
                  wxDefaultPosition, wxSize(360, 300),
                  wxCAPTION | wxMINIMIZE_BOX),
          m_timer(this, ID_Timer) {
        wxPanel* panel = new wxPanel(this);
        m_panel = panel;

        wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
        panel->SetSizer(mainSizer);

        // 模式选择
        wxString modes[] = {wxString::FromUTF8("分钟倒计时"),
                            wxString::FromUTF8("结束时间")};
        m_mode = new wxRadioBox(panel, wxID_ANY, wxString::FromUTF8("输入模式"),
                                wxDefaultPosition, wxDefaultSize,
                                WXSIZEOF(modes), modes, 1, wxRA_SPECIFY_COLS);
        mainSizer->Add(m_mode, 0, wxALL | wxEXPAND, 10);

        // 时间选择器，居中展开
        m_timePicker = new wxTimePickerCtrl(panel, wxID_ANY);
        int sizer_flag = wxALIGN_CENTER;
        // wxEXPAND | wxALIGN_CENTER | wxLEFT | wxRIGHT;

        mainSizer->Add(m_timePicker, 0, sizer_flag, 20);

        /*
        // 分钟输入控件，居中展开
        m_minuteCtrl = new wxSpinCtrl(panel, wxID_ANY,
                                      wxString::FromUTF8("1"),
                                      wxDefaultPosition, wxDefaultSize,
                                      wxSP_ARROW_KEYS, 1, 1440, 1);
        */
        wxString presets[] = {"1", "5", "10", "15", "20", "30", "60"};
        m_minuteCtrl = new wxComboBox(panel, wxID_ANY, "5", wxPoint(20, 110),
                                      wxSize(80, 25), WXSIZEOF(presets),
                                      presets, wxCB_DROPDOWN);

        mainSizer->Add(m_minuteCtrl, 0, sizer_flag, 20);

        // 倒计时显示
        m_display =
            new wxStaticText(panel, wxID_ANY, wxString::FromUTF8("00:00:00"),
                             wxDefaultPosition, wxDefaultSize, wxALIGN_CENTER);
        m_display->SetFont(wxFont(24, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL,
                                  wxFONTWEIGHT_BOLD));
        mainSizer->Add(m_display, 1, wxALL | wxEXPAND, 10);

        // 按钮区
        wxBoxSizer* btnSizer = new wxBoxSizer(wxHORIZONTAL);
        m_btnStart = new wxButton(panel, wxID_ANY, wxString::FromUTF8("开始"));
        btnSizer->Add(m_btnStart, 0, wxRIGHT, 20);
        wxButton* btnExit =
            new wxButton(panel, wxID_CANCEL, wxString::FromUTF8("退出"));
        btnSizer->Add(btnExit, 0);
        mainSizer->Add(btnSizer, 0, wxALIGN_CENTER | wxBOTTOM, 15);

        // 事件绑定
        m_mode->Bind(wxEVT_RADIOBOX, &CountdownFrame::OnModeSwitch, this);
        m_btnStart->Bind(wxEVT_BUTTON, &CountdownFrame::OnStartStop, this);
        btnExit->Bind(wxEVT_BUTTON, [this](wxCommandEvent&) {
            int answer =
                wxMessageBox(wxString::FromUTF8("是否退出程序?"),
                             wxString::FromUTF8("确认"), wxYES_NO, this);
            if (answer == wxYES)
                Close();
        });
        Bind(wxEVT_TIMER, &CountdownFrame::OnTimer, this, ID_Timer);

        // 初始布局
        OnModeSwitch(wxCommandEvent());

        Centre();
        Show();
    }

private:
    void OnModeSwitch(wxCommandEvent&) {
        bool timeMode = (m_mode->GetSelection() == 0);
        m_timePicker->Show(!timeMode);
        m_minuteCtrl->Show(timeMode);
        m_timePicker->Enable(true);
        m_minuteCtrl->Enable(true);
        // 关键：重新布局以应用居中对齐
        m_panel->Layout();
    }

    void OnStart() {
        m_display->SetLabel(wxString::FromUTF8("00:00:00"));
        m_btnStart->SetLabel(wxString::FromUTF8("开始"));
        m_timePicker->Enable(true);
        m_minuteCtrl->Enable(true);
    }

    void OnStartStop(wxCommandEvent&) {
        if (m_timer.IsRunning()) {
            m_timer.Stop();
            m_display->SetLabel(wxString::FromUTF8("00:00:00"));
            m_btnStart->SetLabel(wxString::FromUTF8("开始"));
            m_timePicker->Enable(true);
            m_minuteCtrl->Enable(true);
            OnModeSwitch(wxCommandEvent());
            return;
        }

        // 计算倒计时秒数
        if (m_mode->GetSelection() == 1) {
            wxDateTime now = wxDateTime::Now();
            wxDateTime target = m_timePicker->GetValue();
            target.SetYear(now.GetYear());
            target.SetMonth(now.GetMonth());
            target.SetDay(now.GetDay());
            if (target < now)
                target += wxTimeSpan::Days(1);
            m_total = (target - now).GetSeconds().ToLong();
            m_inputDesc = target.FormatTime();
        } else {

            int mins = 1;
            m_minuteCtrl->GetValue().ToInt(&mins, 10);
            m_total = mins * 60;
            m_inputDesc = wxString::Format(wxString::FromUTF8("%d 分钟"), mins);
        }
        //m_total = 10;
        if (m_total <= 0)
            return;

        m_remain = m_total;
        m_timePicker->Enable(false);
        m_minuteCtrl->Enable(false);
        m_btnStart->SetLabel(wxString::FromUTF8("停止"));
        m_timer.Start(1000, wxTIMER_CONTINUOUS);
    }

    void OnTimer(wxTimerEvent&) {
        if (--m_remain < 0) {
            m_timer.Stop();
            // Hide();
            auto* rf = new ResultFrame(m_total, m_inputDesc);
            /*
            rf->Bind(wxEVT_CLOSE_WINDOW, [this](wxCloseEvent& event) {
                // 用户点击“确定”后，关闭结果并恢复主窗口
                this->Show();
            });*/
            rf->Show();

            OnModeSwitch(wxCommandEvent());
            OnStart();
            return;
        }
        int h = m_remain / 3600;
        int m = (m_remain % 3600) / 60;
        int s = m_remain % 60;
        m_display->SetLabel(
            wxString::Format(wxString::FromUTF8("%02d:%02d:%02d"), h, m, s));
        m_panel->Layout();
    }

    wxPanel* m_panel;
    wxRadioBox* m_mode;
    wxTimePickerCtrl* m_timePicker;
    // wxSpinCtrl*       m_minuteCtrl;
    wxComboBox* m_minuteCtrl;
    wxStaticText* m_display;
    wxButton* m_btnStart;
    wxTimer m_timer;

    int m_total{0};
    int m_remain{0};
    wxString m_inputDesc;
};

class MyApp : public wxApp {
public:
    bool OnInit() override {
        new CountdownFrame();
        return true;
    }
};

wxIMPLEMENT_APP(MyApp);
