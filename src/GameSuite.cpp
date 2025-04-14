#include <string.h>

#include <wx/wx.h>
#include <wx/bitmap.h>
#include <wx/bmpbuttn.h>
#include <wx/image.h>

#include "AbstractGameGUI.hpp"
#include "TTTGameGUI.hpp"
#include "RPSGameGUI.hpp"

#include "DDSGameController.hpp"

class MyApp : public wxApp
{
public:
    bool OnInit() override;
};


wxIMPLEMENT_APP(MyApp);

class MyFrame : public wxFrame
{
public:
    MyFrame();
 
private:
    void setupWaitingDisplay();
    void OnWaitingTimerComplete(wxTimerEvent& event);
    void OnExit(wxCommandEvent& event);
    
    AbstractGame* ttt_panel_;
    AbstractGame* rps_panel_;
    AbstractGame* current_panel_;
    wxPanel* waiting_panel_;
    wxBoxSizer* frameSizer;
    wxTimer* timer;
    int currentGame;
};

bool MyApp::OnInit()
{
    MyFrame *frame = new MyFrame();
    wxImage::AddHandler(new wxPNGHandler);
    wxInitAllImageHandlers();
    frame->Show();
    return true;
}

enum
{
    ID_Hello = 1
};

MyFrame::MyFrame()
        : wxFrame(nullptr, wxID_ANY, "DDS Game Suite Early Build", wxDefaultPosition, wxSize(600, 450))
{
    timer = new wxTimer(this, wxID_ANY);
    Bind(wxEVT_TIMER, &MyFrame::OnWaitingTimerComplete, this, timer->GetId());
    CreateStatusBar();
    
    waiting_panel_ = new wxPanel(this);
    setupWaitingDisplay();
    
    ttt_panel_ = new TTTGameGUI(this, waiting_panel_, timer);
    rps_panel_ = new RPSGameGUI(this, waiting_panel_, timer);
    
    waiting_panel_->Hide();
    ttt_panel_->Hide();
    rps_panel_->Hide();

    current_panel_ = rps_panel_;

    frameSizer = new wxBoxSizer(wxVERTICAL);
    frameSizer->Add(ttt_panel_, 1, wxEXPAND);
    frameSizer->Add(rps_panel_, 1, wxEXPAND);
    frameSizer->Add(waiting_panel_, 1, wxEXPAND);
    SetSizer(frameSizer);
    Layout(); // Forces the layout to update visually


    current_panel_->waitingDisplayEnter();

    currentGame = 1; // Set currentGame to tic tac toe

    Bind(wxEVT_MENU, &MyFrame::OnExit, this, wxID_EXIT);
}

void MyFrame::setupWaitingDisplay()
{
    waiting_panel_->SetBackgroundColour(*wxLIGHT_GREY);
    wxStaticText* welcomeText = new wxStaticText(waiting_panel_, wxID_ANY, "Waiting On Response from other player");
    welcomeText->SetFont(wxFontInfo(16).Bold());

    wxBoxSizer* waitingSizer = new wxBoxSizer(wxVERTICAL);

    // Add top spacer
    waitingSizer->AddStretchSpacer(1);

    // Center the text horizontally
    waitingSizer->Add(welcomeText, 0, wxALIGN_CENTER | wxBOTTOM, 20);

    // Add bottom spacer to balance the layout
    waitingSizer->AddStretchSpacer(1);

    waiting_panel_->SetSizer(waitingSizer);
    waiting_panel_->Layout();
}

void MyFrame::OnWaitingTimerComplete(wxTimerEvent& event)
{
    current_panel_->waitingDisplayExit();
    Layout();
}

void MyFrame::OnExit(wxCommandEvent& event)
{
    ttt_panel_->Hide();
    waiting_panel_->Hide();

    delete waiting_panel_;
    delete timer;
    delete ttt_panel_;

    Close(true);
}