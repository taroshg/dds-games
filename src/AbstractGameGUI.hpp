#ifndef ABSTRACTGAMEGUI_HPP
#define ABSTRACTGAMEGUI_HPP

#include <wx/wx.h>
#include <string.h>

#include "GameUser.hpp"
#include "WaitingPanel.hpp"
#include "GameMessage.hpp"

enum
{
    SCREEN_GAME_SELECTION = 0,
    SCREEN_RPS = 1,
    SCREEN_TTT = 2,
};

class AbstractGamePanel : public wxPanel 
{
protected:
    GameUser* game_user_;
    GameMessage* my_msg_;
    GameMessage* opp_msg_;

    WaitingPanel* waiting_panel_;
    wxFrame* parentFrame;
    wxPanel* home_panel_;
    std::function<void(int)> setScreen;
    int GAME_ID;

    bool is_opp_active_;

public:

    virtual void gameButtonClick(wxCommandEvent& event) = 0;
    virtual bool determineWinner() = 0;
    virtual std::string getFrameStatusText() = 0;
    virtual void updateDisplay() = 0;
    virtual void setupGame() = 0;
    

    AbstractGamePanel(wxFrame* parent, WaitingPanel* waitingPanel, std::function<void(int)> setScreen, GameUser* game_user) : 
        wxPanel(parent, wxID_ANY),
        my_msg_(new GameMessage()),
        opp_msg_(new GameMessage()),
        game_user_(game_user),
        waiting_panel_(waitingPanel),
        setScreen(setScreen),
        parentFrame(parent)
    {
        wxButton* HOME_BTN = new wxButton(this, wxID_ANY, "back", wxDefaultPosition);
        HOME_BTN->Bind(wxEVT_BUTTON, &AbstractGamePanel::onHomeButtonPress, this);
    }

    ~AbstractGamePanel() {}

    // Note: make sure the that opp_msg_ is updated before calling this
    bool oppActive(){
        if (opp_msg_->game_id() == GAME_ID) return true;

        setupGame();

        std::cout << "opp not in game" << std::endl;
        wxMessageBox("opp not in game");

        return false;
    }

    void setOppActive(bool active){
        is_opp_active_ = active;
    }

    void waitingMoveEnter() {
        Hide();
        setFrameStatusText("Listening...");
        waiting_panel_->Text("waiting for move..");
        waiting_panel_->Show();
        waiting_panel_->Fit();
        waiting_panel_->Layout();
        parentFrame->Layout();

        std::thread([this]() {
            while(!game_user_->messageAvailable())
                std::this_thread::sleep_for(std::chrono::milliseconds(500));
            
            wxTheApp->CallAfter([this](){
                waitingMoveExit();
                updateDisplay();
            });
        }).detach();
    }

    void waitingMoveExit() {
        waiting_panel_->Hide();
        setFrameStatusText(getFrameStatusText());
        Show();
        parentFrame->Layout();
    }

    void onHomeButtonPress(wxCommandEvent& evt){
        this->goHome();
    }

    void goHome(){
        // broadcast you have left the game!
        my_msg_->game_id(0);
        game_user_->sendGameMessage(my_msg_);

        // reset messages for next time
        my_msg_ = new GameMessage();
        opp_msg_ = new GameMessage();

        setScreen(SCREEN_GAME_SELECTION);
    }

    void setFrameStatusText(std::string input) {
        parentFrame->SetStatusText(input);
    }
};
#endif