#ifndef RPSGAMEGUI_HPP
#define RPSGAMEGUI_HPP

#include <string.h>

#include <wx/wx.h>
#include <wx/bitmap.h>
#include <wx/bmpbuttn.h>
#include <wx/image.h>
#include <stdexcept>

#include "GameUser.hpp"
#include "WaitingPanel.hpp"
#include "AbstractGameGUI.hpp"

class RPSGameGUI : public AbstractGamePanel
{
private:
    wxBitmap rockButtonImage;
    wxBitmap paperButtonImage;
    wxBitmap scissorsButtonImage;
    wxBitmap blankButtonImage;
    wxSize buttonTileSize;
    int turnCounter;
    wxBitmapButton *buttons[3]; // Store button references

    // RPS GUI vars
    int rounds;
    int player1Wins;
    int player2Wins;
    int ties;

    std::string player1Name;
    std::string player2Name;
    char player1Choice, player2Choice;

public:
    const uint8_t GAME_ID = 1;
    RPSGameGUI(wxFrame *parent, WaitingPanel *waitingPanel, std::function<void(int)> setScreen, GameUser *game_user)
        : AbstractGamePanel(parent, waitingPanel, setScreen, game_user),
          player1Choice(0), player2Choice(0)
    {
        my_msg_ = new GameMessage();
        my_msg_->game_id(GAME_ID);
        opp_msg_ = new GameMessage();

        // Create a grid sizer for the 3x3 layout
        wxGridSizer *gridSizer = new wxGridSizer(1, 3, 5, 5); // 3 rows, 3 columns, 5px padding

        setupGame();

        SetBackgroundColour(wxColour(173, 216, 230)); // light blue

        Refresh();
        Update();

        buttonTileSize = wxSize(128, 128);
        wxImage blankImage(buttonTileSize.GetWidth(), buttonTileSize.GetHeight());
        blankImage.SetRGB(wxRect(0, 0, buttonTileSize.GetWidth(), buttonTileSize.GetHeight()), 255, 255, 255);
        blankButtonImage = wxBitmap(blankImage);

        rockButtonImage.LoadFile(wxT("./resources/rock.png"), wxBITMAP_TYPE_PNG);
        paperButtonImage.LoadFile(wxT("./resources/paper.png"), wxBITMAP_TYPE_PNG);
        scissorsButtonImage.LoadFile(wxT("./resources/scissors.png"), wxBITMAP_TYPE_PNG);

        buttons[0] = new wxBitmapButton(this, 101, rockButtonImage, wxDefaultPosition, buttonTileSize);
        buttons[0]->Bind(wxEVT_BUTTON, &RPSGameGUI::gameButtonClick, this);
        gridSizer->Add(buttons[0], 0, wxALIGN_CENTER, 5);

        buttons[1] = new wxBitmapButton(this, 1 + 101, paperButtonImage, wxDefaultPosition, buttonTileSize);
        buttons[1]->Bind(wxEVT_BUTTON, &RPSGameGUI::gameButtonClick, this);
        gridSizer->Add(buttons[1], 0, wxALIGN_CENTER, 5);

        buttons[2] = new wxBitmapButton(this, 2 + 101, scissorsButtonImage, wxDefaultPosition, buttonTileSize);
        buttons[2]->Bind(wxEVT_BUTTON, &RPSGameGUI::gameButtonClick, this);
        gridSizer->Add(buttons[2], 0, wxALIGN_CENTER, 5);

        // Wrap gridSizer inside a box sizer to center it
        wxBoxSizer *mainSizer = new wxBoxSizer(wxVERTICAL);
        mainSizer->AddStretchSpacer();                            // Pushes content down
        mainSizer->Add(gridSizer, 0, wxALIGN_CENTER | wxALL, 20); // Centers grid
        mainSizer->AddStretchSpacer();                            // Pushes content up

        SetSizer(mainSizer);
    }
    ~RPSGameGUI() {};

    void gameButtonClick(wxCommandEvent &event) override
    {
        if (!is_opp_active_)
            return;

        int choice = event.GetId() - 100;

        player1Choice = intChoicetoChar(choice);

        std::cout << "sending: " << player1Choice << std::endl;

        my_msg_->rps(choice);
        game_user_->sendGameMessage(my_msg_);

        // waits till opp message is recieved
        waitingMoveEnter();

        determineRoundWinner();
    }
    bool determineWinner() override
    {
        return false;
    }
    std::string getFrameStatusText() override
    {
        return "your wins: " + std::to_string(player1Wins) + ". your losses: " + std::to_string(player2Wins) + ". ties: " + std::to_string(ties);
    }
    void updateDisplay() override
    {
        determineRoundWinner();
        return;
    }

private:
    // Helper functions specific to RPS
    void setupGame() override
    {
        turnCounter = 1;
        player1Wins = 0;
        player2Wins = 0;
        ties = 0;
    }
    void determineRoundWinner()
    {
        if (!is_opp_active_)
            return;

        if (!game_user_->messageAvailable() || player1Choice == 0)
            return;
        opp_msg_ = game_user_->readGameMessage();

        // if opp left the game here!!!
        if (opp_msg_->message() == "screen selection" && opp_msg_->game_id() != GAME_ID)
        {
            std::cout << "opp left the game" << std::endl;
            setFrameStatusText("opp left the game");
            setupGame();
            setOppActive(false);
            return;
        }

        player2Choice = intChoicetoChar(opp_msg_->rps());

        std::cout << "you: " << player1Choice << " opp: " << player2Choice << std::endl;

        if (player1Choice == player2Choice)
        {
            wxMessageBox("No one wins the round...");
            ties++;
        }
        else if ((player1Choice == 'r' && player2Choice == 's') ||
                 (player1Choice == 'p' && player2Choice == 'r') ||
                 (player1Choice == 's' && player2Choice == 'p'))
        {
            wxMessageBox("you win the round!");
            player1Wins++;
        }
        else
        {
            wxMessageBox("you lose the round");
            player2Wins++;
        }

        // resets player choices for the next round;
        player1Choice = player2Choice = 0;
    }
    const char intChoicetoChar(int choice)
    {
        if (choice == 1)
            return 'r'; // rock
        if (choice == 2)
            return 'p'; // paper
        if (choice == 3)
            return 's'; // scissors
        return '-';
    }
};

#endif