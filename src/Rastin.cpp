#include "rastin/MainWindow.h"


class MyApp : public wxApp
{
public:
    virtual bool OnInit();
};

wxIMPLEMENT_APP(MyApp);

bool MyApp::OnInit()
{

    MainWindow* window = new MainWindow();
    window->Fit();
    window->Center();
    window->Show();
    return true;
}


