#include "main.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

#ifdef DEVMODE
    TestHarness harness_window;
    harness_window.show();
#else
    // hide the mouse cursor
    //ShowCursor(false); // DEBUG

    // jump into the app
    Hardware *hw = new Hardware();
#endif

    return a.exec();
}
