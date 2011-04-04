#include "main.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

#ifndef RUNNING_IN_CAR
    TestHarness harness_window;
    harness_window.show();
#else
    // hide the mouse cursor
    ShowCursor(false);

    // jump into the app
    Hardware *hw = new Hardware();
#endif

    return a.exec();
}
