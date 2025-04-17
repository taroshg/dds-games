#include <string.h>

#include <wx/wx.h>
#include <wx/bitmap.h>
#include <wx/bmpbuttn.h>
#include <wx/image.h>

#include "AbstractGameGUI.hpp"
#include "TTTGameGUI.hpp"
#include "RPSGameGUI.hpp"
#include "GameUser.hpp"

#include "DDSGameController.hpp"

#include <thread>
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
    void setupGameSelection();
    void selectionButtonClick(wxCommandEvent& event);
    void OnExit(wxCommandEvent& event);
    
    AbstractGame* ttt_panel_;
    AbstractGame* rps_panel_;
    AbstractGame* current_panel_;
    wxPanel* waiting_panel_;
    wxPanel* game_selection_panel_;
    wxBoxSizer* frameSizer;
    wxTimer* timer;
    int currentGame;

    GameUser* game_user_;
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
    game_selection_panel_ = new wxPanel(this);
    setupWaitingDisplay();
    setupGameSelection();
    
    ttt_panel_ = new TTTGameGUI(this, waiting_panel_, timer);
    rps_panel_ = new RPSGameGUI(this, waiting_panel_, timer, 3);
    
    waiting_panel_->Hide();
    game_selection_panel_->Hide();
    ttt_panel_->Hide();
    rps_panel_->Hide();

    frameSizer = new wxBoxSizer(wxVERTICAL);
    frameSizer->Add(ttt_panel_, 1, wxEXPAND);
    frameSizer->Add(rps_panel_, 1, wxEXPAND);
    frameSizer->Add(waiting_panel_, 1, wxEXPAND);
    frameSizer->Add(game_selection_panel_, 1, wxEXPAND);
    SetSizer(frameSizer);
    Layout(); // Forces the layout to update visually

    waiting_panel_->Show();
    // starts a thread for the initialization of game_user
    std::thread([this]() {
        game_user_ = new GameUser();
        game_user_->init(); // This blocks, but is in a thread
    
        // Return to GUI thread to continue
        wxTheApp->CallAfter([this]() {
            waiting_panel_->Hide();
            game_selection_panel_->Show();
            Layout();
        });
    
    }).detach();

    Bind(wxEVT_MENU, &MyFrame::OnExit, this, wxID_EXIT);
}

void MyFrame::setupGameSelection(){
    wxGridSizer* gridSizer = new wxGridSizer(1, 2, 5, 5); // 3 rows, 3 columns, 5px padding

    wxBitmap rockButtonImage;
    rockButtonImage.LoadFile(wxT("./resources/rock.png"), wxBITMAP_TYPE_PNG);
    wxBitmap xButtonImage;
    xButtonImage.LoadFile(wxT("./resources/X@0,25x.png"), wxBITMAP_TYPE_PNG);
    wxSize buttonTileSize = wxSize(100,100);

    wxBitmapButton* RPS = new wxBitmapButton(game_selection_panel_, 201, rockButtonImage, wxDefaultPosition, buttonTileSize);
    RPS->Bind(wxEVT_BUTTON, &MyFrame::selectionButtonClick, this);
    wxBitmapButton* TTT = new wxBitmapButton(game_selection_panel_, 202, xButtonImage, wxDefaultPosition, buttonTileSize);
    TTT->Bind(wxEVT_BUTTON, &MyFrame::selectionButtonClick, this);

    gridSizer->Add(RPS, 0, wxALIGN_CENTER , 5);
    gridSizer->Add(TTT, 0, wxALIGN_CENTER , 5);

    // Wrap gridSizer inside a box sizer to center it
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

    wxStaticText* infoText = new wxStaticText(game_selection_panel_, wxID_ANY, "Choose a game");
    infoText->SetFont(wxFontInfo(12).Bold());
    mainSizer->Add(infoText, 0, wxALIGN_CENTER | wxTOP, 20); // Centers grid

    mainSizer->AddStretchSpacer(); // Pushes content down
    mainSizer->Add(gridSizer, 0, wxALIGN_CENTER | wxALL, 20); // Centers grid
    mainSizer->AddStretchSpacer(); // Pushes content up

    game_selection_panel_->SetSizer(mainSizer);
    game_selection_panel_->Layout();
}

void MyFrame::selectionButtonClick(wxCommandEvent& event) {
    int id = event.GetId()-200;

    if(id == 1) // Player chose RPS
    {
        current_panel_ = rps_panel_;
    }
    else if (id == 2) // Player chose TTT
    {
        current_panel_ = ttt_panel_;
    }
    Layout();
    game_selection_panel_->Hide();
    current_panel_->waitingDisplayEnter();
}

void MyFrame::setupWaitingDisplay()
{
    waiting_panel_->SetBackgroundColour(*wxLIGHT_GREY);
    wxStaticText* welcomeText = new wxStaticText(waiting_panel_, wxID_ANY, "Waiting On Response from other player");

    wxBoxSizer* waitingSizer = new wxBoxSizer(wxVERTICAL);

    // Center the text horizontally
    waitingSizer->AddStretchSpacer(); // Pushes content down
    waitingSizer->Add(welcomeText, 0, wxALIGN_CENTER | wxALL, 0);
    waitingSizer->AddStretchSpacer(); // Pushes content up

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