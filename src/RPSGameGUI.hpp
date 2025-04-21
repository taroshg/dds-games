#ifndef RPSGAMEGUI_HPP
#define RPSGAMEGUI_HPP

#include <string.h>

#include <wx/wx.h>
#include <wx/bitmap.h>
#include <wx/bmpbuttn.h>
#include <wx/image.h>
#include "GameUser.hpp"
#include <stdexcept>

#include "AbstractGameGUI.hpp"

class RPSGameGUI : public AbstractGame 
{
public:
    RPSGameGUI(wxFrame* parent, wxPanel* waitingPanel, wxTimer* timer, GameUser* game_user, int rounds);
    ~RPSGameGUI();
    void setUpGame() override;
    void gameButtonClick(wxCommandEvent& event) override;
    bool determineWinner() override;
    std::string getFrameStatusText() override;
    void updateDisplay() override;

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

    std::string player1Name;
    std::string player2Name;
    char player1Choice, player2Choice;

    // Helper functions specific to RPS
    void setupRPSGame(std::string username1, std::string username2);
    void setupRPSDisplay();
    void resetRPSGame();
    void determineRoundWinner();
    const char intChoicetoChar(int choice);
};

RPSGameGUI::RPSGameGUI(wxFrame* parent, wxPanel* waitingPanel, wxTimer* timer, GameUser* game_user, int rounds) 
: AbstractGame(parent, waitingPanel, timer, game_user),
player1Choice(0), player2Choice(0) {
    this->rounds = rounds;
    setUpGame();
}

RPSGameGUI::~RPSGameGUI() {}

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
    int choice = event.GetId()-100;

    player1Choice = intChoicetoChar(choice);

    std::cout << "sending: " << player1Choice << std::endl;

    my_msg_->rps(choice);
    game_user_->sendGameMessage(my_msg_);
    
    // waits till opp message is recieved
    waitingDisplayEnter();

    determineRoundWinner();
}

const char RPSGameGUI::intChoicetoChar(int choice){
    if (choice == 1) return 'r'; // rock
    if (choice == 2) return 'p'; // paper
    if (choice == 3) return 's'; // scissors
    return '-';
}

std::string RPSGameGUI::getFrameStatusText() {
    return "Player 1 wins: " + std::to_string(player1Wins) + ". Player 2 wins: " + std::to_string(player2Wins) +". Round: " + std::to_string(ties + player1Wins + player2Wins + 1) + " of " + std::to_string(rounds);
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
    if (!game_user_->messageAvailable() || player1Choice == 0) return;
    opp_msg_ = game_user_->readGameMessage();
    player2Choice = intChoicetoChar(opp_msg_->rps());

    std::cout << "you: " << player1Choice << " opp: " << player2Choice << std::endl;
    
    if (player1Choice == player2Choice) {
        wxMessageBox("No one wins the round...");
        ties++;
    } else if ((player1Choice == 'r' && player2Choice == 's') ||
               (player1Choice == 'p' && player2Choice == 'r') ||
               (player1Choice == 's' && player2Choice == 'p')) {
        wxMessageBox("you win the round!");
        player1Wins++;
    } else {
        wxMessageBox("you lose the round");
        player2Wins++;
    }

    // Check if the game is over
    if(player1Wins + player2Wins + ties == rounds)
    {
        if (player1Wins == player2Wins) {
            wxMessageBox("its a tie!");
            resetRPSGame();
        } 
        else if (player1Wins > player2Wins) {
            wxMessageBox("you win the game!");
            resetRPSGame();
        } 
        else if (player1Wins < player2Wins) {
            wxMessageBox("you lose the game");
            resetRPSGame();
        } 
    }

    // resets player choices for the next round;
    player1Choice = player2Choice = 0;
}

void RPSGameGUI::setupRPSGame(std::string username1, std::string username2)
{   
    turnCounter = 1;
    player1Wins = 0;
    player2Wins = 0;
    ties = 0;
}

void RPSGameGUI::resetRPSGame()
{   
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

void RPSGameGUI::updateDisplay(){
    determineRoundWinner();
    return;
}

#endif