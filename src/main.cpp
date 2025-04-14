#include <wx/wx.h>
#include <wx/bitmap.h>
#include <wx/image.h>

// RPS Game Class for Play with Friend
class RPSGameFriend : public wxFrame {
public:
    RPSGameFriend(wxWindow* parent, const wxString& title, int rounds, wxString player1, wxString player2)
        : wxFrame(parent, wxID_ANY, title, wxDefaultPosition, wxSize(500, 500)), rounds(rounds), player1Name(player1), player2Name(player2), player1Wins(0), player2Wins(0), ties(0), currentPlayer(0) {

        // Panel and Sizer setup
        wxPanel* panel = new wxPanel(this, wxID_ANY);
        wxBoxSizer* vbox = new wxBoxSizer(wxVERTICAL);

        // Name Labels (already split into player1Text, vsText, player2Text)
        wxFont vsFont(16, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD);
        wxFont nameFont(25, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD);

        player1Text = new wxStaticText(panel, wxID_ANY, player1Name);
        vsText = new wxStaticText(panel, wxID_ANY, "VS");
        player2Text = new wxStaticText(panel, wxID_ANY, player2Name);

        // Set font for all
        player1Text->SetFont(nameFont);
        vsText->SetFont(vsFont);
        player2Text->SetFont(nameFont);

        // Horizontal layout split into 3 parts
        wxBoxSizer* nameRow = new wxBoxSizer(wxHORIZONTAL);

        // Left side - player1 centered in left half
        wxBoxSizer* leftBox = new wxBoxSizer(wxHORIZONTAL);
        leftBox->AddStretchSpacer(1);
        leftBox->Add(player1Text, 0, wxALIGN_CENTER);
        leftBox->AddStretchSpacer(1);

        // Middle - just the "VS"
        wxBoxSizer* middleBox = new wxBoxSizer(wxHORIZONTAL);
        middleBox->Add(vsText, 0, wxALIGN_CENTER);

        // Right side - player2 centered in right half
        wxBoxSizer* rightBox = new wxBoxSizer(wxHORIZONTAL);
        rightBox->AddStretchSpacer(1);
        rightBox->Add(player2Text, 0, wxALIGN_CENTER);
        rightBox->AddStretchSpacer(1);

        // Each part takes 1/3 of the space
        nameRow->Add(leftBox, 1, wxEXPAND);
        nameRow->Add(middleBox, 0, wxALIGN_CENTER);
        nameRow->Add(rightBox, 1, wxEXPAND);

        vbox->Add(nameRow, 0, wxEXPAND | wxTOP | wxBOTTOM, 15);




        // 2 Default image 
        wxBoxSizer* imageRow = new wxBoxSizer(wxHORIZONTAL);

        
        // Load images
        /*wxBitmap defaultImage1("default.png", wxBITMAP_TYPE_PNG);
        wxBitmap defaultImage2("default.png", wxBITMAP_TYPE_PNG);

        default1map = new wxStaticBitmap(panel, wxID_ANY, defaultImage1);
        default2map = new wxStaticBitmap(panel, wxID_ANY, defaultImage2);

        imageRow->Add(default1map, 1, wxALIGN_CENTER);
        imageRow->Add(default2map, 1, wxALIGN_CENTER);

        vbox->Add(imageRow, 0, wxALIGN_CENTER | wxTOP, 20);*/

        wxBitmap defaultImage1("default.png", wxBITMAP_TYPE_PNG);
        wxBitmap defaultImage2("default.png", wxBITMAP_TYPE_PNG);

        // Create static bitmaps
        default1map = new wxStaticBitmap(panel, wxID_ANY, defaultImage1);
        default2map = new wxStaticBitmap(panel, wxID_ANY, defaultImage2);



        // Add the first image to the sizer (it will be placed on the left)
        imageRow->Add(default1map, 0, wxALIGN_CENTER_VERTICAL | wxALL, 10);  // Optional padding

        // Add a spacer between the images (increase width for more space)
        imageRow->AddSpacer(180);  // Adjust the value (e.g., 30 pixels) for more space

        // Add the second image to the sizer (it will be placed on the right)
        imageRow->Add(default2map, 0, wxALIGN_CENTER_VERTICAL | wxALL, 10);  // Optional padding

        // Create the main vertical sizer (vbox) if not already done
        vbox->Add(imageRow, 0, wxALIGN_CENTER | wxTOP, 20);  // This centers the image row in the parent container

        // Finally, set the sizer for the panel
        panel->SetSizer(vbox);


        




        

        // Result text
        resultText = new wxStaticText(panel, wxID_ANY, player1Name + "'s turn", wxDefaultPosition, wxSize(-1, 50));
        vbox->Add(resultText, 0, wxALIGN_CENTER | wxTOP | wxBOTTOM, 10);

        // Result Buttons for Rock, Paper, Scissors
        vbox->AddStretchSpacer(1);

        // Buttons for Rock, Paper, Scissors (in a row)
        wxBoxSizer* buttonRow = new wxBoxSizer(wxHORIZONTAL);
        rockButton = new wxButton(panel, wxID_ANY, "Rock");
        paperButton = new wxButton(panel, wxID_ANY, "Paper");
        scissorsButton = new wxButton(panel, wxID_ANY, "Scissors");

        buttonRow->Add(rockButton, 1, wxEXPAND | wxRIGHT, 10);
        buttonRow->Add(paperButton, 1, wxEXPAND | wxRIGHT, 10);
        buttonRow->Add(scissorsButton, 1, wxEXPAND);

        vbox->Add(buttonRow, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 10);

        // Quit button below RPS row
        quitButton = new wxButton(panel, wxID_ANY, "Quit");
        vbox->Add(quitButton, 0, wxALIGN_CENTER | wxBOTTOM, 20);

        panel->SetSizer(vbox);

        // Bind button events
        Bind(wxEVT_BUTTON, &RPSGameFriend::OnRockClick, this, rockButton->GetId());
        Bind(wxEVT_BUTTON, &RPSGameFriend::OnPaperClick, this, paperButton->GetId());
        Bind(wxEVT_BUTTON, &RPSGameFriend::OnScissorsClick, this, scissorsButton->GetId());
        Bind(wxEVT_BUTTON, &RPSGameFriend::OnQuitButtonClick, this, quitButton->GetId());

        // Initialize random seed
        srand(time(0));

        panel->Bind(wxEVT_SIZE, &RPSGameFriend::OnResize, this);
    }

private:
    wxStaticText* resultText;
    wxStaticText* player1Text;
    wxStaticText* vsText;
    wxStaticText* player2Text;
    wxButton* rockButton;
    wxButton* paperButton;
    wxButton* scissorsButton;
    wxButton* quitButton;

