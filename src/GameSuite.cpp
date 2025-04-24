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
    void setGame(int game_id);
    void OnExit(wxCommandEvent& event);
    
    AbstractGame* ttt_panel_;
    AbstractGame* rps_panel_;
    AbstractGame* current_panel_;

    wxPanel* waiting_panel_;
    wxStaticText* waitingText_;

    wxPanel* game_selection_panel_;
    wxBoxSizer* frameSizer;
    wxTimer* timer;
    int currentGame;

    GameUser* game_user_;
};

bool MyApp::OnInit()
{
    wxInitAllImageHandlers();

    MyFrame *frame = new MyFrame();
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
    waitingText_ = new wxStaticText(waiting_panel_, wxID_ANY, "Waiting On Response from other player");
    waitingText_->SetForegroundColour(wxColor(255,255,255,255));
    setupWaitingDisplay();

    game_selection_panel_ = new wxPanel(this);
    setupGameSelection();

    game_user_ = new GameUser();
    
    ttt_panel_ = new TTTGameGUI(this, waiting_panel_, timer, game_user_);
    rps_panel_ = new RPSGameGUI(this, waiting_panel_, timer, game_user_, 3);
    
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

    waitingText_->SetLabel("waiting for people to join...");
    waiting_panel_->Show();

    // starts a thread for the initialization of game_user
    std::thread([this]() {
        game_user_->init();
        
        // Return to GUI thread to continue
        wxTheApp->CallAfter([this]() {
            // if you are first to join show game selection panel
            if (game_user_->first_){
                waiting_panel_->Hide();
                waitingText_->SetLabel("waiting for opp...");
                game_selection_panel_->Show();
            }  
            else{
                waitingText_->SetLabel("waiting for game selection to be made...");

                // starts a thread for waiting for other player to select game
                std::thread([this]() {
    
                    while (!game_user_->hasReceivedGameChoice()) {
                        std::this_thread::sleep_for(std::chrono::milliseconds(500));
                    }

                    wxTheApp->CallAfter([this]() {
                        waiting_panel_->Hide();
                        setGame(game_user_->currentGameID());
                    });
    
                }).detach();
            }
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
    int game_id = event.GetId()-200;
    // will publish the selection and then move to next screen
    if (game_user_->selectGame(game_id)){
        setGame(game_id);
    }
    return;
}

void MyFrame::setGame(int game_id){
    if (game_id == 1) current_panel_ = rps_panel_;
    else if (game_id == 2) current_panel_ = ttt_panel_;
    Layout();
    game_selection_panel_->Hide();

    waitingText_->SetLabel("waiting for opp...");
    //  if first, dont show waiting display, and show current game panel
    if (game_user_->first_){
        current_panel_->waitingDisplayExit();
        Layout();
        
    }// if your not first to move, you will wait
    else{
        current_panel_->waitingDisplayEnter();
    }
}

void MyFrame::setupWaitingDisplay()
{
    waiting_panel_->SetBackgroundColour(*wxBLACK);
    wxBoxSizer* waitingSizer = new wxBoxSizer(wxVERTICAL);

    // Center the text horizontally
    waitingSizer->AddStretchSpacer(); // Pushes content down
    waitingSizer->Add(waitingText_, 0, wxALIGN_CENTER | wxALL, 0);
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
    delete rps_panel_;

    Close(true);
}