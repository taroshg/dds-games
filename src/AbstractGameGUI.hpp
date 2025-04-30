#ifndef ABSTRACTGAMEGUI_HPP
#define ABSTRACTGAMEGUI_HPP

#include <wx/wx.h>

#include <string.h>

#include "GameUser.hpp"
#include "WaitingPanel.hpp"
#include "./idl/GameMessage.hpp"

enum
{
    SCREEN_GAME_SELECTION = 0,
    SCREEN_RPS = 1,
    SCREEN_TTT = 2,
    SCREEN_C4 = 3,
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
    wxButton* back_btn_;
    std::function<void(int)> setScreen;
    int GAME_ID;

    bool is_opp_active_;

    void OnResize(wxSizeEvent& event) {
        if (waiting_panel_) {
            waiting_panel_->SetSize(GetSize());  // Always stretch the waiting panel to cover the game
            waiting_panel_->Raise();              // Make sure it stays above
        }
        event.Skip(); // Let other things process resize
    }

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
        back_btn_ = new wxButton(this, wxID_ANY, "back", wxDefaultPosition);
        back_btn_->Bind(wxEVT_BUTTON, &AbstractGamePanel::onHomeButtonPress, this);
        Bind(wxEVT_SIZE, &AbstractGamePanel::OnResize, this);
    }

    ~AbstractGamePanel() {}

    void setOppActive(bool active){
        is_opp_active_ = active;
    }

    void setInteractive(bool enable){
        wxWindowList children = GetChildren();
        for (wxWindowList::iterator it = children.begin(); it != children.end(); ++it) {
            if (*it != back_btn_) {
                (*it)->Enable(enable);
            }
        }
    }

    void waitingMoveEnter() {
        setFrameStatusText("waiting for move...");
        setInteractive(false);
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
        setInteractive(true);
        setFrameStatusText(getFrameStatusText());
        parentFrame->Layout();
    }

    void onHomeButtonPress(wxCommandEvent& evt){
        this->goHome();
    }

    void goHome(){
        // reset messages for next time
        my_msg_ = new GameMessage();
        opp_msg_ = new GameMessage();

        // will broadcast you have left the game to everyone
        setScreen(SCREEN_GAME_SELECTION);
    }

    void setFrameStatusText(std::string input) {
        parentFrame->SetStatusText(input);
    }

};
#endif