#ifndef MAIN_H
#define MAIN_H

#include <QApplication>

#ifndef RUNNING_IN_CAR
    #include "TestHarness.h"
#else
    #include <windows.h>
    #include "Hardware.h"
#endif

#endif // MAIN_H
