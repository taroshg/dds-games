#ifndef USERNAME_PANEL_HPP
#define USERNAME_PANEL_HPP

#include <wx/wx.h>
#include <string.h>
#include <functional>

class UsernamePanel : public wxPanel
{
private:
    wxStaticText* username_text_;
    wxTextCtrl* username_field_;
    std::function<void(int)> setScreen_;
    bool entered_username_;
public:
    UsernamePanel(wxFrame* parent, std::function<void(int)> setScreen): wxPanel(parent, wxID_ANY) {
        this->SetBackgroundColour(wxColour(255, 255, 255, 150));
        wxBoxSizer* waiting_sizer = new wxBoxSizer(wxVERTICAL);
        username_text_ = new wxStaticText(this, wxID_ANY, "Enter your username");
        username_field_ = new wxTextCtrl(this, wxID_ANY);
        entered_username_ = false;

        username_text_->SetForegroundColour(*wxBLACK);
        waiting_sizer->AddStretchSpacer();
        waiting_sizer->Add(username_text_, 0, wxALIGN_CENTER | wxALL, 5);
        waiting_sizer->Add(username_field_, 0, wxALIGN_CENTER | wxALL, 5);

        wxButton* continue_btn = new wxButton(this, wxID_ANY, "Continue");
        continue_btn->Bind(wxEVT_BUTTON, &UsernamePanel::OnContinue, this);
        waiting_sizer->Add(continue_btn, 0, wxALIGN_CENTER | wxALL, 10);

        waiting_sizer->AddStretchSpacer();
        this->SetSizer(waiting_sizer);
        this->Layout();

        this->setScreen_ = setScreen;
    }
    ~UsernamePanel() {}

    void Text(const std::string& text) {
        username_text_->SetLabel(text.c_str());
        Layout();
    }

    std::string GetUsername() const {
        return std::string(username_field_->GetValue().mb_str());
    }
    
    void OnContinue(wxCommandEvent&) {
        entered_username_ = true;
    }

    bool getUsernameStatus()
    {
        return entered_username_;
    }
    
};

#endif