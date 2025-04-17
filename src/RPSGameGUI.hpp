#ifndef RPSGAMEGUI_HPP
#define RPSGAMEGUI_HPP

#include <string.h>

#include <wx/wx.h>
#include <wx/bitmap.h>
#include <wx/bmpbuttn.h>
#include <wx/image.h>
#include "GameUser.hpp"

#include "AbstractGameGUI.hpp"

class RPSGameGUI : public AbstractGame 
{
public:
    RPSGameGUI(wxFrame* parent, wxPanel* waitingPanel, wxTimer* timer, int rounds);
    ~RPSGameGUI();
    void setUpGame() override;
    void gameButtonClick(wxCommandEvent& event) override;
    bool determineWinner() override;
    std::string getFrameStatusText() override;

private:
    wxBitmap rockButtonImage;
    wxBitmap paperButtonImage;
    wxBitmap scissorsButtonImage;
    wxBitmap blankButtonImage;
    wxSize buttonTileSize;
    int turnCounter;
    wxBitmapButton* buttons[3]; // Store button references

    // RPS GUI vars
    int rounds;
    int player1Wins;
    int player2Wins;
    int ties;
    int currentPlayer; // 0 for player1, 1 for player2

    std::string player1Name;
    std::string player2Name;
    char player1Choice, player2Choice;

    GameUser* game_user_;

    // Helper functions specific to RPS
    void setupRPSGame(std::string username1, std::string username2);
    void setupRPSDisplay();
    void resetRPSGame();
    void determineRoundWinner();
};

RPSGameGUI::RPSGameGUI(wxFrame* parent, wxPanel* waitingPanel, wxTimer* timer, int rounds) : AbstractGame(parent, waitingPanel, timer) {
    this->rounds = rounds;
    setUpGame();
}

RPSGameGUI::~RPSGameGUI() {
    
}

// AbstractGame methods
void RPSGameGUI::setUpGame()
{
    // Create a grid sizer for the 3x3 layout
    wxGridSizer* gridSizer = new wxGridSizer(1, 3, 5, 5); // 3 rows, 3 columns, 5px padding

    setupRPSGame("Player1", "Player2");
    setupRPSDisplay();
    
    buttons[0] = new wxBitmapButton(this, 101, rockButtonImage, wxDefaultPosition, buttonTileSize);
    buttons[0]->Bind(wxEVT_BUTTON, &RPSGameGUI::gameButtonClick, this);
    gridSizer->Add(buttons[0], 0, wxALIGN_CENTER , 5);

    buttons[1] = new wxBitmapButton(this, 1+101, paperButtonImage, wxDefaultPosition, buttonTileSize);
    buttons[1]->Bind(wxEVT_BUTTON, &RPSGameGUI::gameButtonClick, this);
    gridSizer->Add(buttons[1], 0, wxALIGN_CENTER , 5);

    buttons[2] = new wxBitmapButton(this, 2+101, scissorsButtonImage, wxDefaultPosition, buttonTileSize);
    buttons[2]->Bind(wxEVT_BUTTON, &RPSGameGUI::gameButtonClick, this);
    gridSizer->Add(buttons[2], 0, wxALIGN_CENTER , 5);

    // Wrap gridSizer inside a box sizer to center it
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
    mainSizer->AddStretchSpacer(); // Pushes content down
    mainSizer->Add(gridSizer, 0, wxALIGN_CENTER | wxALL, 20); // Centers grid
    mainSizer->AddStretchSpacer(); // Pushes content up

    SetSizer(mainSizer);
}

void RPSGameGUI::gameButtonClick(wxCommandEvent& event)
{
    int id = event.GetId()-100;
    char currentChoice;

    if(id == 1) // Current Player chose rock
    {
        currentChoice = 'r';
    }
    else if (id == 2) // Current Player chose paper
    {
        currentChoice = 'p';
    }
    else // Current Player chose scisors
    {
        currentChoice = 's';
    }
    
    if(currentPlayer == 0)
    {
        player1Choice = currentChoice;
        currentPlayer = 1;
    }
    else if(currentPlayer == 1)
    {
        player2Choice = currentChoice;
        determineRoundWinner(); // Run since both players have now inputed for the current round.
        currentPlayer = 0;
    }

    // TODO: Publish with DDS
    
    waitingDisplayEnter();
}

std::string RPSGameGUI::getFrameStatusText() {
    return "Player 1 wins: " + std::to_string(player1Wins) + ". Player 2 wins: " + std::to_string(player2Wins) +". Round: " + std::to_string(ties + player1Wins + player2Wins + 1) + " of " + std::to_string(rounds) + " It is currently Player " + std::to_string(currentPlayer+1) + "'s turn.";
}

/**
 * This isn't needed here so it's just a dummy definition made to appease the AbstractGameGUI's rules
 */
bool RPSGameGUI::determineWinner()
{
    return false;
}

// RPS Specific Helper methods
/**
 * Function to get the result of the game
 */
void RPSGameGUI::determineRoundWinner() {
    if (player1Choice == player2Choice) {
        wxMessageBox("No one wins the round...");
        ties++;
    } else if ((player1Choice == 'r' && player2Choice == 's') ||
               (player1Choice == 'p' && player2Choice == 'r') ||
               (player1Choice == 's' && player2Choice == 'p')) {
        wxMessageBox(player1Name + " wins the round!");
        player1Wins++;
    } else {
        wxMessageBox(player2Name + " wins the round!");
        player2Wins++;
    }

    // Check if the game is over
    if(player1Wins + player2Wins + ties == rounds)
    {
        if (player1Wins == player2Wins) {
            wxMessageBox("No one wins the game you tied!");
            resetRPSGame();
        } 
        else if (player1Wins > player2Wins) {
            wxMessageBox(player1Name + " wins the game!");
            resetRPSGame();
        } 
        else if (player1Wins < player2Wins) {
            wxMessageBox(player2Name + " wins the game!");
            resetRPSGame();
        } 
        
    }
}

void RPSGameGUI::setupRPSGame(std::string username1, std::string username2)
{   
    player1Name = username1;
    player2Name = username2;
    currentPlayer = 0;
    turnCounter = 1;
    player1Wins = 0;
    player2Wins = 0;
    ties = 0;
}

void RPSGameGUI::resetRPSGame()
{   
    currentPlayer = 0;
    turnCounter = 1;
    player1Wins = 0;
    player2Wins = 0;
    ties = 0;
}


void RPSGameGUI::setupRPSDisplay()
{
    buttonTileSize = wxSize(100,100);
    wxImage blankImage(buttonTileSize.GetWidth(), buttonTileSize.GetHeight());
    blankImage.SetRGB(wxRect(0, 0, buttonTileSize.GetWidth(), buttonTileSize.GetHeight()), 255, 255, 255);
    blankButtonImage = wxBitmap(blankImage);

    rockButtonImage.LoadFile(wxT("./resources/rock.png"), wxBITMAP_TYPE_PNG);
    paperButtonImage.LoadFile(wxT("./resources/paper.png"), wxBITMAP_TYPE_PNG);
    scissorsButtonImage.LoadFile(wxT("./resources/scissors.png"), wxBITMAP_TYPE_PNG);
}

#endif