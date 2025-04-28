#ifndef WAITING_PANEL_HPP
#define WAITING_PANEL_HPP

#include <wx/wx.h>
#include <string.h>

class WaitingPanel : public wxPanel
{
private:
    wxStaticText* waiting_text_;
    std::function<void(int)> setScreen_;
public:
    WaitingPanel(wxFrame* parent, std::function<void(int)> setScreen): wxPanel(parent, wxID_ANY) {
        this->SetBackgroundColour(wxColour(0, 0, 0, 150));
        wxBoxSizer* waiting_sizer = new wxBoxSizer(wxVERTICAL);
        waiting_text_ = new wxStaticText(this, wxID_ANY, "Waiting for selection...");
        waiting_text_->SetForegroundColour(*wxWHITE);
        waiting_sizer->AddStretchSpacer();
        waiting_sizer->Add(waiting_text_, 0, wxALIGN_CENTER | wxALL, 20);
        waiting_sizer->AddStretchSpacer();
        this->SetSizer(waiting_sizer);
        this->Layout();

        this->setScreen_ = setScreen;
    }
    ~WaitingPanel() {}

    void Text(const std::string& text) {
        waiting_text_->SetLabel(text.c_str());
        Layout();
    }

    void onHomeButtonPress(wxCommandEvent& evt){
        this->setScreen_(0);
    }
};

#endif