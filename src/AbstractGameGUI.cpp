#include "AbstractGameGUI.hpp"

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
    parentFrame->Layout();
    timer->StartOnce(1); // Simulate connection after 3 seconds
    Layout(); // Ensure everything redraws properly
}

void AbstractGame::waitingDisplayExit()
{
    waiting_panel_->Hide();
    setFrameStatusText(getFrameStatusText());
    Show();
    Layout(); // Ensure everything redraws properly
    parentFrame->Layout();
}