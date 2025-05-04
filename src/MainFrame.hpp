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
    WaitingPanel* waiting_panel_;
    UsernamePanel* username_panel_;
    wxPanel* game_selection_panel_;

    wxBoxSizer* frameSizer;

    GameUser* game_user_;

    bool initalized_;

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

        waiting_panel_->Hide();
        username_panel_->Hide();
        game_selection_panel_->Hide();
        ttt_panel_->Hide();
        rps_panel_->Hide();
        c4_panel_->Hide();

        frameSizer = new wxBoxSizer(wxVERTICAL);
        frameSizer->Add(ttt_panel_, 1, wxEXPAND);
        frameSizer->Add(rps_panel_, 1, wxEXPAND);
        frameSizer->Add(c4_panel_, 1, wxEXPAND);
        frameSizer->Add(waiting_panel_, 1, wxEXPAND);
        frameSizer->Add(game_selection_panel_, 1, wxEXPAND);
        frameSizer->Add(username_panel_, 1, wxEXPAND);
        SetSizer(frameSizer);
        Layout(); // Forces the layout to update visually

        username_panel_->Show();
        Layout();

        waiting_panel_->Text("waiting for people to join...");

        initalized_ = false;

        // updates waiting panel to show how many seconds we are waiting for
        std::thread([this](){
            auto start = std::chrono::steady_clock::now();
            while(!username_panel_->getUsernameStatus()) {
                std::this_thread::sleep_for(std::chrono::milliseconds(500));

                std::stringstream ss;
                
                auto timer = (std::chrono::steady_clock::now() - start);
                int seconds = std::chrono::duration_cast<std::chrono::seconds>(timer).count();
    
                wxTheApp->CallAfter([this](){
                    Refresh();
                });
            }
            username_panel_->Hide();
            game_user_->username = username_panel_->GetUsername();
            waiting_panel_->Show();
            Layout();
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

        // starts a thread for the initialization of game_user
        std::thread([this]() {
            initalized_ = game_user_->init();

            // Return to GUI thread to continue
            wxTheApp->CallAfter([this]() {
                // if you are first to join show game selection panel
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
    }
    
private:
    void setupGameSelection(){
        wxGridSizer* gridSizer = new wxGridSizer(1, 2, 5, 5); // 3 rows, 3 columns, 5px padding

        wxBitmap rockButtonImage;
        rockButtonImage.LoadFile(wxT("./resources/rock.png"), wxBITMAP_TYPE_PNG);
        wxBitmap xButtonImage;
        xButtonImage.LoadFile(wxT("./resources/TTTlogo.png"), wxBITMAP_TYPE_PNG);
        wxBitmap rButtonImage;
        rButtonImage.LoadFile(wxT("./resources/red_dot.png"), wxBITMAP_TYPE_PNG);
        wxSize buttonTileSize = wxSize(100,100);
    
        wxBitmapButton* RPS = new wxBitmapButton(game_selection_panel_, 201, rockButtonImage, wxDefaultPosition, buttonTileSize);
        RPS->Bind(wxEVT_BUTTON, &MainFrame::selectionButtonClick, this);
        wxBitmapButton* TTT = new wxBitmapButton(game_selection_panel_, 202, xButtonImage, wxDefaultPosition, buttonTileSize);
        TTT->Bind(wxEVT_BUTTON, &MainFrame::selectionButtonClick, this);
        wxBitmapButton* C4 = new wxBitmapButton(game_selection_panel_, 203, rButtonImage, wxDefaultPosition, buttonTileSize);
        C4->Bind(wxEVT_BUTTON, &MainFrame::selectionButtonClick, this);
    
        gridSizer->Add(RPS, 0, wxALIGN_CENTER , 5);
        gridSizer->Add(TTT, 0, wxALIGN_CENTER , 5);
        gridSizer->Add(C4, 0, wxALIGN_CENTER , 5);
    
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

