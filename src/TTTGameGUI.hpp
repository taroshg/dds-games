#ifndef TTTGAMEGUI_HPP
#define TTTGAMEGUI_HPP

#include <string.h>

#include <wx/wx.h>
#include <wx/bitmap.h>
#include <wx/bmpbuttn.h>
#include <wx/image.h>
#include "GameUser.hpp"
#include "WaitingPanel.hpp"

#include "games/ttt.hpp"

#include "AbstractGameGUI.hpp"

class TTTGameGUI : public AbstractGamePanel
{
public:
    const uint8_t GAME_ID = 2;
    TTTGameGUI(wxFrame *parent, WaitingPanel *waitingPanel, std::function<void(int)> setScreen, GameUser *game_user)
        : AbstractGamePanel(parent, waitingPanel, setScreen, game_user)
    {
        // Create a grid sizer for the 3x3 layout
        wxGridSizer *gridSizer = new wxGridSizer(3, 3, 5, 5); // 3 rows, 3 columns, 5px padding

        setupGame();
        setupTTTDisplay();

        // Create and add 9 bitmap buttons to the sizer
        for (int i = 0; i < 9; i++)
        {
            buttons[i] = new wxBitmapButton(this, i, blankButtonImage, wxDefaultPosition, buttonTileSize);
            buttons[i]->Bind(wxEVT_BUTTON, &TTTGameGUI::gameButtonClick, this);
            gridSizer->Add(buttons[i], 0, wxALIGN_CENTER, 5);
        }

        // Wrap gridSizer inside a box sizer to center it
        wxBoxSizer *mainSizer = new wxBoxSizer(wxVERTICAL);
        mainSizer->AddStretchSpacer();                            // Pushes content down
        mainSizer->Add(gridSizer, 0, wxALIGN_CENTER | wxALL, 20); // Centers grid
        mainSizer->AddStretchSpacer();                            // Pushes content up

        SetSizer(mainSizer);
    }
    ~TTTGameGUI() {};

    void gameButtonClick(wxCommandEvent &event) override
    {
        if (!is_opp_active_)
            return;

        int pos = event.GetId();

        std::cout << "able to send msg bc opp game id: " << std::to_string(opp_msg_->game_id()) << std::endl;

        if (ttt.make_move(pos))
        {
            // FASTDDS publish code
            std::cout << "sending: \n"
                      << ttt.boardString() << std::endl;
            my_msg_->ttt(ttt.myState());
            game_user_->sendGameMessage(my_msg_);

            updateDisplay();
            waitingMoveEnter();
        }
        else
        {
            wxMessageBox("Please choose an empty slot on the board");
        }

        determineWinner();
    }
    bool determineWinner() override
    {
        if (ttt.is_end(my_msg_, opp_msg_))
        {
            int winner = ttt.get_winner(my_msg_, opp_msg_);

            if (winner > 0)
                wxMessageBox("You Win!!");
            if (winner < 0)
                wxMessageBox("You Lose :(");
            if (winner == 0)
                wxMessageBox("it is a draw!");

            setupGame();
            updateDisplay();

            return true;
        }
        return false;
    }

    void updateDisplay() override
    {
        if (!is_opp_active_)
            return;

        if (game_user_->messageAvailable())
        {
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

            ttt.setOppState(opp_msg_->ttt());
            std::cout << "recieved: \n"
                      << ttt.boardString() << std::endl;
            std::cout << game_user_->lastMessageCount() << " / " << game_user_->messageCount() << std::endl;
        }

        wxBitmap myMarker = game_user_->first_ ? xButtonImage : oButtonImage;
        wxBitmap oppMarker = game_user_->first_ ? oButtonImage : xButtonImage;

        for (int i = 0; i < 9; i++)
        {
            // if my marker is at i
            if (ttt.myState() & (1 << i))
            {
                buttons[i]->SetBitmap(myMarker);
            }

            // if opp marker is at i
            else if (ttt.oppState() & (1 << i))
            {
                buttons[i]->SetBitmap(oppMarker);
            }

            else
            {
                buttons[i]->SetBitmap(blankButtonImage);
            }
        }

        determineWinner();
    }
    std::string getFrameStatusText() override
    {
        if (game_user_->turn_)
        {
            return game_user_->first_ ? "it is your turn (you are X)" : "it is your turn (you are O)";
        }
        else
            return "it is opp turn";
    }

private:
    wxBitmap xButtonImage;
    wxBitmap oButtonImage;
    wxBitmap blankButtonImage;
    wxSize buttonTileSize;
    wxBitmapButton *buttons[9]; // Store button references

    TTT ttt;

    void setupGame() override
    {
        ttt.reset();
        my_msg_ = new GameMessage();
        my_msg_->game_id(GAME_ID);
        opp_msg_ = new GameMessage();
    }
    void setupTTTDisplay()
    {
        SetBackgroundColour(wxColour(0, 0, 0));

        int scaleFactor = 1;

        buttonTileSize = wxSize(100, 100);
        wxImage blankImage(buttonTileSize.GetWidth(), buttonTileSize.GetHeight());
        blankImage.SetRGB(wxRect(0, 0, buttonTileSize.GetWidth(), buttonTileSize.GetHeight()), 255, 255, 255);
        blankButtonImage = wxBitmap(blankImage);

        if (scaleFactor > 3)
        {
            xButtonImage.LoadFile(wxT("./resources/Xletter.png"), wxBITMAP_TYPE_PNG);
            oButtonImage.LoadFile(wxT("./resources/CircleOut.png"), wxBITMAP_TYPE_PNG);
            buttonTileSize = wxSize(200, 200);
        }
        else if (scaleFactor > 2)
        {
            xButtonImage.LoadFile(wxT("./resources/Xletter.png"), wxBITMAP_TYPE_PNG);
            oButtonImage.LoadFile(wxT("./resources/CircleOut.png"), wxBITMAP_TYPE_PNG);
            buttonTileSize = wxSize(150, 150);
        }
        else if (scaleFactor > 1)
        {
            xButtonImage.LoadFile(wxT("./resources/Xletter.png"), wxBITMAP_TYPE_PNG);
            oButtonImage.LoadFile(wxT("./resources/CircleOut.png"), wxBITMAP_TYPE_PNG);
            buttonTileSize = wxSize(125, 125);
        }
        else
        {
            xButtonImage.LoadFile(wxT("./resources/Xletter.png"), wxBITMAP_TYPE_PNG);
            oButtonImage.LoadFile(wxT("./resources/CircleOut.png"), wxBITMAP_TYPE_PNG);
        }
    }
};

#endif