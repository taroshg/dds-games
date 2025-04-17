#ifndef ABSTRACTGAMEGUI_HPP
#define ABSTRACTGAMEGUI_HPP

#include <wx/wx.h>
#include <string.h>

class AbstractGame : public wxPanel 
{
public:
    AbstractGame(wxFrame* parent, wxPanel* waitingPanel, wxTimer* timer);
    virtual void setUpGame() = 0;
    virtual void gameButtonClick(wxCommandEvent& event) = 0;
    virtual bool determineWinner() = 0;
    virtual std::string getFrameStatusText() = 0;
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

AbstractGame::AbstractGame(wxFrame* parent, wxPanel* waitingPanel, wxTimer* timer) : wxPanel(parent) {
    this->parentFrame = parent;
    this->timer = timer;
    this->waiting_panel_= waitingPanel;
}

AbstractGame::~AbstractGame() {

}

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

    // TODO: wait for read from DDSGameController
    timer->StartOnce(500); // Simulate connection after 3 seconds
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