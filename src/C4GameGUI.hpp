#ifndef C4GAMEGUI_HPP
#define C4GAMEGUI_HPP

#include <string.h>

#include <wx/wx.h>
#include <wx/bitmap.h>
#include <wx/bmpbuttn.h>
#include <wx/image.h>

#include "GameUser.hpp"
#include "WaitingPanel.hpp"
#include "games/c4.hpp"
#include "AbstractGameGUI.hpp"

class C4GameGUI : public AbstractGamePanel 
{

private:
    wxBitmap rButtonImage;
    wxBitmap bButtonImage;
    wxBitmap blankButtonImage;
    wxSize buttonTileSize;
    wxBitmapButton* buttons[42]; // Store button references

    C4 c4;

    void setupGame() override{
        c4.reset();
        my_msg_ = new GameMessage();
        my_msg_->game_id(GAME_ID);
        opp_msg_ = new GameMessage();
    }
    void setupC4Display(){
        // Create blank white tile
        wxImage blankImage(buttonTileSize.GetWidth(), buttonTileSize.GetHeight());
        blankImage.SetRGB(wxRect(0, 0, buttonTileSize.GetWidth(), buttonTileSize.GetHeight()), 255, 255, 255);
        blankButtonImage = wxBitmap(blankImage);

        // Load and rescale red and blue marker images
        wxImage rImg(wxT("./resources/red_dot.png"), wxBITMAP_TYPE_PNG);
        rImg.Rescale(buttonTileSize.GetWidth(), buttonTileSize.GetHeight(), wxIMAGE_QUALITY_HIGH);
        rButtonImage = wxBitmap(rImg);

        wxImage bImg(wxT("./resources/blue_dot.png"), wxBITMAP_TYPE_PNG);
        bImg.Rescale(buttonTileSize.GetWidth(), buttonTileSize.GetHeight(), wxIMAGE_QUALITY_HIGH);
        bButtonImage = wxBitmap(bImg);
    }
public:
    const uint8_t GAME_ID = 2;
    C4GameGUI(wxFrame* parent, WaitingPanel* waitingPanel, std::function<void(int)> setScreen, GameUser* game_user)
    :AbstractGamePanel(parent, waitingPanel, setScreen, game_user)
    {
        // Set tile size bigger for better visibility
        buttonTileSize = wxSize(50, 50);

        // Create a grid sizer for the 7x6 layout
        wxGridSizer* gridSizer = new wxGridSizer(6, 7, 5, 5); // 6 rows, 7 columns, 5px gaps

        setupGame();
        setupC4Display();

        // Create and add 6*7 bitmap buttons to the sizer
        for (int r = 5; r >= 0; --r) {
            for (int c = 0; c < 7; ++c) {
                int i = r * 7 + c;
                buttons[i] = new wxBitmapButton(this, i, blankButtonImage, wxDefaultPosition, buttonTileSize);
                buttons[i]->Bind(wxEVT_BUTTON, &C4GameGUI::gameButtonClick, this);
                gridSizer->Add(buttons[i], 0, wxALIGN_CENTER);
            }
        }

        // Wrap gridSizer inside a vertical box sizer to center it
        wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
        mainSizer->AddStretchSpacer(1); // Top flexible spacer
        mainSizer->Add(gridSizer, 0, wxALIGN_CENTER | wxALL, 20); // Centered grid with padding
        mainSizer->AddStretchSpacer(1); // Bottom flexible spacer

        SetSizer(mainSizer);
        SetMinSize(wxSize(600, 600)); // Make window reasonably large
        Layout();
    }
    ~C4GameGUI() {};

    void gameButtonClick(wxCommandEvent& event) override{
        if (!is_opp_active_) return;

        int pos = event.GetId() % 7;
        
        std::cout << "selected column: " << std::to_string(pos) << std::endl;

        if(c4.make_move(pos)){
            // FASTDDS publish code
            std::cout << "sending: \n" << c4.boardString() << std::endl;
            my_msg_->c4(c4.myState());
            game_user_->sendGameMessage(my_msg_);
    
            updateDisplay();
            waitingMoveEnter();
        }
        else{
            wxMessageBox("Please choose an empty slot on the board");
        }
    
        determineWinner();
    }
    bool determineWinner() override{
        if(c4.is_end(my_msg_, opp_msg_)){
            int winner = c4.get_winner(my_msg_, opp_msg_);
            
            if (winner > 0) wxMessageBox("You Win!!");
            if (winner < 0) wxMessageBox("You Lose :(");
            if (winner == 0) wxMessageBox("it is a draw!");
    
            setupGame();
            updateDisplay();
    
            return true;
        }
        return false;
    }

    void updateDisplay() override{
        if (!is_opp_active_) return;

        if(game_user_->messageAvailable()){
            opp_msg_ = game_user_->readGameMessage();

            // if opp left the game here!!!
            if (opp_msg_->message() == "screen selection" && opp_msg_->game_id() != GAME_ID){
                std::cout << "opp left the game" << std::endl;
                setFrameStatusText("opp left the game");
                setupGame();
                setOppActive(false);
                return;
            }

            c4.setOppState(opp_msg_->c4());
            std::cout << "recieved: \n" << c4.boardString() << std::endl;
            std::cout << game_user_->lastMessageCount() << " / " << game_user_->messageCount() << std::endl;
        }
    
        wxBitmap myMarker = game_user_->first_ ? rButtonImage : bButtonImage;
        wxBitmap oppMarker = game_user_->first_ ? bButtonImage : rButtonImage;
    
        for (int i = 0; i < 42; i++)
        {   
            // if my marker is at i
            if(c4.myState() & (1ULL << i)){
                buttons[i]->SetBitmap(myMarker);
            }
            
            // if opp marker is at i
            else if(c4.oppState() & (1ULL << i)){
                buttons[i]->SetBitmap(oppMarker);
            }
    
            else {
                buttons[i]->SetBitmap(blankButtonImage);
            }
        }
    
        determineWinner();
    }
    std::string getFrameStatusText() override{
        if (game_user_->turn_){
            return game_user_->first_ ? "it is your turn (you are X)" : "it is your turn (you are O)";
        }
        else return "it is opp turn";
    }
};

#endif