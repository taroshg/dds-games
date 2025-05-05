#ifndef MAINFRAME_HPP
#define MAINFRAME_HPP

#include <string.h>

#include <wx/wx.h>
#include <wx/bitmap.h>
#include <wx/bmpbuttn.h>
#include <wx/image.h>
#include <thread>

#include "DDSGameController.hpp"

#include "AbstractGameGUI.hpp"
#include "TTTGameGUI.hpp"
#include "RPSGameGUI.hpp"
#include "C4GameGUI.hpp"
#include "ChessGameGUI.hpp"

#include "GameUser.hpp"
#include "WaitingPanel.hpp"
#include "UsernamePanel.hpp"

int OPP_JOIN_WAIT = 10;

class MainFrame : public wxFrame
{
private:
    AbstractGamePanel* ttt_panel_;
    AbstractGamePanel* rps_panel_;
    AbstractGamePanel* c4_panel_;
    AbstractGamePanel* chess_panel_;

    WaitingPanel* waiting_panel_;
    wxPanel* game_selection_panel_;

    wxBoxSizer* frameSizer;

    GameUser* game_user_;

    bool initalized_;
    bool username_set_;
    UsernamePanel* username_panel_;

public:
    MainFrame(): wxFrame(nullptr, wxID_ANY, "DDS Game Suite Early Build", wxDefaultPosition, wxSize(600, 450))
    {
        CreateStatusBar();

        waiting_panel_ = new WaitingPanel(this, [this](int screen_id){ setScreen(screen_id); });
        username_panel_ = new UsernamePanel(this, [this](int screen_id){ setScreen(screen_id); });

        game_selection_panel_ = new wxPanel(this);
        setupGameSelection();

        game_user_ = new GameUser();
        ttt_panel_ = new TTTGameGUI(this, waiting_panel_, [this](int screen_id){ setScreen(screen_id); }, game_user_);
        rps_panel_ = new RPSGameGUI(this, waiting_panel_, [this](int screen_id){ setScreen(screen_id); }, game_user_);
        c4_panel_ = new C4GameGUI(this, waiting_panel_, [this](int screen_id){ setScreen(screen_id); }, game_user_);
        chess_panel_ = new ChessGameGUI(this, waiting_panel_, [this](int screen_id){ setScreen(screen_id); }, game_user_);

        waiting_panel_->Hide();
        game_selection_panel_->Hide();
        ttt_panel_->Hide();
        rps_panel_->Hide();
        c4_panel_->Hide();
        chess_panel_->Hide();

        frameSizer = new wxBoxSizer(wxVERTICAL);
        frameSizer->Add(ttt_panel_, 1, wxEXPAND);
        frameSizer->Add(rps_panel_, 1, wxEXPAND);
        frameSizer->Add(c4_panel_, 1, wxEXPAND);
        frameSizer->Add(chess_panel_, 1, wxEXPAND);
        frameSizer->Add(waiting_panel_, 1, wxEXPAND);
        frameSizer->Add(game_selection_panel_, 1, wxEXPAND);
        frameSizer->Add(username_panel_, 1, wxEXPAND);
        SetSizer(frameSizer);
        Layout(); // Forces the layout to update visually

        // shows the username panel first thing!
        username_panel_->Show();
        Layout();

        initalized_ = false;
        username_set_ = false;
        // updates waiting panel to show how many seconds we are waiting for
        std::thread([this](){
            while(!username_set_){
                username_set_ = username_panel_->getUsernameStatus();
                std::this_thread::sleep_for(std::chrono::seconds(1));
            }

            // after username is set, hide username panel and show waiting panel if needed
            wxTheApp->CallAfter([this](){
                username_panel_->Hide();
                if (!initalized_){
                    waiting_panel_->Text("waiting for people to join...");
                    waiting_panel_->Show();
                }
                Layout();
            });

            auto start = std::chrono::steady_clock::now();
            while(!initalized_){
                std::this_thread::sleep_for(std::chrono::milliseconds(500));

                std::stringstream ss;
                
                auto timer = (std::chrono::steady_clock::now() - start);
                int seconds = std::chrono::duration_cast<std::chrono::seconds>(timer).count();
    
                ss << "waiting for opp to join (" << seconds << " seconds)";
                waiting_panel_->Text(ss.str());
    
                wxTheApp->CallAfter([this](){
                    Refresh();
                });
            }
        }).detach();

        // starts a thread for the initialization of game_user, to show game selection panel
        std::thread([this]() {
            // wait for username to be set before showing game selection panel
            while(!username_set_) {
                std::this_thread::sleep_for(std::chrono::seconds(1));
            }

            initalized_ = game_user_->init();

            // Return to GUI thread to continue
            wxTheApp->CallAfter([this]() {
                waiting_panel_->Hide();
                waiting_panel_->Text("waiting for opp...");
                game_selection_panel_->Show();
                Layout();
            });
        
        }).detach();

        Bind(wxEVT_MENU, &MainFrame::OnExit, this, wxID_EXIT);
        
    }

