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

TTTGameGUI::TTTGameGUI(wxFrame* parent, wxPanel* waitingPanel, wxTimer* timer) : AbstractGame(parent, waitingPanel, timer) {
    setUpGame();
}

TTTGameGUI::~TTTGameGUI() {
    
}

// AbstractGame methods
void TTTGameGUI::setUpGame()
{
    // Create a grid sizer for the 3x3 layout
    wxGridSizer* gridSizer = new wxGridSizer(3, 3, 5, 5); // 3 rows, 3 columns, 5px padding

    setupTTTGame("Player1", "Player2");
    setupTTTDisplay();

    // Create and add 9 bitmap buttons to the sizer
    for (int i = 0; i < 9; i++)
    {
        buttons[i] = new wxBitmapButton(this, i+1, blankButtonImage, wxDefaultPosition, buttonTileSize);
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
    int id = event.GetId();
    bool isO = currentPlayer.getSymbol() == "O";
    bool validMove = false;
    
    validMove = gameGrid.addToGrid(id, isO); // Flag to see if game grid was actually updated

    updateGraphicalGameGrid();

    // Check for a win
    if (gameGrid.verifyWin()) {
        wxMessageBox(currentPlayer.getName() + " Wins!");
        setupTTTGame(p1.getName(), p2.getName());
        updateGraphicalGameGrid();
    }
    else if (turnCounter == 9) // Check if board is full with no winning patern and reset if so
    {
        wxMessageBox("Board filled with no winning patern nobody wins :(");
        setupTTTGame(p1.getName(), p2.getName());
        updateGraphicalGameGrid();
    }
    else // Run turn advancement stuff if game hasn't been won yet
    {
        if(validMove & currentPlayer.getSymbol() == p1.getSymbol()) // Use validMove to see if the turn should be advanced
        {
            currentPlayer = p2;
            turnCounter++;
        }
        else if(validMove) // Advance turn but this turn had p2 making the move
        {
            currentPlayer = p1;
            turnCounter++;
        }
        else // Invalid choice made prompt user to make another choice
        {
            wxMessageBox("Please choose an empty slot on the board");
        }
    }
    waitingDisplayEnter();
}

std::string TTTGameGUI::getFrameStatusText() {
    return "It is currently Player " + currentPlayer.getName() + "'s turn. Symbol: (" + currentPlayer.getSymbol() + ") Turn: " + std::to_string(turnCounter);
}

bool TTTGameGUI::determineWinner()
{
    return gameGrid.verifyWin();
}

// TTT Specific Private Helper methods

void TTTGameGUI::setupTTTGame(std::string username1, std::string username2)
{
    gameGrid.resetGrid();
    p1.setSymbol("O");
    p2.setSymbol("X");
    p1.setName(username1);
    p2.setName(username2);
    currentPlayer = p1;
    turnCounter = 1;
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

void TTTGameGUI::updateGraphicalGameGrid()
{
    for (int i = 0; i < 9; i++)
    {
        std::string symbol = gameGrid.getGridSymbol(i);
        
        if(symbol == "O")
        {
            buttons[i]->SetBitmap(oButtonImage);
        }
        else if (symbol == "X") {
            buttons[i]->SetBitmap(xButtonImage);
        }
        else {
            buttons[i]->SetBitmap(blankButtonImage);
        }
    }
}

#endif