    wxStaticBitmap* default1map;
    wxStaticBitmap* default2map;



    int rounds;
    int player1Wins;
    int player2Wins;
    int ties;
    int currentPlayer; // 0 for player1, 1 for player2
    wxString player1Name;
    wxString player2Name;
    char player1Choice, player2Choice;

    void OnResize(wxSizeEvent& event) {
        // Get the new size of the window (panel)
        wxSize size = event.GetSize();
        int width = size.GetWidth();
        int height = size.GetHeight();

        // Resize images while maintaining aspect ratio
        wxImage defaultImage1 = wxBitmap("default.png", wxBITMAP_TYPE_PNG).ConvertToImage();
        wxImage defaultImage2 = wxBitmap("default.png", wxBITMAP_TYPE_PNG).ConvertToImage();

        defaultImage1.Rescale(width / 4, height / 4);  // Resize to 1/4 of window size
        defaultImage2.Rescale(width / 4, height / 4);
        

        // Update the static bitmaps with resized images
        default1map->SetBitmap(wxBitmap(defaultImage1));
        default2map->SetBitmap(wxBitmap(defaultImage2));
        

        event.Skip();
    }
    




    // Function to get the result of the game
    void ShowResult(char player1Choice, char player2Choice) {
        wxString result;
        if (player1Choice == player2Choice) {
            result = "It's a tie!";
            ties++;
        } else if ((player1Choice == 'r' && player2Choice == 's') ||
                   (player1Choice == 'p' && player2Choice == 'r') ||
                   (player1Choice == 's' && player2Choice == 'p')) {
            result = "You wins this round!";
            player1Wins++;
        } else {
            result = "You lose this round!";
            player2Wins++;
        }

        // Update the result text
        resultText->SetLabel(result + "\n\n" +
                             player1Name + " wins: " + wxString::Format("%d", player1Wins) + "\n" +
                             player2Name + " wins: " + wxString::Format("%d", player2Wins) + "\nTies: " + wxString::Format("%d", ties));

        // Check if the game is over
        int majorityWins = (rounds % 2 == 0) ? rounds / 2 + 1 : (rounds + 1) / 2;

        if (player1Wins >= majorityWins) {
            wxMessageBox(player1Name + " wins the game!", "Game Over", wxOK | wxICON_INFORMATION);
            Close(true);
        } else if (player2Wins >= majorityWins) {
            wxMessageBox(player2Name + " wins the game!", "Game Over", wxOK | wxICON_INFORMATION);
            Close(true);
        }
    }