    void setScreen(int screen_id){
        std::cout << "running screen: " << screen_id << std::endl;

        // hide all panels
        waiting_panel_->Hide();
        game_selection_panel_->Hide();
        rps_panel_->Hide();
        ttt_panel_->Hide();
        c4_panel_->Hide();
        chess_panel_->Hide();

        // update game_user screen, which broadcasts your current screen to everyone
        game_user_->setScreen(screen_id);


        if(screen_id == SCREEN_GAME_SELECTION) {
            game_selection_panel_->Show();
            Layout();
            return;
        }

        if (screen_id == SCREEN_RPS){
            waiting_panel_->Text("waiting for opp..");
            waiting_panel_->Show();
            Layout();
            
            std::thread([this]() {
                auto start = std::chrono::steady_clock::now();
                auto end = start + std::chrono::seconds(OPP_JOIN_WAIT);

                while(game_user_->readOppGameChoice() != SCREEN_RPS && std::chrono::steady_clock::now() < end){
                    std::this_thread::sleep_for(std::chrono::milliseconds(500));

                    std::stringstream ss;
                    
                    auto timer = (std::chrono::steady_clock::now() - start);
                    int seconds = OPP_JOIN_WAIT - std::chrono::duration_cast<std::chrono::seconds>(timer).count();

                    ss << "waiting for opp (" << seconds << " seconds)";
                    waiting_panel_->Text(ss.str());

                    wxTheApp->CallAfter([this](){
                        Refresh();
                    });
                }
                    

                if (std::chrono::steady_clock::now() < end){
                    wxTheApp->CallAfter([this](){
                        waiting_panel_->Hide();
                        rps_panel_->setOppActive(true);
                        rps_panel_->Show();
                        rps_panel_->updateDisplay();
                        Layout();
                        if (!game_user_->first_)
                            rps_panel_->waitingMoveEnter();                     
                    });
                }
                else {
                    setScreen(0);
                    SetStatusText("waiting for opp rock-paper-scissors, timed out!");
                }
                
            }).detach();

            return;
        } 
        if (screen_id == SCREEN_TTT){
            waiting_panel_->Text("waiting for opp..");
            waiting_panel_->Show();
            Layout();
            
            std::thread([this]() {
                auto start = std::chrono::steady_clock::now();
                auto end = start + std::chrono::seconds(OPP_JOIN_WAIT);

                while(game_user_->readOppGameChoice() != SCREEN_TTT && std::chrono::steady_clock::now() < end){
                    std::this_thread::sleep_for(std::chrono::milliseconds(500));

                    std::stringstream ss;
                    
                    auto timer = (std::chrono::steady_clock::now() - start);
                    int seconds = OPP_JOIN_WAIT - std::chrono::duration_cast<std::chrono::seconds>(timer).count();

                    ss << "waiting for opp (" << seconds << " seconds)";
                    waiting_panel_->Text(ss.str());

                    wxTheApp->CallAfter([this](){
                        Refresh();
                    });
                    
                }
                    
                
                if (std::chrono::steady_clock::now() < end){
                    wxTheApp->CallAfter([this](){
                        waiting_panel_->Hide();
                        ttt_panel_->setOppActive(true);
                        ttt_panel_->Show();
                        Layout();
                        if (!game_user_->first_)
                            ttt_panel_->waitingMoveEnter();                     
                    });
                }
                else {
                    setScreen(0);
                    SetStatusText("waiting for opp in tic-tac-toe, timed out!");
                }

            }).detach();

            return;
        }
        if (screen_id == SCREEN_C4){
            waiting_panel_->Text("waiting for opp..");
            waiting_panel_->Show();
            Layout();
            
            std::thread([this]() {
                auto start = std::chrono::steady_clock::now();
                auto end = start + std::chrono::seconds(OPP_JOIN_WAIT);

                while(game_user_->readOppGameChoice() != SCREEN_C4 && std::chrono::steady_clock::now() < end){
                    std::this_thread::sleep_for(std::chrono::milliseconds(500));
                    
                    std::stringstream ss;
                    
                    auto timer = (std::chrono::steady_clock::now() - start);
                    int seconds = OPP_JOIN_WAIT - std::chrono::duration_cast<std::chrono::seconds>(timer).count();

                    ss << "waiting for opp (" << seconds << " seconds)";
                    waiting_panel_->Text(ss.str());

                    wxTheApp->CallAfter([this](){
                        Refresh();
                    });
                }
                
                if (std::chrono::steady_clock::now() < end){
                    wxTheApp->CallAfter([this](){
                        waiting_panel_->Hide();
                        c4_panel_->setOppActive(true);
                        c4_panel_->Show();
                        Layout();
                        if (!game_user_->first_)
                            c4_panel_->waitingMoveEnter();                     
                    });
                }
                else {
                    setScreen(0);
                    SetStatusText("waiting for opp in connect4, timed out!");
                }

            }).detach();

            return;
        }
        if(screen_id == SCREEN_CHESS){
            waiting_panel_->Text("waiting for opp..");
            waiting_panel_->Show();
            Layout();
            
            std::thread([this]() {
                auto start = std::chrono::steady_clock::now();
                auto end = start + std::chrono::seconds(OPP_JOIN_WAIT);

                while(game_user_->readOppGameChoice() != SCREEN_CHESS && std::chrono::steady_clock::now() < end){
                    std::this_thread::sleep_for(std::chrono::milliseconds(500));
                    
                    std::stringstream ss;
                    
                    auto timer = (std::chrono::steady_clock::now() - start);
                    int seconds = OPP_JOIN_WAIT - std::chrono::duration_cast<std::chrono::seconds>(timer).count();

                    ss << "waiting for opp (" << seconds << " seconds)";
                    waiting_panel_->Text(ss.str());

                    wxTheApp->CallAfter([this](){
                        Refresh();
                    });
                }
                
                if (std::chrono::steady_clock::now() < end){
                    wxTheApp->CallAfter([this](){
                        waiting_panel_->Hide();
                        chess_panel_->setOppActive(true);
                        chess_panel_->Show();
                        Layout();
                        if (!game_user_->first_)
                            chess_panel_->waitingMoveEnter();                     
                    });
                }
                else {
                    setScreen(0);
                    SetStatusText("waiting for opp in chess, timed out!");
                }

            }).detach();
        }
    }
    
private:
    void setupGameSelection(){
        game_selection_panel_->SetBackgroundColour(wxColour(173, 216, 230)); // light blue

        // Main title
        wxStaticText *titleText = new wxStaticText(game_selection_panel_, wxID_ANY, "Welcome to the Game Lobby");
        titleText->SetFont(wxFontInfo(24).Bold().FaceName("Georgia"));
        titleText->SetForegroundColour(*wxBLACK);

        // Subtitle
        wxStaticText *subtitleText = new wxStaticText(game_selection_panel_, wxID_ANY, "Select the game you would like to play");
        subtitleText->SetFont(wxFontInfo(14).Italic().FaceName("Segoe UI"));
        subtitleText->SetForegroundColour(wxColour(80, 80, 80)); // gray

        wxGridSizer* gridSizer = new wxGridSizer(1, 2, 5, 5); // 3 rows, 3 columns, 5px padding

        wxBitmap rockButtonImage;
        rockButtonImage.LoadFile(wxT("./resources/rock.png"), wxBITMAP_TYPE_PNG);
        wxBitmap xButtonImage;
        xButtonImage.LoadFile(wxT("./resources/TTTlogo.png"), wxBITMAP_TYPE_PNG);
        wxBitmap rButtonImage;
        rButtonImage.LoadFile(wxT("./resources/red_dot.png"), wxBITMAP_TYPE_PNG);
        wxBitmap chessButtonImage;
        chessButtonImage.LoadFile(wxT("./resources/chess/wp.png"), wxBITMAP_TYPE_PNG);
        wxSize buttonTileSize = wxSize(128,128);
    
        wxBitmapButton* RPS = new wxBitmapButton(game_selection_panel_, 201, rockButtonImage, wxDefaultPosition, buttonTileSize);
        RPS->Bind(wxEVT_BUTTON, &MainFrame::selectionButtonClick, this);
        wxBitmapButton* TTT = new wxBitmapButton(game_selection_panel_, 202, xButtonImage, wxDefaultPosition, buttonTileSize);
        TTT->Bind(wxEVT_BUTTON, &MainFrame::selectionButtonClick, this);
        wxBitmapButton* C4 = new wxBitmapButton(game_selection_panel_, 203, rButtonImage, wxDefaultPosition, buttonTileSize);
        C4->Bind(wxEVT_BUTTON, &MainFrame::selectionButtonClick, this);
        wxBitmapButton* CHESS = new wxBitmapButton(game_selection_panel_, 204, chessButtonImage, wxDefaultPosition, buttonTileSize);
        CHESS->Bind(wxEVT_BUTTON, &MainFrame::selectionButtonClick, this);

        // LABEL MAKER
        auto makeLabeledButton = [&](wxBitmapButton *btn, const wxString &label) -> wxBoxSizer *
        {
            wxBoxSizer *sizer = new wxBoxSizer(wxVERTICAL);
            wxStaticText *text = new wxStaticText(game_selection_panel_, wxID_ANY, label);
            text->SetFont(wxFontInfo(10).Bold());
            text->SetForegroundColour(*wxBLACK);
            text->Wrap(125);
            sizer->Add(btn, 0, wxALIGN_CENTER);
            sizer->AddSpacer(5);
            sizer->Add(text, 0, wxALIGN_CENTER);
            return sizer;
        };

        // RPS and TTT labels
        wxBoxSizer *topRowSizer = new wxBoxSizer(wxHORIZONTAL);
        topRowSizer->Add(makeLabeledButton(RPS, "Rock Paper Scissors"), 0, wxALIGN_CENTER | wxRIGHT, 40);
        topRowSizer->Add(makeLabeledButton(TTT, "Tic Tac Toe"), 0, wxALIGN_CENTER);

        // Connect four label
        wxBoxSizer *bottomRowSizer = new wxBoxSizer(wxHORIZONTAL);
        bottomRowSizer->Add(makeLabeledButton(C4, "Connect Four"), 0, wxALIGN_CENTER | wxRIGHT, 40);
        bottomRowSizer->Add(makeLabeledButton(CHESS, "Chess"), 0, wxALIGN_CENTER);

        // Lay out for the main screen
        wxBoxSizer *mainSizer = new wxBoxSizer(wxVERTICAL);
        mainSizer->AddSpacer(20);
        mainSizer->Add(titleText, 0, wxALIGN_CENTER | wxBOTTOM, 5);
        mainSizer->Add(subtitleText, 0, wxALIGN_CENTER | wxBOTTOM, 20);
        mainSizer->Add(topRowSizer, 0, wxALIGN_CENTER | wxBOTTOM, 30);
        mainSizer->Add(bottomRowSizer, 0, wxALIGN_CENTER);
        mainSizer->AddStretchSpacer();
    
        game_selection_panel_->SetSizer(mainSizer);
        game_selection_panel_->Layout();
    }
    void selectionButtonClick(wxCommandEvent& event){
        int game_id = event.GetId()-200;
        setScreen(game_id);
        return;
    }
    
    void OnExit(wxCommandEvent& event){    
        delete waiting_panel_;
        delete ttt_panel_;
        delete rps_panel_;
        delete c4_panel_;
        delete game_selection_panel_;
    
        Close(true);
    }

};

#endif

