#ifndef TTTGAMEGUI_HPP
#define TTTGAMEGUI_HPP

#include <string.h>

#include <wx/wx.h>
#include <wx/bitmap.h>
#include <wx/bmpbuttn.h>
#include <wx/image.h>

#include "games/ttt/ttt.hpp"

#include "AbstractGameGUI.hpp"

class TTTGameGUI : public AbstractGame 
{
public:
    TTTGameGUI(wxFrame* parent, wxPanel* waitingPanel, wxTimer* timer, GameUser* game_user);
    ~TTTGameGUI();
    void setUpGame() override;
    void gameButtonClick(wxCommandEvent& event) override;
    bool determineWinner() override;
    void updateDisplay() override;
    std::string getFrameStatusText() override;

private:
    wxBitmap xButtonImage;
    wxBitmap oButtonImage;
    wxBitmap blankButtonImage;
    wxSize buttonTileSize;
    wxBitmapButton* buttons[9]; // Store button references

    TTT ttt;

    void setupTTTGame();
    void setupTTTDisplay();
};

TTTGameGUI::TTTGameGUI(wxFrame* parent, wxPanel* waitingPanel, wxTimer* timer, GameUser* game_user) : 
AbstractGame(parent, waitingPanel, timer, game_user) {
    setUpGame();
}

TTTGameGUI::~TTTGameGUI() {}

// AbstractGame methods
void TTTGameGUI::setUpGame()
{
    // Create a grid sizer for the 3x3 layout
    wxGridSizer* gridSizer = new wxGridSizer(3, 3, 5, 5); // 3 rows, 3 columns, 5px padding

    setupTTTGame();
    setupTTTDisplay();

    // Create and add 9 bitmap buttons to the sizer
    for (int i = 0; i < 9; i++)
    {
        buttons[i] = new wxBitmapButton(this, i, blankButtonImage, wxDefaultPosition, buttonTileSize);
        buttons[i]->Bind(wxEVT_BUTTON, &TTTGameGUI::gameButtonClick, this);
        gridSizer->Add(buttons[i], 0, wxALIGN_CENTER , 5);
    }

    // Wrap gridSizer inside a box sizer to center it
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
    mainSizer->AddStretchSpacer(); // Pushes content down
    mainSizer->Add(gridSizer, 0, wxALIGN_CENTER | wxALL, 20); // Centers grid
    mainSizer->AddStretchSpacer(); // Pushes content up

    SetSizer(mainSizer);
}

void TTTGameGUI::gameButtonClick(wxCommandEvent& event)
{
    int pos = event.GetId();
 
    if(ttt.make_move(pos)){
        // FASTDDS publish code
        std::cout << "sending: \n" << ttt.boardString() << std::endl;
        my_msg_->ttt(ttt.myState());
        game_user_->sendGameMessage(my_msg_);

        updateDisplay();
        waitingDisplayEnter();
    }
    else{
        wxMessageBox("Please choose an empty slot on the board");
    }

    // determineWinner();
}

std::string TTTGameGUI::getFrameStatusText() {
    if (game_user_->turn_){
        return "it is your turn";
    }
    else return "it is opp turn";
}

bool TTTGameGUI::determineWinner()
{
    if(ttt.is_end(my_msg_, opp_msg_)){
        int winner = ttt.get_winner(my_msg_, opp_msg_);
        
        if (winner > 0) wxMessageBox("You Win!!");
        if (winner < 0) wxMessageBox("You Lose :(");
        if (winner == 0) wxMessageBox("it is a draw!");

        setupTTTGame();
        updateDisplay();

        return true;
    }
    return false;
}

void TTTGameGUI::setupTTTGame()
{
    ttt.reset();
    ttt.hard_reset(my_msg_);
    ttt.hard_reset(opp_msg_);
}

/**
 * This is supposed to provide an option scale up UI elements based on a users input
 * but for now it's going unused since we have bigger fish to fry
 */
void TTTGameGUI::setupTTTDisplay()
{
    int scaleFactor = 1;

    buttonTileSize = wxSize(100,100);
    wxImage blankImage(buttonTileSize.GetWidth(), buttonTileSize.GetHeight());
    blankImage.SetRGB(wxRect(0, 0, buttonTileSize.GetWidth(), buttonTileSize.GetHeight()), 255, 255, 255);
    blankButtonImage = wxBitmap(blankImage);

    if (scaleFactor > 3) {
        xButtonImage.LoadFile(wxT("./resources/X.png"), wxBITMAP_TYPE_PNG);
        oButtonImage.LoadFile(wxT("./resources/O.png"), wxBITMAP_TYPE_PNG);
        buttonTileSize = wxSize(200,200);
    }
    else if (scaleFactor > 2) {
        xButtonImage.LoadFile(wxT("./resources/X@0,75x.png"), wxBITMAP_TYPE_PNG);
        oButtonImage.LoadFile(wxT("./resources/O@0,75x.png"), wxBITMAP_TYPE_PNG);
        buttonTileSize = wxSize(150,150);
    }
    else if (scaleFactor > 1) {
        xButtonImage.LoadFile(wxT("./resources/X@0,5x.png"), wxBITMAP_TYPE_PNG);
        oButtonImage.LoadFile(wxT("./resources/O@0,5x.png"), wxBITMAP_TYPE_PNG);
        buttonTileSize = wxSize(125,125);
    }
    else {
        xButtonImage.LoadFile(wxT("./resources/X@0,25x.png"), wxBITMAP_TYPE_PNG);
        oButtonImage.LoadFile(wxT("./resources/O@0,25x.png"), wxBITMAP_TYPE_PNG);
    }
}

void TTTGameGUI::updateDisplay()
{
    opp_msg_ = game_user_->readGameMessage();
    if(opp_msg_ != nullptr){
        ttt.setOppState(opp_msg_->ttt());
        std::cout << "recieved: \n" << ttt.boardString() << std::endl;
        std::cout << game_user_->lastMessageCount() << " / " << game_user_->messageCount() << std::endl;
    }
    else std::cout << "nothing to read!" << std::endl;

    wxBitmap myMarker = game_user_->first_ ? xButtonImage : oButtonImage;
    wxBitmap oppMarker = game_user_->first_ ? oButtonImage : xButtonImage;

    for (int i = 0; i < 9; i++)
    {   
        // if my marker is at i
        if(ttt.myState() & (1 << i)){
            buttons[i]->SetBitmap(myMarker);
        }
        
        // if opp marker is at i
        else if(ttt.oppState() & (1 << i)){
            buttons[i]->SetBitmap(oppMarker);
        }
    }

    // determineWinner();
}
#endif