    // Event handler for Rock Button
    void OnRockClick(wxCommandEvent& event) {
        if (currentPlayer == 0) {
            player1Choice = 'r';
            currentPlayer = 1;
            resultText->SetLabel("Wait for " + player2Name + "'s move");
        } else {
            player2Choice = 'r';
            currentPlayer = 0;
            ShowResult(player1Choice, player2Choice);
        }
    }

    // Event handler for Paper Button
    void OnPaperClick(wxCommandEvent& event) {
        if (currentPlayer == 0) {
            player1Choice = 'p';
            currentPlayer = 1;
            resultText->SetLabel("Wait for " + player2Name + "'s move");
        } else {
            player2Choice = 'p';
            currentPlayer = 0;
            ShowResult(player1Choice, player2Choice);
        }
    }

    // Event handler for Scissors Button
    void OnScissorsClick(wxCommandEvent& event) {
        if (currentPlayer == 0) {
            player1Choice = 's';
            currentPlayer = 1;
            resultText->SetLabel("Wait for " + player2Name + "'s move");
        } else {
            player2Choice = 's';
            currentPlayer = 0;
            ShowResult(player1Choice, player2Choice);
        }
    }

    // Event handler for Quit Button
    void OnQuitButtonClick(wxCommandEvent& event) {
        Close(true); // Quit the game window
    }
};

// Main Frame (Main Window)
class MyFrame : public wxFrame {
public:
    MyFrame(const wxString& title)
        : wxFrame(NULL, wxID_ANY, title, wxDefaultPosition, wxSize(500, 300)) {

        wxPanel* panel = new wxPanel(this, wxID_ANY);
        wxBoxSizer* vbox = new wxBoxSizer(wxVERTICAL);

        // Images and labels for Rock, Paper, Scissors
        wxBoxSizer* imageRow = new wxBoxSizer(wxHORIZONTAL);

        // Load images (ensure the images exist in your working directory)
        wxBitmap rockImage("rock.png", wxBITMAP_TYPE_PNG);
        wxBitmap paperImage("paper.png", wxBITMAP_TYPE_PNG);
        wxBitmap scissorsImage("scissors.png", wxBITMAP_TYPE_PNG);

        // Create static images for Rock, Paper, and Scissors
        rockBitmap = new wxStaticBitmap(panel, wxID_ANY, rockImage);
        paperBitmap = new wxStaticBitmap(panel, wxID_ANY, paperImage);
        scissorsBitmap = new wxStaticBitmap(panel, wxID_ANY, scissorsImage);

        // Add images to the row
        imageRow->Add(rockBitmap, 1, wxALIGN_CENTER);
        imageRow->Add(paperBitmap, 1, wxALIGN_CENTER);
        imageRow->Add(scissorsBitmap, 1, wxALIGN_CENTER);

        vbox->Add(imageRow, 0, wxALIGN_CENTER | wxTOP, 20);






        // Labels for each image
        wxBoxSizer* labelRow = new wxBoxSizer(wxHORIZONTAL);
        wxStaticText* rockLabel = new wxStaticText(panel, wxID_ANY, "Rock");
        wxStaticText* paperLabel = new wxStaticText(panel, wxID_ANY, "Paper");
        wxStaticText* scissorsLabel = new wxStaticText(panel, wxID_ANY, "Scissors");

        // left side rock
        wxBoxSizer* leftBox = new wxBoxSizer(wxHORIZONTAL);
        leftBox->AddStretchSpacer(1);
        leftBox->Add(rockLabel, 0, wxALIGN_CENTER);
        leftBox->AddStretchSpacer(1);


        // Middle - paper
        wxBoxSizer* middleBox = new wxBoxSizer(wxHORIZONTAL);
        middleBox->Add(paperLabel, 0, wxALIGN_CENTER);

        // Right side - scissor
        wxBoxSizer* rightBox = new wxBoxSizer(wxHORIZONTAL);
        rightBox->AddStretchSpacer(1);
        rightBox->Add(scissorsLabel, 0, wxALIGN_CENTER);
        rightBox->AddStretchSpacer(1);

        // Each part takes 1/3 of the space
        labelRow->Add(leftBox, 1, wxEXPAND);
        labelRow->Add(middleBox, 0, wxALIGN_CENTER);
        labelRow->Add(rightBox, 1, wxEXPAND);

        vbox->Add(labelRow, 0, wxEXPAND | wxTOP | wxBOTTOM, 15);




        //labelRow->Add(rockLabel, 1, wxALIGN_CENTER);
        //labelRow->Add(paperLabel, 1, wxALIGN_CENTER);
        //labelRow->Add(scissorsLabel, 1, wxALIGN_CENTER);

        //vbox->Add(labelRow, 0, wxALIGN_CENTER | wxTOP, 5);




        // Round selection buttons (3 or 5 rounds)
        wxBoxSizer* hbox1 = new wxBoxSizer(wxHORIZONTAL);
        wxButton* threeRoundsButton = new wxButton(panel, wxID_ANY, "3 Rounds");
        wxButton* fiveRoundsButton = new wxButton(panel, wxID_ANY, "5 Rounds");

        hbox1->Add(threeRoundsButton, 1, wxEXPAND | wxRIGHT, 10);
        hbox1->Add(fiveRoundsButton, 1, wxEXPAND);
        vbox->Add(hbox1, 0, wxEXPAND | wxALL, 50);

        // Quit button
        wxButton* quitButton = new wxButton(panel, wxID_ANY, "Quit");
        vbox->Add(quitButton, 0, wxALIGN_CENTER | wxALL, 0);

        panel->SetSizer(vbox);

        // Bind button events
        Bind(wxEVT_BUTTON, &MyFrame::OnThreeRoundsClick, this, threeRoundsButton->GetId());
        Bind(wxEVT_BUTTON, &MyFrame::OnFiveRoundsClick, this, fiveRoundsButton->GetId());
        Bind(wxEVT_BUTTON, &MyFrame::OnQuitButtonClick, this, quitButton->GetId());

        // Bind the window resize event
        panel->Bind(wxEVT_SIZE, &MyFrame::OnResize, this);
    }

private:
    wxStaticBitmap* rockBitmap;
    wxStaticBitmap* paperBitmap;
    wxStaticBitmap* scissorsBitmap;

