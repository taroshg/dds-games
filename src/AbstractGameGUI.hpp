#ifndef ABSTRACTGAMEGUI_HPP
#define ABSTRACTGAMEGUI_HPP

#include <wx/wx.h>
#include <string.h>
#include "GameUser.hpp"

class AbstractGame : public wxPanel 
{
protected:
    GameUser* game_user_;
    GameMessage* my_msg_;
    GameMessage* opp_msg_;
public:
    AbstractGame(wxFrame* parent, wxPanel* waitingPanel, wxTimer* timer, GameUser* game_user);
    virtual void setUpGame() = 0;
    virtual void gameButtonClick(wxCommandEvent& event) = 0;
    virtual bool determineWinner() = 0;
    virtual std::string getFrameStatusText() = 0;
    virtual void updateDisplay() = 0;
    ~AbstractGame();
    
    wxPanel* getWaitingDisplay();
    void waitingDisplayEnter();
    void waitingDisplayExit();
    void setFrameStatusText(std::string input);

private:
    wxPanel* waiting_panel_;
    wxTimer* timer;
    wxFrame* parentFrame;
};

AbstractGame::AbstractGame(wxFrame* parent, wxPanel* waitingPanel, wxTimer* timer, GameUser* game_user) : 
wxPanel(parent),
my_msg_(new GameMessage()),
opp_msg_(new GameMessage())
{
    this->game_user_ = game_user;
    this->parentFrame = parent;
    this->timer = timer;
    this->waiting_panel_= waitingPanel;
}

AbstractGame::~AbstractGame() {}

void AbstractGame::setFrameStatusText(std::string input) {
    parentFrame->SetStatusText(input);
}

wxPanel* AbstractGame::getWaitingDisplay()
{
    return waiting_panel_;
}

void AbstractGame::waitingDisplayEnter()
{
    Hide();
    setFrameStatusText("Listening...");
    waiting_panel_->Show();
    waiting_panel_->Fit();
    waiting_panel_->Layout();
    Layout(); // Ensure everything redraws properly
    parentFrame->Layout();

    std::thread([this]() {
        while(!game_user_->messageAvailable())
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        
        wxTheApp->CallAfter([this](){
            timer->StartOnce(100);
            updateDisplay();
        });
    }).detach();
}

void AbstractGame::waitingDisplayExit()
{
    waiting_panel_->Hide();
    setFrameStatusText(getFrameStatusText());
    Show();
    Layout(); // Ensure everything redraws properly
    parentFrame->Layout();
}

#endif