#ifndef MAIN_H
#define MAIN_H

#include <QApplication>

#include "buildctrl.h"

#ifdef DEVMODE
    #include "TestHarness.h"
#else
    #include <windows.h>
    #include "Hardware.h"
#endif

#endif // MAIN_H