    void OnResize(wxSizeEvent& event) {
        // Get the new size of the window (panel)
        wxSize size = event.GetSize();
        int width = size.GetWidth();
        int height = size.GetHeight();

        // Resize images while maintaining aspect ratio
        wxImage rockImage = wxBitmap("rock.png", wxBITMAP_TYPE_PNG).ConvertToImage();
        wxImage paperImage = wxBitmap("paper.png", wxBITMAP_TYPE_PNG).ConvertToImage();
        wxImage scissorsImage = wxBitmap("scissors.png", wxBITMAP_TYPE_PNG).ConvertToImage();

        rockImage.Rescale(width / 4, height / 4);  // Resize to 1/4 of window size
        paperImage.Rescale(width / 4, height / 4);
        scissorsImage.Rescale(width / 4, height / 4);

        // Update the static bitmaps with resized images
        rockBitmap->SetBitmap(wxBitmap(rockImage));
        paperBitmap->SetBitmap(wxBitmap(paperImage));
        scissorsBitmap->SetBitmap(wxBitmap(scissorsImage));

        event.Skip();
    }

    void OnThreeRoundsClick(wxCommandEvent& event) {
        StartGame(3);
    }

    void OnFiveRoundsClick(wxCommandEvent& event) {
        StartGame(5);
    }

    void StartGame(int rounds) {
        // Show name input window when playing with a friend
        /*wxTextEntryDialog player1Dialog(this, "Enter Player 1 name:", "Player 1 Name");
        if (player1Dialog.ShowModal() == wxID_OK) {
            wxString player1 = player1Dialog.GetValue();
            wxTextEntryDialog player2Dialog(this, "Enter Player 2 name:", "Player 2 Name");
            if (player2Dialog.ShowModal() == wxID_OK) {
                wxString player2 = player2Dialog.GetValue();
                RPSGameFriend* rpsGameFriend = new RPSGameFriend(this, "Rock Paper Scissors Game", rounds, player1, player2);
                rpsGameFriend->Show(true);
            }
        }*/
        RPSGameFriend* rpsGameFriend = new RPSGameFriend(this, "Rock Paper Scissors Game", rounds, "player1", "player2");
                rpsGameFriend->Show(true);
    }

    void OnQuitButtonClick(wxCommandEvent& event) {
        Close(true);
    }
};

class MyApp : public wxApp {
public:
    virtual bool OnInit() {
        MyFrame* frame = new MyFrame("Game Center");
        wxImage::AddHandler(new wxPNGHandler);
        wxInitAllImageHandlers();
        frame->Show(true);
        return true;
    }
};

wxIMPLEMENT_APP(MyApp);
