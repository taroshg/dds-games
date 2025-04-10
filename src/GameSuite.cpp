#include <wx/wx.h>
#include <string.h>
#include <wx/bitmap.h>
#include <wx/bmpbuttn.h>
#include <wx/image.h>
#include "Grid.hpp"
#include "Player.hpp"
#include "DDSGameController.hpp"

class MyApp : public wxApp
{
public:
    bool OnInit() override;
};


wxIMPLEMENT_APP(MyApp);

class MyFrame : public wxFrame // This versions equivalent to the game clase
{
public:
    MyFrame();
    
    void updateGraphicalGameGrid()
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

    bool checkWin()
    {
        return gameGrid.verifyWin();
    }
 
private:
    void OnHello(wxCommandEvent& event);
    void OnExit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
    void OnTTTButtonClick(wxCommandEvent& event);
    void setupTTTDisplay();
    void setupTTTGame(std::string username1, std::string username2);
    void TTTsetup();
    void OnConnected(wxTimerEvent& event);
    void setupWaitingDisplay();
    void goIntoWaitingDisplay();
    void setCurrentGameStatus();

    Player p1{"1"};
    Player p2{"2"};
    Grid gameGrid;
    Player currentPlayer;

    wxBitmap xButtonImage;
    wxBitmap oButtonImage;
    wxBitmap blankButtonImage;
    wxSize buttonTileSize;
    int turnCounter;
    long last_message_count;
    DDSGameController my_controller_;
    wxPanel* waiting_panel_;
    wxPanel* ttt_panel_;
    wxPanel* current_game_panel_;
    wxTimer* timer;
    wxBitmapButton* buttons[9]; // Store button references
    int currentGame;
};

bool MyApp::OnInit()
{
    MyFrame *frame = new MyFrame();
    wxImage::AddHandler(new wxPNGHandler);
    wxInitAllImageHandlers();
    frame->Show();
    return true;
}

enum
{
    ID_Hello = 1
};

MyFrame::MyFrame()
        : wxFrame(nullptr, wxID_ANY, "DDS Game Suite Early Build", wxDefaultPosition, wxSize(600, 450))
{
    waiting_panel_ = new wxPanel(this);
    ttt_panel_ = new wxPanel(this);
    ttt_panel_->Hide();
    waiting_panel_->Hide();

    current_game_panel_= ttt_panel_;
    CreateStatusBar();

    wxBoxSizer* frameSizer = new wxBoxSizer(wxVERTICAL);
    frameSizer->Add(waiting_panel_, 1, wxEXPAND);
    frameSizer->Add(ttt_panel_, 1, wxEXPAND);
    SetSizer(frameSizer);
    Layout(); // Forces the layout to update visually
    
    timer = new wxTimer(this, wxID_ANY);

    currentGame = 1; // Set currentGame to tic tac toe

    TTTsetup();
    setupWaitingDisplay();

    goIntoWaitingDisplay();

    Bind(wxEVT_MENU, &MyFrame::OnExit, this, wxID_EXIT);
}

void MyFrame::goIntoWaitingDisplay()
{
    current_game_panel_->Hide();
    SetStatusText("Listening...");
    waiting_panel_->Show();
    Bind(wxEVT_TIMER, &MyFrame::OnConnected, this);
    timer->StartOnce(3000); // Simulate connection after 3 seconds
    Layout(); // Ensure everything redraws properly
}

void MyFrame::setupWaitingDisplay()
{
    waiting_panel_->SetBackgroundColour(*wxLIGHT_GREY);
    wxStaticText* welcomeText = new wxStaticText(waiting_panel_, wxID_ANY, "Waiting On Response from other player");
    welcomeText->SetFont(wxFontInfo(16).Bold());

    wxBoxSizer* waitingSizer = new wxBoxSizer(wxVERTICAL);

    // Add top spacer
    waitingSizer->AddStretchSpacer(1);

    // Center the text horizontally
    waitingSizer->Add(welcomeText, 0, wxALIGN_CENTER | wxBOTTOM, 20);

    // Add bottom spacer to balance the layout
    waitingSizer->AddStretchSpacer(1);

    waiting_panel_->SetSizer(waitingSizer);
    waiting_panel_->Layout();
}

void MyFrame::OnConnected(wxTimerEvent& event)
{
    waiting_panel_->Hide();
    setCurrentGameStatus();
    current_game_panel_->Show();
    Layout(); // Ensure everything redraws properly
}

void MyFrame::setCurrentGameStatus()
{
    if(currentGame == 1)
    {
        SetStatusText("It is currently Player " + currentPlayer.getName() + "'s turn. Symbol: (" + currentPlayer.getSymbol() + ") Turn: " + std::to_string(turnCounter));
    }
}

void MyFrame::TTTsetup()
{
    // Create a grid sizer for the 3x3 layout
    wxGridSizer* gridSizer = new wxGridSizer(3, 3, 5, 5); // 3 rows, 3 columns, 5px padding

    setupTTTGame("Player1", "Player2");
    setupTTTDisplay();

    // Create and add 9 bitmap buttons to the sizer
    for (int i = 0; i < 9; i++)
    {
        buttons[i] = new wxBitmapButton(ttt_panel_, i+1, blankButtonImage, wxDefaultPosition, buttonTileSize);
        buttons[i]->Bind(wxEVT_BUTTON, &MyFrame::OnTTTButtonClick, this);
        gridSizer->Add(buttons[i], 0, wxALIGN_CENTER , 5);
    }

    // Wrap gridSizer inside a box sizer to center it
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
    mainSizer->AddStretchSpacer(); // Pushes content down
    mainSizer->Add(gridSizer, 0, wxALIGN_CENTER | wxALL, 20); // Centers grid
    mainSizer->AddStretchSpacer(); // Pushes content up

    ttt_panel_->SetSizer(mainSizer);
}

/**
 * This is supposed to provide an option scale up UI elements based on a users input
 * but for now it's going unused since we have bigger fish to fry
 */
void MyFrame::setupTTTDisplay()
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

void MyFrame::setupTTTGame(std::string username1, std::string username2)
{
    gameGrid.resetGrid();
    p1.setSymbol("O");
    p2.setSymbol("X");
    p1.setName(username1);
    p2.setName(username2);
    currentPlayer = p1;
    turnCounter = 1;
}


void MyFrame::OnTTTButtonClick(wxCommandEvent& event)
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
    goIntoWaitingDisplay();
}

void MyFrame::OnExit(wxCommandEvent& event)
{
    waiting_panel_->Hide();
    ttt_panel_->Hide();

    delete waiting_panel_;
    delete ttt_panel_;
    delete current_game_panel_;
    delete timer;
    delete[] buttons;
    Close(true);
}