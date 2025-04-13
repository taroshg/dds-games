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

#endif