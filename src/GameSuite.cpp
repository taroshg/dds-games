#include "MainFrame.hpp"

class MyApp : public wxApp
{
public:
    bool OnInit()
    {
        wxInitAllImageHandlers();

        MainFrame *frame = new MainFrame();
        frame->Show();
        return true;
    }
};  

wxIMPLEMENT_APP(MyApp);