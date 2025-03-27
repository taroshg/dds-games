#include <wx/wx.h>
#include "Grid.hpp"

class MyApp : public wxApp
{
public:
    bool OnInit() override;
};

wxIMPLEMENT_APP(MyApp);

class MyFrame : public wxFrame
{
public:
    MyFrame();
    ~MyFrame(); // Destructor to stop the timer when the frame is closed

private:
    void OnHello(wxCommandEvent& event);
    void OnExit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
    void OnTimer(wxTimerEvent& event); // Timer event handler

    wxTimer* timer; // Timer object
    int state;      // Variable to keep track of state
    wxStaticText* staticText; // Pointer to static text for displaying the grid string
};

enum
{
    ID_Hello = 1
};

bool MyApp::OnInit()
{
    MyFrame *frame = new MyFrame();
    frame->Show(true);
    return true;
}

MyFrame::MyFrame()
    : wxFrame(nullptr, wxID_ANY, "Hello World"),
      state(0) // Initialize state to 0
{
    wxMenu *menuFile = new wxMenu;
    menuFile->Append(ID_Hello, "&Hello...\tCtrl-H",
                     "Help string shown in status bar for this menu item");
    menuFile->AppendSeparator();
    menuFile->Append(wxID_EXIT);

    wxMenu *menuHelp = new wxMenu;
    menuHelp->Append(wxID_ABOUT);

    wxMenuBar *menuBar = new wxMenuBar;
    menuBar->Append(menuFile, "&File");
    menuBar->Append(menuHelp, "&Help");

    SetMenuBar(menuBar);

    CreateStatusBar();
    SetStatusText("Welcome to wxWidgets!");

    Grid testGrid;
    testGrid.setGridState(0);
    std::string gridString = testGrid.getGrid();
    
    // Create the static text control to display the grid string
    staticText = new wxStaticText(this, wxID_ANY, gridString, 
        wxPoint(10, 50), wxSize(200, 20));

    // Create the timer and bind it to the OnTimer method
    timer = new wxTimer(this, wxID_ANY);
    Bind(wxEVT_TIMER, &MyFrame::OnTimer, this);

    // Start the timer to trigger every second (1000 ms)
    timer->Start(1000);

    Bind(wxEVT_MENU, &MyFrame::OnHello, this, ID_Hello);
    Bind(wxEVT_MENU, &MyFrame::OnAbout, this, wxID_ABOUT);
    Bind(wxEVT_MENU, &MyFrame::OnExit, this, wxID_EXIT);
}

MyFrame::~MyFrame()
{
    if (timer) {
        timer->Stop(); // Stop the timer when the frame is destroyed
    }
}

void MyFrame::OnExit(wxCommandEvent& event)
{
    Close(true);
}

void MyFrame::OnAbout(wxCommandEvent& event)
{
    wxMessageBox("This is a wxWidgets Hello World example",
                 "About Hello World", wxOK | wxICON_INFORMATION);
}

void MyFrame::OnHello(wxCommandEvent& event)
{
    wxLogMessage("Hello world from wxWidgets!");
}

void MyFrame::OnTimer(wxTimerEvent& event)
{
    // Increment the state every second
    state++;

    // Create the updated grid string based on the new state
    Grid testGrid;
    testGrid.setGridState(state);
    std::string gridString = testGrid.getGrid();

    // Update the static text with the new grid string
    staticText->SetLabel(wxString(gridString));
}
