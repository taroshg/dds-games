#ifndef TTTGAMEGUI_HPP
#define TTTGAMEGUI_HPP

#include <string.h>

#include <wx/wx.h>
#include <wx/bitmap.h>
#include <wx/bmpbuttn.h>
#include <wx/image.h>

#include "Grid.hpp"
#include "Player.hpp"

#include "AbstractGameGUI.hpp"

class TTTGameGUI : public AbstractGame 
{
public:
    TTTGameGUI(wxFrame* parent, wxPanel* waitingPanel, wxTimer* timer);
    ~TTTGameGUI();
    void setUpGame() override;
    void gameButtonClick(wxCommandEvent& event) override;
    bool determineWinner() override;
    std::string getFrameStatusText() override;

private:
    wxBitmap xButtonImage;
    wxBitmap oButtonImage;
    wxBitmap blankButtonImage;
    wxSize buttonTileSize;
    int turnCounter;
    wxBitmapButton* buttons[9]; // Store button references

    Player p1{"1"};
    Player p2{"2"};
    Grid gameGrid;
    Player currentPlayer;

    void setupTTTGame(std::string username1, std::string username2);
    void setupTTTDisplay();
    void updateGraphicalGameGrid();
    bool checkWin();

};


#endif