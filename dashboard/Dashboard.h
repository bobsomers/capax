#ifndef DASHBOARD_H
#define DASHBOARD_H

#include <QWidget>
#include <QLabel>
#include <QGroupBox>
#include <QGridLayout>
#include <QPushButton>

#include "ucvtypes.h"
#include "Options.h"
#include "qneedleindicator.h"

// define the size of the touch screen
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

// the number of laps to show on the right hand side
#define NUMBER_OF_LAPS 10

// the total number of seconds for an entire run
#define MAX_RUN_TIME_SECS 2340 // 39 minutes

// tachometer range (in rpm)
#define TACH_MIN 0
#define TACH_MAX 8000

// speedometer range (in mph)
#define SPEED_MIN 0
#define SPEED_MAX 45

class Dashboard : public QWidget
{
    Q_OBJECT

public:
    Dashboard(QWidget *parent = 0);
    ~Dashboard();

public slots:
    // hardware interface
    void EcuUpdate(ecustate_t state);
    void GpsUpdate(gpsstate_t state);
    void TmrUpdate(int ms);

    void OptionsButtonClicked();
    void StartRunButtonClicked();

    void NextLap();

signals:
    void StartRun();
    void StopRun();

private:
    QLabel *lap_number[NUMBER_OF_LAPS], *lap_expected_time[NUMBER_OF_LAPS],
           *lap_actual_time[NUMBER_OF_LAPS], *run_status;
    QPushButton *start_button, *next_lap_button, *options_button;
    QNeedleIndicator *atach, *aspeed;
    QLabel *dtach, *dspeed, *davgspeed;

    int current_lap;
    int current_secs;
    int expected_lap_secs[NUMBER_OF_LAPS];
    double avgspeed_numerator;
    int avgspeed_denominator;
    bool run_in_progress;

    Options *options;
};

#endif // DASHBOARD_H
